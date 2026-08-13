// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"

#include "AI/RealityWitnessComponent.h"
#include "AI/RealityWitnessTestActor.h"
#include "Components/BoxComponent.h"
#include "Developer/DeveloperModeComponent.h"
#include "Developer/RealityEditableComponent.h"
#include "Developer/RealityEditableTestActor.h"
#include "Developer/RealityTimeTestActor.h"
#include "Engine/CollisionProfile.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/WorldSettings.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "RealitySystem/RealityManagerSubsystem.h"
#include "UI/DeveloperConsoleWidget.h"

namespace RealityTimeTests
{
	FGameplayTag Tag(const TCHAR* Name) { return FGameplayTag::RequestGameplayTag(Name); }

	void SetCapabilities(URealityEditableComponent* Editable, const std::initializer_list<const TCHAR*> Names)
	{
		FGameplayTagContainer Tags;
		for (const TCHAR* Name : Names) Tags.AddTag(Tag(Name));
		Editable->SetSupportedCheats(Tags);
	}

	ARealityEditableTestActor* SpawnEditable(UWorld* World, const FVector& Location, const std::initializer_list<const TCHAR*> Capabilities)
	{
		ARealityEditableTestActor* Actor = World->SpawnActor<ARealityEditableTestActor>(Location, FRotator::ZeroRotator);
		SetCapabilities(Actor->EditableComponent, Capabilities);
		Actor->EditableComponent->OnRealityCheatEvent.AddDynamic(Actor, &ARealityEditableTestActor::HandleRealityCheatEvent);
		return Actor;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRealityTimeModificationTest,
	"Reality.Developer.TimeModification",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::EngineFilter)

bool FRealityTimeModificationTest::RunTest(const FString& Parameters)
{
	using namespace RealityTimeTests;
	const FName WorldName = MakeUniqueObjectName(nullptr, UWorld::StaticClass(), NAME_None, EUniqueObjectNameOptions::GloballyUnique);
	FWorldContext& WorldContext = GEngine->CreateNewWorldContext(EWorldType::Game);
	UWorld* World = UWorld::CreateWorld(EWorldType::Game, false, WorldName, GetTransientPackage());
	if (!TestNotNull(TEXT("A Time test world can be created"), World)) return false;
	World->AddToRoot();
	WorldContext.SetCurrentWorld(World);
	World->InitializeActorsForPlay(FURL());
	World->BeginPlay();
	AActor* Instigator = World->SpawnActor<AActor>();

	ARealityEditableTestActor* Unsupported = SpawnEditable(World, FVector::ZeroVector, {});
	TestFalse(TEXT("Unsupported Time Apply fails"), Unsupported->EditableComponent->ApplyTimeModification(ERealityTimePreset::Half, Instigator));
	TestEqual(TEXT("Unsupported Time emits no event"), Unsupported->CheatEventCount, 0);

	ARealityEditableTestActor* Actor = SpawnEditable(World, FVector(300.0f, 0.0f, 0.0f), { TEXT("Cheat.Time") });
	Actor->CustomTimeDilation = 1.6f;
	const float GlobalDilation = World->GetWorldSettings()->TimeDilation;
	TestFalse(TEXT("1.0x is a no-op before a Time cycle"), Actor->EditableComponent->ApplyTimeModification(ERealityTimePreset::One, Instigator));
	TestTrue(TEXT("0.5x Time Apply succeeds"), Actor->EditableComponent->ApplyTimeModification(ERealityTimePreset::Half, Instigator));
	TestEqual(TEXT("Non-default baseline is captured"), Actor->EditableComponent->GetOriginalTimeDilation(), 1.6f);
	TestEqual(TEXT("0.5x is baseline-relative"), Actor->CustomTimeDilation, 0.8f);
	TestEqual(TEXT("Time event tag is correct"), Actor->LastCheatEvent.CheatTag, Tag(TEXT("Cheat.Time")));
	TestEqual(TEXT("Time Apply event operation is correct"), Actor->LastCheatEvent.Operation, ERealityCheatOperation::Apply);
	TestEqual(TEXT("Time event target is correct"), Actor->LastCheatEvent.TargetActor.Get(), static_cast<AActor*>(Actor));
	TestEqual(TEXT("Time event instigator is correct"), Actor->LastCheatEvent.InstigatingActor.Get(), Instigator);
	const int32 HalfEvents = Actor->CheatEventCount;
	TestFalse(TEXT("Same Time preset is rejected"), Actor->EditableComponent->ApplyTimeModification(ERealityTimePreset::Half, Instigator));
	TestEqual(TEXT("Repeated preset emits no event"), Actor->CheatEventCount, HalfEvents);
	TestTrue(TEXT("4.0x preset switch succeeds"), Actor->EditableComponent->ApplyTimeModification(ERealityTimePreset::Quadruple, Instigator));
	TestEqual(TEXT("Preset switching does not compound"), Actor->CustomTimeDilation, 6.4f);
	TestTrue(TEXT("1.0x returns to baseline inside active cycle"), Actor->EditableComponent->ApplyTimeModification(ERealityTimePreset::One, Instigator));
	TestTrue(TEXT("1.0x keeps the Time cycle active"), Actor->EditableComponent->IsTimeModified());
	TestEqual(TEXT("1.0x restores physical baseline"), Actor->CustomTimeDilation, 1.6f);
	TestFalse(TEXT("Invalid preset fails safely"), Actor->EditableComponent->ApplyTimeModification(static_cast<ERealityTimePreset>(255), Instigator));
	TestTrue(TEXT("Explicit Time Restore succeeds"), Actor->EditableComponent->RestoreTimeModification(Instigator));
	TestEqual(TEXT("Restore returns exact original dilation"), Actor->CustomTimeDilation, 1.6f);
	TestEqual(TEXT("Restore event operation is correct"), Actor->LastCheatEvent.Operation, ERealityCheatOperation::Restore);
	TestFalse(TEXT("Repeated Time Restore is a no-op"), Actor->EditableComponent->RestoreTimeModification(Instigator));
	TestEqual(TEXT("Local Time never changes global dilation"), World->GetWorldSettings()->TimeDilation, GlobalDilation);

	Actor->CustomTimeDilation = 0.75f;
	TestTrue(TEXT("A later cycle captures a new legitimate baseline"), Actor->EditableComponent->ApplyTimeModification(ERealityTimePreset::Double, Instigator));
	TestEqual(TEXT("Later cycle uses new baseline"), Actor->CustomTimeDilation, 1.5f);
	Actor->CustomTimeDilation = 3.0f;
	TestTrue(TEXT("Active cycle retains captured baseline despite an external change"), Actor->EditableComponent->ApplyTimeModification(ERealityTimePreset::Quarter, Instigator));
	TestEqual(TEXT("Active preset reasserts from captured baseline"), Actor->CustomTimeDilation, 0.1875f);
	TestTrue(TEXT("Later cycle restores captured value"), Actor->EditableComponent->RestoreTimeModification(Instigator));
	TestEqual(TEXT("External active-cycle write does not replace baseline"), Actor->CustomTimeDilation, 0.75f);
	Actor->CustomTimeDilation = -1.0f;
	TestFalse(TEXT("Invalid negative baseline fails safely"), Actor->EditableComponent->ApplyTimeModification(ERealityTimePreset::Half, Instigator));
	Actor->CustomTimeDilation = NAN;
	TestFalse(TEXT("Invalid non-finite baseline fails safely"), Actor->EditableComponent->ApplyTimeModification(ERealityTimePreset::Half, Instigator));
	Actor->CustomTimeDilation = 1.0f;

	ARealityEditableTestActor* Other = SpawnEditable(World, FVector(600.0f, 0.0f, 0.0f), { TEXT("Cheat.Time") });
	TestTrue(TEXT("Two Actors can hold independent Time presets A"), Actor->EditableComponent->ApplyTimeModification(ERealityTimePreset::Quarter, Instigator));
	TestTrue(TEXT("Two Actors can hold independent Time presets B"), Other->EditableComponent->ApplyTimeModification(ERealityTimePreset::Double, Instigator));
	TestEqual(TEXT("Actor A local Time remains independent"), Actor->CustomTimeDilation, 0.25f);
	TestEqual(TEXT("Actor B local Time remains independent"), Other->CustomTimeDilation, 2.0f);
	Actor->EditableComponent->RestoreTimeModification(Instigator);
	Other->EditableComponent->RestoreTimeModification(Instigator);

	ARealityTimeTestActor* Rotor = World->SpawnActor<ARealityTimeTestActor>();
	SetCapabilities(Rotor->EditableComponent, { TEXT("Cheat.Time") });
	TestTrue(TEXT("Tick-driven rotor accepts Time"), Rotor->EditableComponent->ApplyTimeModification(ERealityTimePreset::Half, Instigator));
	// Transient automation worlds do not register spawned Actor ticks consistently, so target the engine tick function explicitly.
	Rotor->PrimaryActorTick.Target = Rotor;
	Rotor->PrimaryActorTick.ExecuteTick(2.0f, LEVELTICK_All, ENamedThreads::GameThread, FGraphEventRef());
	TestEqual(TEXT("Actor tick receives CustomTimeDilation-scaled delta"), Rotor->LastReceivedDeltaSeconds, 1.0f);
	TestEqual(TEXT("Tick-driven rotation uses scaled delta"), Rotor->GetActorRotation().Yaw, 90.0);

	ARealityEditableTestActor* AllCheats = SpawnEditable(World, FVector(900.0f, 0.0f, 0.0f), {
		TEXT("Cheat.Collision"), TEXT("Cheat.Scale"), TEXT("Cheat.Gravity"), TEXT("Cheat.Mass"), TEXT("Cheat.Friction"), TEXT("Cheat.Time") });
	AllCheats->PrimitiveA->SetCollisionProfileName(UCollisionProfile::PhysicsActor_ProfileName);
	AllCheats->PrimitiveA->SetSimulatePhysics(true);
	TestTrue(TEXT("Scale coexists with Time"), AllCheats->EditableComponent->ApplyScaleModification(ERealityScalePreset::Double, Instigator));
	TestTrue(TEXT("Gravity coexists with Time"), AllCheats->EditableComponent->ApplyGravityModification(ERealityGravityPreset::Low, Instigator));
	TestTrue(TEXT("Mass coexists with Time"), AllCheats->EditableComponent->ApplyMassModification(ERealityMassPreset::Half, Instigator));
	TestTrue(TEXT("Friction coexists with Time"), AllCheats->EditableComponent->ApplyFrictionModification(ERealityFrictionPreset::Low, Instigator));
	TestTrue(TEXT("Time coexists with every prior property"), AllCheats->EditableComponent->ApplyTimeModification(ERealityTimePreset::Double, Instigator));
	TestTrue(TEXT("Collision coexists with active Time"), AllCheats->EditableComponent->ApplyCollisionModification(Instigator));
	TestTrue(TEXT("Collision restores while Time remains active"), AllCheats->EditableComponent->RestoreCollisionModification(Instigator));
	TestTrue(TEXT("Time survives Collision Restore"), AllCheats->EditableComponent->IsTimeModified());
	TestTrue(TEXT("Collision can reactivate before Time Restore"), AllCheats->EditableComponent->ApplyCollisionModification(Instigator));
	TestTrue(TEXT("Time restores independently"), AllCheats->EditableComponent->RestoreTimeModification(Instigator));
	TestTrue(TEXT("Collision remains active after Time Restore"), AllCheats->EditableComponent->IsCollisionModified());
	TestTrue(TEXT("Scale remains active after Time Restore"), AllCheats->EditableComponent->IsScaleModified());
	TestTrue(TEXT("Gravity remains active after Time Restore"), AllCheats->EditableComponent->IsGravityModified());
	TestTrue(TEXT("Mass remains active after Time Restore"), AllCheats->EditableComponent->IsMassModified());
	TestTrue(TEXT("Friction remains active after Time Restore"), AllCheats->EditableComponent->IsFrictionModified());
	TestTrue(TEXT("Friction restores independently after Time"), AllCheats->EditableComponent->RestoreFrictionModification(Instigator));
	TestTrue(TEXT("Mass restores independently after Time"), AllCheats->EditableComponent->RestoreMassModification(Instigator));
	TestTrue(TEXT("Collision restores independently after Time"), AllCheats->EditableComponent->RestoreCollisionModification(Instigator));
	TestTrue(TEXT("Gravity restores independently after Time"), AllCheats->EditableComponent->RestoreGravityModification(Instigator));
	TestTrue(TEXT("Scale restores independently after Time"), AllCheats->EditableComponent->RestoreScaleModification(Instigator));

	URealityManagerSubsystem* Manager = World->GetSubsystem<URealityManagerSubsystem>();
	Manager->ResetRealityState();
	TestTrue(TEXT("Time Apply reaches Reality Manager"), Actor->EditableComponent->ApplyTimeModification(ERealityTimePreset::Half, Instigator));
	TestEqual(TEXT("Unwitnessed Time Apply adds +20"), Manager->GetSuspicion(), 20.0f);
	const FRealityProcessedCheatRecord* Record = Manager->GetMostRecentEvent();
	if (TestNotNull(TEXT("Time history record exists"), Record))
	{
		TestEqual(TEXT("Time record Base is +20"), Record->BaseSuspicionDelta, 20.0f);
		TestEqual(TEXT("Time invents no Context reduction"), Record->ContextSuspicionReduction, 0.0f);
	}
	TestTrue(TEXT("Time Restore reaches Reality Manager"), Actor->EditableComponent->RestoreTimeModification(Instigator));
	TestEqual(TEXT("Time Restore is exactly -5"), Manager->GetSuspicion(), 15.0f);

	Manager->ResetRealityState();
	ARealityWitnessTestActor* Witness = World->SpawnActor<ARealityWitnessTestActor>(FVector::ZeroVector, FRotator::ZeroRotator);
	Witness->WitnessComponent->WitnessType = Tag(TEXT("Witness.Human"));
	Witness->WitnessComponent->ObservationRange = 1500.0f;
	Witness->WitnessComponent->FieldOfViewDegrees = 90.0f;
	Witness->WitnessComponent->bRequireLineOfSight = false;
	if (!Witness->HasActorBegunPlay()) Witness->DispatchBeginPlay();
	TestTrue(TEXT("Witnessed Time Apply succeeds"), Actor->EditableComponent->ApplyTimeModification(ERealityTimePreset::Double, Instigator));
	TestEqual(TEXT("Human Witness adds existing +10 to Time"), Manager->GetSuspicion(), 30.0f);

	World->DestroyWorld(false);
	GEngine->DestroyWorldContext(World);
	World->RemoveFromRoot();
	return true;
}

#endif
