// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"

#include "AI/RealityWitnessComponent.h"
#include "AI/RealityWitnessTestActor.h"
#include "Components/BoxComponent.h"
#include "Developer/DeveloperModeComponent.h"
#include "Developer/RealityEditableComponent.h"
#include "Developer/RealityEditableTestActor.h"
#include "Engine/CollisionProfile.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "PhysicsEngine/BodyInstance.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "RealitySystem/RealityManagerSubsystem.h"
#include "RealitySystem/RealitySuspicionSettings.h"
#include "UI/DeveloperConsoleWidget.h"

namespace RealityFrictionTests
{
	FGameplayTag Tag(const TCHAR* Name) { return FGameplayTag::RequestGameplayTag(Name); }

	void SetCapabilities(URealityEditableComponent* Editable, const std::initializer_list<const TCHAR*> Names)
	{
		FGameplayTagContainer Tags;
		for (const TCHAR* Name : Names) Tags.AddTag(Tag(Name));
		Editable->SetSupportedCheats(Tags);
	}

	void ConfigurePhysics(UBoxComponent* Primitive, UPhysicalMaterial* Material = nullptr)
	{
		Primitive->SetBoxExtent(FVector(50.0f));
		Primitive->SetCollisionProfileName(UCollisionProfile::PhysicsActor_ProfileName);
		Primitive->SetSimulatePhysics(true);
		if (Material) Primitive->SetPhysMaterialOverride(Material);
	}

