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
#include "RealitySystem/RealityContextComponent.h"
#include "RealitySystem/RealityContextTestActor.h"
#include "RealitySystem/RealityManagerSubsystem.h"
#include "RealitySystem/RealitySuspicionSettings.h"
#include "UI/DeveloperConsoleWidget.h"

namespace RealityMassTests
{
	FGameplayTag Tag(const TCHAR* Name) { return FGameplayTag::RequestGameplayTag(Name); }

	void SetCapabilities(URealityEditableComponent* Editable, const std::initializer_list<const TCHAR*> Names)
	{
		FGameplayTagContainer Capabilities;
		for (const TCHAR* Name : Names) Capabilities.AddTag(Tag(Name));
		Editable->SetSupportedCheats(Capabilities);
	}

	void ConfigurePhysics(UBoxComponent* Primitive, const float MassScale = 1.0f)
	{
		Primitive->SetBoxExtent(FVector(50.0f));
		Primitive->SetCollisionProfileName(UCollisionProfile::PhysicsActor_ProfileName);
		Primitive->SetSimulatePhysics(true);
		Primitive->SetMassScale(NAME_None, MassScale);
	}

	ARealityEditableTestActor* SpawnMassActor(UWorld* World, const FVector& Location = FVector::ZeroVector, const bool bTwoBodies = false)
	{
		ARealityEditableTestActor* Actor = World->SpawnActor<ARealityEditableTestActor>(Location, FRotator::ZeroRotator);
		SetCapabilities(Actor->EditableComponent, { TEXT("Cheat.Mass") });
		ConfigurePhysics(Actor->PrimitiveA);
		if (bTwoBodies) ConfigurePhysics(Actor->PrimitiveB, 1.5f);
		Actor->EditableComponent->OnRealityCheatEvent.AddDynamic(Actor, &ARealityEditableTestActor::HandleRealityCheatEvent);
		return Actor;
	}

	bool NearlyEqual(const float A, const float B) { return FMath::IsNearlyEqual(A, B, FMath::Max(0.01f, FMath::Abs(B) * 0.01f)); }
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRealityMassModificationTest,
	"Reality.Developer.MassModification",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::EngineFilter)

bool FRealityMassModificationTest::RunTest(const FString& Parameters)
{
	using namespace RealityMassTests;
	const FName WorldName = MakeUniqueObjectName(nullptr, UWorld::StaticClass(), NAME_None, EUniqueObjectNameOptions::GloballyUnique);
	FWorldContext& WorldContext = GEngine->CreateNewWorldContext(EWorldType::Game);
	UWorld* World = UWorld::CreateWorld(EWorldType::Game, false, WorldName, GetTransientPackage());
	if (!TestNotNull(TEXT("A Mass test world can be created"), World)) return false;
	World->AddToRoot();
	WorldContext.SetCurrentWorld(World);
	World->InitializeActorsForPlay(FURL());
	World->BeginPlay();
	AActor* Instigator = World->SpawnActor<AActor>();

	ARealityEditableTestActor* Unsupported = World->SpawnActor<ARealityEditableTestActor>();
	ConfigurePhysics(Unsupported->PrimitiveA);
	Unsupported->EditableComponent->OnRealityCheatEvent.AddDynamic(Unsupported, &ARealityEditableTestActor::HandleRealityCheatEvent);
	TestFalse(TEXT("Unsupported Mass Apply fails"), Unsupported->EditableComponent->ApplyMassModification(ERealityMassPreset::Half, Instigator));
	TestEqual(TEXT("Unsupported Mass emits no event"), Unsupported->CheatEventCount, 0);

	ARealityEditableTestActor* NoPhysics = World->SpawnActor<ARealityEditableTestActor>();
	SetCapabilities(NoPhysics->EditableComponent, { TEXT("Cheat.Mass") });
	TestFalse(TEXT("Mass rejects an Actor without simulated bodies"), NoPhysics->EditableComponent->ApplyMassModification(ERealityMassPreset::Half, Instigator));
	TestFalse(TEXT("Mass never enables simulation"), NoPhysics->PrimitiveA->IsAnySimulatingPhysics());
	TestEqual(TEXT("No-physics Actor has zero eligible bodies"), NoPhysics->EditableComponent->GetEligibleMassComponentCount(), 0);

	ARealityEditableTestActor* Actor = SpawnMassActor(World);
	FBodyInstance* Body = Actor->PrimitiveA->GetBodyInstance();
	TestNotNull(TEXT("Mass fixture has a body instance"), Body);
	Actor->PrimitiveA->SetMassScale(NAME_None, 1.75f);
	const float Baseline = Actor->PrimitiveA->GetMass();
	TestTrue(TEXT("Fixture baseline mass is valid"), Baseline > 0.0f);
	TestFalse(TEXT("1.0x is a no-op before a Mass cycle"), Actor->EditableComponent->ApplyMassModification(ERealityMassPreset::One, Instigator));
	TestTrue(TEXT("0.5x Mass Apply succeeds"), Actor->EditableComponent->ApplyMassModification(ERealityMassPreset::Half, Instigator));
	TestTrue(TEXT("0.5x is baseline-relative"), NearlyEqual(Actor->PrimitiveA->GetMass(), Baseline * 0.5f));
	TestTrue(TEXT("Mass reports modified"), Actor->EditableComponent->IsMassModified());
	TestEqual(TEXT("Mass event tag is correct"), Actor->LastCheatEvent.CheatTag, Tag(TEXT("Cheat.Mass")));
	TestEqual(TEXT("Mass event target is correct"), Actor->LastCheatEvent.TargetActor.Get(), static_cast<AActor*>(Actor));
	TestEqual(TEXT("Mass event instigator is correct"), Actor->LastCheatEvent.InstigatingActor.Get(), Instigator);
	const int32 EventsAfterHalf = Actor->CheatEventCount;
	TestFalse(TEXT("Repeated active preset is rejected"), Actor->EditableComponent->ApplyMassModification(ERealityMassPreset::Half, Instigator));
	TestEqual(TEXT("Repeated preset emits no duplicate event"), Actor->CheatEventCount, EventsAfterHalf);
	TestTrue(TEXT("2.0x switches relative to baseline"), Actor->EditableComponent->ApplyMassModification(ERealityMassPreset::Double, Instigator));
	TestTrue(TEXT("2.0x does not compound"), NearlyEqual(Actor->PrimitiveA->GetMass(), Baseline * 2.0f));
	TestTrue(TEXT("4.0x switches relative to baseline"), Actor->EditableComponent->ApplyMassModification(ERealityMassPreset::Quadruple, Instigator));
	TestTrue(TEXT("4.0x does not compound"), NearlyEqual(Actor->PrimitiveA->GetMass(), Baseline * 4.0f));
	TestTrue(TEXT("Active 1.0x returns to captured effective baseline"), Actor->EditableComponent->ApplyMassModification(ERealityMassPreset::One, Instigator));
	TestTrue(TEXT("Active 1.0x preserves effective baseline"), NearlyEqual(Actor->PrimitiveA->GetMass(), Baseline));
	TestTrue(TEXT("Active 1.0x remains logically modified"), Actor->EditableComponent->IsMassModified());
	TestTrue(TEXT("Restore ends Mass cycle"), Actor->EditableComponent->RestoreMassModification(Instigator));
	TestFalse(TEXT("Mass is no longer modified"), Actor->EditableComponent->IsMassModified());
	TestTrue(TEXT("Authored MassScale restores exactly"), NearlyEqual(Actor->PrimitiveA->GetMassScale(), 1.75f));
	TestFalse(TEXT("Calculated-mass mode restores exactly"), Body->bOverrideMass);
	TestEqual(TEXT("Mass Restore event is correct"), Actor->LastCheatEvent.Operation, ERealityCheatOperation::Restore);

	Actor->PrimitiveA->SetMassOverrideInKg(NAME_None, 37.0f, true);
	const float OverrideBaseline = Actor->PrimitiveA->GetMass();
	TestTrue(TEXT("Designer override provides baseline"), NearlyEqual(OverrideBaseline, 37.0f));
	TestTrue(TEXT("Override baseline accepts Quarter"), Actor->EditableComponent->ApplyMassModification(ERealityMassPreset::Quarter, Instigator));
	TestTrue(TEXT("Override baseline is multiplied"), NearlyEqual(Actor->PrimitiveA->GetMass(), OverrideBaseline * 0.25f));
	TestTrue(TEXT("Override baseline restores"), Actor->EditableComponent->RestoreMassModification(Instigator));
	TestTrue(TEXT("Designer override mode restores"), Body->bOverrideMass);
	TestTrue(TEXT("Designer override value restores"), NearlyEqual(Body->GetMassOverride(), 37.0f));

	Actor->PrimitiveA->SetMassOverrideInKg(NAME_None, 61.0f, true);
	TestTrue(TEXT("Later cycle captures new legitimate state"), Actor->EditableComponent->ApplyMassModification(ERealityMassPreset::Half, Instigator));
	TestTrue(TEXT("Later cycle restores"), Actor->EditableComponent->RestoreMassModification(Instigator));
	TestTrue(TEXT("Later legitimate override is retained"), Body->bOverrideMass && NearlyEqual(Body->GetMassOverride(), 61.0f));

	ARealityEditableTestActor* MultiBody = SpawnMassActor(World, FVector::ZeroVector, true);
	MultiBody->PrimitiveA->SetMassOverrideInKg(NAME_None, 20.0f, true);
	MultiBody->PrimitiveB->SetMassOverrideInKg(NAME_None, 40.0f, true);
	TestEqual(TEXT("Two simulated owner-local components are eligible"), MultiBody->EditableComponent->GetEligibleMassComponentCount(), 2);
	TestTrue(TEXT("Multi-body Apply succeeds"), MultiBody->EditableComponent->ApplyMassModification(ERealityMassPreset::Half, Instigator));
	TestTrue(TEXT("Body A uses its own baseline"), NearlyEqual(MultiBody->PrimitiveA->GetMass(), 10.0f));
	TestTrue(TEXT("Body B uses its own baseline"), NearlyEqual(MultiBody->PrimitiveB->GetMass(), 20.0f));
	MultiBody->PrimitiveB->DestroyComponent();
	TestTrue(TEXT("Restore skips a destroyed body and restores survivors"), MultiBody->EditableComponent->RestoreMassModification(Instigator));
	TestTrue(TEXT("Surviving override restores independently"), MultiBody->PrimitiveA->GetBodyInstance()->bOverrideMass && NearlyEqual(MultiBody->PrimitiveA->GetMass(), 20.0f));

	ARealityEditableTestActor* AllDestroyed = SpawnMassActor(World);
	TestTrue(TEXT("Mass starts before body destruction"), AllDestroyed->EditableComponent->ApplyMassModification(ERealityMassPreset::Double, Instigator));
	const int32 BeforeDestroyedRestore = AllDestroyed->CheatEventCount;
	AllDestroyed->PrimitiveA->DestroyComponent();
	TestFalse(TEXT("All-destroyed Restore fails safely"), AllDestroyed->EditableComponent->RestoreMassModification(Instigator));
	TestFalse(TEXT("All-destroyed Restore clears active state"), AllDestroyed->EditableComponent->IsMassModified());
	TestEqual(TEXT("All-destroyed Restore emits no false event"), AllDestroyed->CheatEventCount, BeforeDestroyedRestore);

	ARealityEditableTestActor* ActorA = SpawnMassActor(World);
	ARealityEditableTestActor* ActorB = SpawnMassActor(World);
	TestTrue(TEXT("Actor A accepts independent Quarter"), ActorA->EditableComponent->ApplyMassModification(ERealityMassPreset::Quarter, Instigator));
	TestTrue(TEXT("Actor B accepts independent Quadruple"), ActorB->EditableComponent->ApplyMassModification(ERealityMassPreset::Quadruple, Instigator));
	TestTrue(TEXT("Actor A restores independently"), ActorA->EditableComponent->RestoreMassModification(Instigator));
	TestTrue(TEXT("Actor B remains modified"), ActorB->EditableComponent->IsMassModified());
	TestTrue(TEXT("Actor B restores independently"), ActorB->EditableComponent->RestoreMassModification(Instigator));

	ARealityEditableTestActor* AllCheats = SpawnMassActor(World, FVector(150.0f, 0.0f, 0.0f));
	SetCapabilities(AllCheats->EditableComponent, { TEXT("Cheat.Collision"), TEXT("Cheat.Scale"), TEXT("Cheat.Gravity"), TEXT("Cheat.Mass") });
	const float AllCheatsBaseline = AllCheats->PrimitiveA->GetMass();
	TestTrue(TEXT("Mass activates first"), AllCheats->EditableComponent->ApplyMassModification(ERealityMassPreset::Half, Instigator));
	TestTrue(TEXT("Scale activates while Mass is active"), AllCheats->EditableComponent->ApplyScaleModification(ERealityScalePreset::Double, Instigator));
	TestTrue(TEXT("Scale change reasserts baseline-relative Mass"), NearlyEqual(AllCheats->PrimitiveA->GetMass(), AllCheatsBaseline * 0.5f));
	TestTrue(TEXT("Gravity coexists with Mass"), AllCheats->EditableComponent->ApplyGravityModification(ERealityGravityPreset::Zero, Instigator));
	TestTrue(TEXT("Collision coexists with Mass"), AllCheats->EditableComponent->ApplyCollisionModification(Instigator));
	TestTrue(TEXT("All four properties are independently active"), AllCheats->EditableComponent->IsMassModified() && AllCheats->EditableComponent->IsScaleModified() && AllCheats->EditableComponent->IsGravityModified() && AllCheats->EditableComponent->IsCollisionModified());
	TestTrue(TEXT("Scale Restore leaves Mass active"), AllCheats->EditableComponent->RestoreScaleModification(Instigator));
	TestTrue(TEXT("Mass remains logically baseline-relative after Scale Restore while Collision is off"), AllCheats->EditableComponent->IsMassModified() && NearlyEqual(AllCheats->EditableComponent->GetCurrentEffectiveMassKg(), AllCheatsBaseline * 0.5f));
	TestTrue(TEXT("Gravity Restore leaves Mass active"), AllCheats->EditableComponent->RestoreGravityModification(Instigator));
	TestTrue(TEXT("Collision Restore leaves Mass active"), AllCheats->EditableComponent->RestoreCollisionModification(Instigator));
	TestTrue(TEXT("Collision Restore reasserts live baseline-relative Mass"), NearlyEqual(AllCheats->PrimitiveA->GetMass(), AllCheatsBaseline * 0.5f));
	TestTrue(TEXT("Mass restores last without changing other states"), AllCheats->EditableComponent->RestoreMassModification(Instigator));

	TestFalse(TEXT("Invalid Mass preset fails safely"), Actor->EditableComponent->ApplyMassModification(static_cast<ERealityMassPreset>(255), Instigator));
	URealityEditableComponent* Ownerless = NewObject<URealityEditableComponent>();
	SetCapabilities(Ownerless, { TEXT("Cheat.Mass") });
	TestFalse(TEXT("Ownerless Mass Apply fails safely"), Ownerless->ApplyMassModification(ERealityMassPreset::Half, Instigator));

	URealityManagerSubsystem* Manager = World->GetSubsystem<URealityManagerSubsystem>();
	Manager->ResetRealityState();
	const URealitySuspicionSettings* Settings = GetDefault<URealitySuspicionSettings>();
	float MassSuspicion = 0.0f;
	TestTrue(TEXT("Mass has a configured base rule"), Settings->FindBaseSuspicion(Tag(TEXT("Cheat.Mass")), MassSuspicion));
	TestEqual(TEXT("Mass base Suspicion is 10"), MassSuspicion, 10.0f);
	float ContextReduction = 0.0f;
	TestFalse(TEXT("Mass has no invented Explosion Context rule"), Settings->FindContextReduction(Tag(TEXT("Cheat.Mass")), Tag(TEXT("Context.Explosion")), ContextReduction));

	ARealityEditableTestActor* Integrated = SpawnMassActor(World, FVector(500.0f, 0.0f, 0.0f));
	TestTrue(TEXT("Integrated no-witness Mass Apply succeeds"), Integrated->EditableComponent->ApplyMassModification(ERealityMassPreset::Half, Instigator));
	TestEqual(TEXT("No-witness Mass Apply adds 10"), Manager->GetSuspicion(), 10.0f);
	TestTrue(TEXT("Integrated Mass Restore succeeds"), Integrated->EditableComponent->RestoreMassModification(Instigator));
	TestEqual(TEXT("Mass Restore remains exactly -5"), Manager->GetSuspicion(), 5.0f);

	ARealityWitnessTestActor* Witness = World->SpawnActor<ARealityWitnessTestActor>(FVector::ZeroVector, FRotator::ZeroRotator);
	Witness->WitnessComponent->WitnessType = Tag(TEXT("Witness.Human"));
	Witness->WitnessComponent->ObservationRange = 1500.0f;
	Witness->WitnessComponent->FieldOfViewDegrees = 90.0f;
	Witness->WitnessComponent->bRequireLineOfSight = false;
	Witness->WitnessComponent->OnRealityEventObserved.AddDynamic(Witness, &ARealityWitnessTestActor::HandleRealityEventObserved);
	if (!Witness->HasActorBegunPlay()) Witness->DispatchBeginPlay();
	Manager->ResetRealityState();
	TestTrue(TEXT("Witnessed Mass Apply succeeds"), Integrated->EditableComponent->ApplyMassModification(ERealityMassPreset::Double, Instigator));
	TestEqual(TEXT("Human Witness adds existing +10 to Mass base +10"), Manager->GetSuspicion(), 20.0f);
	const FRealityProcessedCheatRecord* Record = Manager->GetMostRecentEvent();
	if (TestNotNull(TEXT("Witnessed Mass event is recorded"), Record))
	{
		TestEqual(TEXT("Mass record base is 10"), Record->BaseSuspicionDelta, 10.0f);
		TestEqual(TEXT("Mass record Witness bonus is 10"), Record->WitnessSuspicionDelta, 10.0f);
		TestEqual(TEXT("Mass record Context reduction is zero"), Record->ContextSuspicionReduction, 0.0f);
	}
	Integrated->EditableComponent->RestoreMassModification(Instigator);

	AActor* Player = World->SpawnActor<AActor>(FVector::ZeroVector, FRotator::ZeroRotator);
	UDeveloperModeComponent* Developer = NewObject<UDeveloperModeComponent>(Player);
	Player->AddInstanceComponent(Developer);
	Developer->RegisterComponent();
	UDeveloperConsoleWidget* Widget = NewObject<UDeveloperConsoleWidget>(World);
	Widget->SetDeveloperModeComponent(Developer);
	Developer->EnterDeveloperMode();
	TestTrue(TEXT("Console exposes Mass capability"), Widget->IsCheatSectionAvailable(Tag(TEXT("Cheat.Mass"))));
	URealityEditableComponent* ConsoleTarget = Developer->GetFocusedEditableComponent();
	TestNotNull(TEXT("Console has a frozen Mass target"), ConsoleTarget);
	TestTrue(TEXT("Console Mass preset invokes typed gameplay path"), Widget->ExecuteMassPreset(ERealityMassPreset::Quarter));
	TestTrue(TEXT("Console refresh source reports active Mass"), ConsoleTarget && ConsoleTarget->IsMassModified());
	TestTrue(TEXT("Console Mass Restore invokes typed gameplay path"), Widget->ExecuteMassRestore());
	TestFalse(TEXT("Console Mass Restore ends cycle"), ConsoleTarget && ConsoleTarget->IsMassModified());
	Developer->ExitDeveloperMode();

	World->DestroyWorld(false);
	World->SetPhysicsScene(nullptr);
	GEngine->DestroyWorldContext(World);
	World->RemoveFromRoot();
	return true;
}

#endif