	ARealityEditableTestActor* SpawnFrictionActor(UWorld* World, const FVector& Location = FVector::ZeroVector, UPhysicalMaterial* Material = nullptr, bool bTwoBodies = false)
	{
		ARealityEditableTestActor* Actor = World->SpawnActor<ARealityEditableTestActor>(Location, FRotator::ZeroRotator);
		SetCapabilities(Actor->EditableComponent, { TEXT("Cheat.Friction") });
		ConfigurePhysics(Actor->PrimitiveA, Material);
		if (bTwoBodies) ConfigurePhysics(Actor->PrimitiveB, Material);
		Actor->EditableComponent->OnRealityCheatEvent.AddDynamic(Actor, &ARealityEditableTestActor::HandleRealityCheatEvent);
		return Actor;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRealityFrictionModificationTest,
	"Reality.Developer.FrictionModification",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::EngineFilter)

bool FRealityFrictionModificationTest::RunTest(const FString& Parameters)
{
	using namespace RealityFrictionTests;
	const FName WorldName = MakeUniqueObjectName(nullptr, UWorld::StaticClass(), NAME_None, EUniqueObjectNameOptions::GloballyUnique);
	FWorldContext& WorldContext = GEngine->CreateNewWorldContext(EWorldType::Game);
	UWorld* World = UWorld::CreateWorld(EWorldType::Game, false, WorldName, GetTransientPackage());
	if (!TestNotNull(TEXT("A Friction test world can be created"), World)) return false;
	World->AddToRoot();
	WorldContext.SetCurrentWorld(World);
	World->InitializeActorsForPlay(FURL());
	World->BeginPlay();
	AActor* Instigator = World->SpawnActor<AActor>();

	UPhysicalMaterial* SharedMaterial = NewObject<UPhysicalMaterial>(World);
	SharedMaterial->Friction = 0.8f;
	SharedMaterial->StaticFriction = 0.6f;
	SharedMaterial->bOverrideFrictionCombineMode = true;
	SharedMaterial->FrictionCombineMode = EFrictionCombineMode::Max;

	ARealityEditableTestActor* Unsupported = World->SpawnActor<ARealityEditableTestActor>();
	ConfigurePhysics(Unsupported->PrimitiveA, SharedMaterial);
	Unsupported->EditableComponent->OnRealityCheatEvent.AddDynamic(Unsupported, &ARealityEditableTestActor::HandleRealityCheatEvent);
	TestFalse(TEXT("Unsupported Friction Apply fails"), Unsupported->EditableComponent->ApplyFrictionModification(ERealityFrictionPreset::Low, Instigator));
	TestEqual(TEXT("Unsupported Friction emits no event"), Unsupported->CheatEventCount, 0);

	ARealityEditableTestActor* NoPhysics = World->SpawnActor<ARealityEditableTestActor>();
	SetCapabilities(NoPhysics->EditableComponent, { TEXT("Cheat.Friction") });
	TestFalse(TEXT("No-physics Friction Apply fails"), NoPhysics->EditableComponent->ApplyFrictionModification(ERealityFrictionPreset::Low, Instigator));
	TestFalse(TEXT("Friction never enables simulation"), NoPhysics->PrimitiveA->IsAnySimulatingPhysics());

	ARealityEditableTestActor* ActorA = SpawnFrictionActor(World, FVector::ZeroVector, SharedMaterial);
	ARealityEditableTestActor* ActorB = SpawnFrictionActor(World, FVector::ZeroVector, SharedMaterial);
	TestFalse(TEXT("Normal is a no-op before a Friction cycle"), ActorA->EditableComponent->ApplyFrictionModification(ERealityFrictionPreset::Normal, Instigator));
	TestTrue(TEXT("Low Friction Apply succeeds"), ActorA->EditableComponent->ApplyFrictionModification(ERealityFrictionPreset::Low, Instigator));
	TestTrue(TEXT("Friction reports modified"), ActorA->EditableComponent->IsFrictionModified());
	TestEqual(TEXT("Low is baseline-relative"), ActorA->EditableComponent->GetCurrentFriction(), 0.2f);
	UPhysicalMaterial* RuntimeA = ActorA->PrimitiveA->GetBodyInstance()->GetPhysMaterialOverride();
	TestNotEqual(TEXT("Edited Actor receives a unique runtime material"), RuntimeA, SharedMaterial);
	TestEqual(TEXT("Runtime material dynamic friction is Low"), RuntimeA ? RuntimeA->Friction : -1.0f, 0.2f);
	TestEqual(TEXT("Runtime material static friction is Low"), RuntimeA ? RuntimeA->StaticFriction : -1.0f, 0.15f);
	TestEqual(TEXT("Runtime material preserves combine override"), RuntimeA ? RuntimeA->bOverrideFrictionCombineMode : false, true);
	TestEqual(TEXT("Runtime material preserves combine mode"), RuntimeA ? RuntimeA->FrictionCombineMode.GetValue() : EFrictionCombineMode::Average, EFrictionCombineMode::Max);
	TestEqual(TEXT("Shared source material remains unchanged"), SharedMaterial->Friction, 0.8f);
	TestEqual(TEXT("Unedited Actor keeps shared material"), ActorB->PrimitiveA->GetBodyInstance()->GetPhysMaterialOverride(), SharedMaterial);
	TestEqual(TEXT("Unedited Actor effective friction remains unchanged"), ActorB->PrimitiveA->GetBodyInstance()->GetSimplePhysicalMaterial()->Friction, 0.8f);
	TestEqual(TEXT("Friction event tag is correct"), ActorA->LastCheatEvent.CheatTag, Tag(TEXT("Cheat.Friction")));
	TestEqual(TEXT("Friction event target is correct"), ActorA->LastCheatEvent.TargetActor.Get(), static_cast<AActor*>(ActorA));
	TestEqual(TEXT("Friction event instigator is correct"), ActorA->LastCheatEvent.InstigatingActor.Get(), Instigator);
	const int32 LowEvents = ActorA->CheatEventCount;
	TestFalse(TEXT("Same Friction preset is rejected"), ActorA->EditableComponent->ApplyFrictionModification(ERealityFrictionPreset::Low, Instigator));
	TestEqual(TEXT("Same Friction preset emits no event"), ActorA->CheatEventCount, LowEvents);

	TestTrue(TEXT("Zero Friction switch succeeds"), ActorA->EditableComponent->ApplyFrictionModification(ERealityFrictionPreset::Zero, Instigator));
	TestEqual(TEXT("Zero derives from baseline"), ActorA->EditableComponent->GetCurrentFriction(), 0.0f);
	TestTrue(TEXT("High Friction switch succeeds"), ActorA->EditableComponent->ApplyFrictionModification(ERealityFrictionPreset::High, Instigator));
	TestEqual(TEXT("High derives from baseline without compounding"), ActorA->EditableComponent->GetCurrentFriction(), 3.2f);
	TestTrue(TEXT("Active Normal switch succeeds"), ActorA->EditableComponent->ApplyFrictionModification(ERealityFrictionPreset::Normal, Instigator));
	TestTrue(TEXT("Active Normal remains logically modified"), ActorA->EditableComponent->IsFrictionModified());
	TestEqual(TEXT("Active Normal restores original override reference"), ActorA->PrimitiveA->GetBodyInstance()->GetPhysMaterialOverride(), SharedMaterial);
	TestTrue(TEXT("Explicit Friction Restore succeeds"), ActorA->EditableComponent->RestoreFrictionModification(Instigator));
	TestFalse(TEXT("Restore ends Friction cycle"), ActorA->EditableComponent->IsFrictionModified());
	TestEqual(TEXT("Restore retains exact original reference"), ActorA->PrimitiveA->GetBodyInstance()->GetPhysMaterialOverride(), SharedMaterial);
	TestEqual(TEXT("Restore event operation is correct"), ActorA->LastCheatEvent.Operation, ERealityCheatOperation::Restore);

	UPhysicalMaterial* LaterMaterial = NewObject<UPhysicalMaterial>(World);
	LaterMaterial->Friction = 0.35f;
	LaterMaterial->StaticFriction = 0.25f;
	ActorA->PrimitiveA->SetPhysMaterialOverride(LaterMaterial);
	TestTrue(TEXT("Later cycle captures new legitimate material"), ActorA->EditableComponent->ApplyFrictionModification(ERealityFrictionPreset::High, Instigator));
	TestEqual(TEXT("Later High uses new baseline"), ActorA->EditableComponent->GetCurrentFriction(), 1.4f);
	TestTrue(TEXT("Later Restore succeeds"), ActorA->EditableComponent->RestoreFrictionModification(Instigator));
	TestEqual(TEXT("Later Restore returns new material reference"), ActorA->PrimitiveA->GetBodyInstance()->GetPhysMaterialOverride(), LaterMaterial);

	ARealityEditableTestActor* Multi = SpawnFrictionActor(World, FVector::ZeroVector, SharedMaterial, true);
	UPhysicalMaterial* SecondMaterial = NewObject<UPhysicalMaterial>(World);
	SecondMaterial->Friction = 0.4f;
	Multi->PrimitiveB->SetPhysMaterialOverride(SecondMaterial);
	TestEqual(TEXT("Two bodies are Friction eligible"), Multi->EditableComponent->GetEligibleFrictionComponentCount(), 2);
	TestTrue(TEXT("Multi-body Friction Apply succeeds"), Multi->EditableComponent->ApplyFrictionModification(ERealityFrictionPreset::Low, Instigator));
	TestEqual(TEXT("Body A has its own runtime material"), Multi->PrimitiveA->GetBodyInstance()->GetSimplePhysicalMaterial()->Friction, 0.2f);
	TestEqual(TEXT("Body B has its own baseline"), Multi->PrimitiveB->GetBodyInstance()->GetSimplePhysicalMaterial()->Friction, 0.1f);
	Multi->PrimitiveB->DestroyComponent();
	TestTrue(TEXT("Restore skips destroyed body and restores survivor"), Multi->EditableComponent->RestoreFrictionModification(Instigator));
	TestEqual(TEXT("Survivor returns exact original reference"), Multi->PrimitiveA->GetBodyInstance()->GetPhysMaterialOverride(), SharedMaterial);

	ARealityEditableTestActor* AllDestroyed = SpawnFrictionActor(World, FVector::ZeroVector, SharedMaterial);
	TestTrue(TEXT("Friction starts before target destruction"), AllDestroyed->EditableComponent->ApplyFrictionModification(ERealityFrictionPreset::Zero, Instigator));
	const int32 EventsBeforeDestruction = AllDestroyed->CheatEventCount;
	AllDestroyed->PrimitiveA->DestroyComponent();
	TestFalse(TEXT("All-destroyed Restore fails safely"), AllDestroyed->EditableComponent->RestoreFrictionModification(Instigator));
	TestFalse(TEXT("All-destroyed Restore clears active state"), AllDestroyed->EditableComponent->IsFrictionModified());
	TestEqual(TEXT("All-destroyed Restore emits no false event"), AllDestroyed->CheatEventCount, EventsBeforeDestruction);

	TestTrue(TEXT("Actor A independent Zero succeeds"), ActorA->EditableComponent->ApplyFrictionModification(ERealityFrictionPreset::Zero, Instigator));
	TestTrue(TEXT("Actor B independent High succeeds"), ActorB->EditableComponent->ApplyFrictionModification(ERealityFrictionPreset::High, Instigator));
	TestTrue(TEXT("Actor A restores independently"), ActorA->EditableComponent->RestoreFrictionModification(Instigator));
	TestTrue(TEXT("Actor B remains modified"), ActorB->EditableComponent->IsFrictionModified());
	TestTrue(TEXT("Actor B restores independently"), ActorB->EditableComponent->RestoreFrictionModification(Instigator));

	ARealityEditableTestActor* AllCheats = SpawnFrictionActor(World, FVector(150, 0, 0), SharedMaterial);
	SetCapabilities(AllCheats->EditableComponent, { TEXT("Cheat.Collision"), TEXT("Cheat.Scale"), TEXT("Cheat.Gravity"), TEXT("Cheat.Mass"), TEXT("Cheat.Friction") });
	TestTrue(TEXT("Friction activates on all-cheat Actor"), AllCheats->EditableComponent->ApplyFrictionModification(ERealityFrictionPreset::Zero, Instigator));
	TestTrue(TEXT("Mass coexists with Friction"), AllCheats->EditableComponent->ApplyMassModification(ERealityMassPreset::Quadruple, Instigator));
	TestTrue(TEXT("Scale coexists with Friction"), AllCheats->EditableComponent->ApplyScaleModification(ERealityScalePreset::Double, Instigator));
	TestTrue(TEXT("Scale recreation retains Friction"), AllCheats->EditableComponent->IsFrictionModified() && AllCheats->EditableComponent->GetCurrentFriction() == 0.0f);
	TestTrue(TEXT("Gravity coexists with Friction"), AllCheats->EditableComponent->ApplyGravityModification(ERealityGravityPreset::Low, Instigator));
	TestTrue(TEXT("Collision coexists with Friction"), AllCheats->EditableComponent->ApplyCollisionModification(Instigator));
	TestTrue(TEXT("All five properties are active"), AllCheats->EditableComponent->IsCollisionModified() && AllCheats->EditableComponent->IsScaleModified() && AllCheats->EditableComponent->IsGravityModified() && AllCheats->EditableComponent->IsMassModified() && AllCheats->EditableComponent->IsFrictionModified());
	TestTrue(TEXT("Collision Restore keeps Friction active"), AllCheats->EditableComponent->RestoreCollisionModification(Instigator));
	TestTrue(TEXT("Collision body recreation reapplies private Friction material"), AllCheats->PrimitiveA->GetBodyInstance()->GetPhysMaterialOverride() != SharedMaterial);
	TestTrue(TEXT("Scale Restore keeps Friction active"), AllCheats->EditableComponent->RestoreScaleModification(Instigator));
	TestTrue(TEXT("Mass Restore keeps Friction active"), AllCheats->EditableComponent->RestoreMassModification(Instigator));
	TestTrue(TEXT("Gravity Restore keeps Friction active"), AllCheats->EditableComponent->RestoreGravityModification(Instigator));
	TestTrue(TEXT("Friction restores last"), AllCheats->EditableComponent->RestoreFrictionModification(Instigator));

	ARealityEditableTestActor* CollisionFirst = SpawnFrictionActor(World, FVector::ZeroVector, SharedMaterial);
	SetCapabilities(CollisionFirst->EditableComponent, { TEXT("Cheat.Collision"), TEXT("Cheat.Friction") });
	TestTrue(TEXT("Collision can disable before Friction Apply"), CollisionFirst->EditableComponent->ApplyCollisionModification(Instigator));
	TestTrue(TEXT("Friction can capture while Collision is off from retained simulation intent"), CollisionFirst->EditableComponent->ApplyFrictionModification(ERealityFrictionPreset::Low, Instigator));
	TestTrue(TEXT("Collision Restore succeeds after Friction Apply"), CollisionFirst->EditableComponent->RestoreCollisionModification(Instigator));
	TestEqual(TEXT("Collision Restore reapplies active Low Friction"), CollisionFirst->PrimitiveA->GetBodyInstance()->GetSimplePhysicalMaterial()->Friction, 0.2f);
	TestTrue(TEXT("Friction Restore succeeds after Collision recreation"), CollisionFirst->EditableComponent->RestoreFrictionModification(Instigator));

	TestFalse(TEXT("Invalid Friction preset fails safely"), ActorA->EditableComponent->ApplyFrictionModification(static_cast<ERealityFrictionPreset>(255), Instigator));
	URealityEditableComponent* Ownerless = NewObject<URealityEditableComponent>();
	SetCapabilities(Ownerless, { TEXT("Cheat.Friction") });
	TestFalse(TEXT("Ownerless Friction Apply fails safely"), Ownerless->ApplyFrictionModification(ERealityFrictionPreset::Low, Instigator));

	URealityManagerSubsystem* Manager = World->GetSubsystem<URealityManagerSubsystem>();
	Manager->ResetRealityState();
	const URealitySuspicionSettings* Settings = GetDefault<URealitySuspicionSettings>();
	float Base = 0.0f;
	TestTrue(TEXT("Friction has configured Suspicion"), Settings->FindBaseSuspicion(Tag(TEXT("Cheat.Friction")), Base));
	TestEqual(TEXT("Friction base Suspicion is 10"), Base, 10.0f);
	float Context = 0.0f;
	TestFalse(TEXT("Friction has no invented Explosion Context"), Settings->FindContextReduction(Tag(TEXT("Cheat.Friction")), Tag(TEXT("Context.Explosion")), Context));
	ARealityEditableTestActor* Integrated = SpawnFrictionActor(World, FVector(500, 0, 0), SharedMaterial);
	TestTrue(TEXT("No-witness Friction Apply succeeds"), Integrated->EditableComponent->ApplyFrictionModification(ERealityFrictionPreset::Low, Instigator));
	TestEqual(TEXT("No-witness Friction adds 10"), Manager->GetSuspicion(), 10.0f);
	TestTrue(TEXT("Friction Restore succeeds"), Integrated->EditableComponent->RestoreFrictionModification(Instigator));
	TestEqual(TEXT("Friction Restore remains -5"), Manager->GetSuspicion(), 5.0f);

	ARealityWitnessTestActor* Witness = World->SpawnActor<ARealityWitnessTestActor>(FVector::ZeroVector, FRotator::ZeroRotator);
	Witness->WitnessComponent->WitnessType = Tag(TEXT("Witness.Human"));
	Witness->WitnessComponent->ObservationRange = 1500.0f;
	Witness->WitnessComponent->FieldOfViewDegrees = 90.0f;
	Witness->WitnessComponent->bRequireLineOfSight = false;
	if (!Witness->HasActorBegunPlay()) Witness->DispatchBeginPlay();
	Manager->ResetRealityState();
	TestTrue(TEXT("Witnessed Friction Apply succeeds"), Integrated->EditableComponent->ApplyFrictionModification(ERealityFrictionPreset::High, Instigator));
	TestEqual(TEXT("Human Witness adds existing +10"), Manager->GetSuspicion(), 20.0f);
	const FRealityProcessedCheatRecord* Record = Manager->GetMostRecentEvent();
	if (TestNotNull(TEXT("Witnessed Friction record exists"), Record))
	{
		TestEqual(TEXT("Record base is 10"), Record->BaseSuspicionDelta, 10.0f);
		TestEqual(TEXT("Record Witness is 10"), Record->WitnessSuspicionDelta, 10.0f);
		TestEqual(TEXT("Record Context is zero"), Record->ContextSuspicionReduction, 0.0f);
	}
	Integrated->EditableComponent->RestoreFrictionModification(Instigator);

	AActor* Player = World->SpawnActor<AActor>();
	UDeveloperModeComponent* Developer = NewObject<UDeveloperModeComponent>(Player);
	Player->AddInstanceComponent(Developer);
	Developer->RegisterComponent();
	UDeveloperConsoleWidget* Widget = NewObject<UDeveloperConsoleWidget>(World);
	Widget->SetDeveloperModeComponent(Developer);
	Developer->EnterDeveloperMode();
	TestTrue(TEXT("Console exposes Friction capability"), Widget->IsCheatSectionAvailable(Tag(TEXT("Cheat.Friction"))));
	URealityEditableComponent* ConsoleTarget = Developer->GetFocusedEditableComponent();
	TestTrue(TEXT("Console Friction preset action succeeds"), Widget->ExecuteFrictionPreset(ERealityFrictionPreset::Low));
	TestTrue(TEXT("Console target reports active Friction"), ConsoleTarget && ConsoleTarget->IsFrictionModified());
	TestTrue(TEXT("Console Friction Restore succeeds"), Widget->ExecuteFrictionRestore());
	TestFalse(TEXT("Console Friction Restore ends cycle"), ConsoleTarget && ConsoleTarget->IsFrictionModified());
	Developer->ExitDeveloperMode();

	World->DestroyWorld(false);
	World->SetPhysicsScene(nullptr);
	GEngine->DestroyWorldContext(World);
	World->RemoveFromRoot();
	return true;
}

#endif
