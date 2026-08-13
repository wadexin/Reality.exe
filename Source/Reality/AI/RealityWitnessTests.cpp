// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"

#include "AI/RealityWitnessComponent.h"
#include "AI/RealityWitnessTestActor.h"
#include "Components/BoxComponent.h"
#include "Developer/RealityEditableTestActor.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "RealitySystem/RealityManagerSubsystem.h"
#include "RealitySystem/RealitySuspicionSettings.h"

namespace RealityWitnessTests
{
	FGameplayTag Tag(const TCHAR* Name)
	{
		return FGameplayTag::RequestGameplayTag(Name);
	}

	FRealityCheatEvent CollisionEvent(AActor* Target, const ERealityCheatOperation Operation = ERealityCheatOperation::Apply)
	{
		return FRealityCheatEvent(Target, Tag(TEXT("Cheat.Collision")), nullptr, Operation);
	}

	ARealityWitnessTestActor* SpawnWitness(
		UWorld* World,
		const FVector& Location,
		const FRotator& Rotation,
		const TCHAR* Type,
		const float Range = 1500.0f,
		const float Fov = 90.0f,
		const bool bLos = true)
	{
		ARealityWitnessTestActor* Witness = World->SpawnActor<ARealityWitnessTestActor>();
		Witness->SetActorLocationAndRotation(Location, Rotation);
		Witness->WitnessComponent->WitnessType = Tag(Type);
		Witness->WitnessComponent->ObservationRange = Range;
		Witness->WitnessComponent->FieldOfViewDegrees = Fov;
		Witness->WitnessComponent->bRequireLineOfSight = bLos;
		Witness->WitnessComponent->OnRealityEventObserved.AddDynamic(Witness, &ARealityWitnessTestActor::HandleRealityEventObserved);
		if (!Witness->HasActorBegunPlay())
		{
			Witness->DispatchBeginPlay();
		}
		return Witness;
	}

	AActor* SpawnOccluder(UWorld* World, const FVector& Location)
	{
		AActor* Occluder = World->SpawnActor<AActor>(Location, FRotator::ZeroRotator);
		UBoxComponent* Box = NewObject<UBoxComponent>(Occluder, TEXT("Visibility Occluder"));
		Occluder->SetRootComponent(Box);
		Occluder->AddInstanceComponent(Box);
		Box->SetBoxExtent(FVector(20.0f, 200.0f, 200.0f));
		Box->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		Box->SetCollisionResponseToAllChannels(ECR_Ignore);
		Box->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
		Box->RegisterComponent();
		return Occluder;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRealityWitnessSystemTest,
	"Reality.AI.Witness",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::EngineFilter)

bool FRealityWitnessSystemTest::RunTest(const FString& Parameters)
{
	using namespace RealityWitnessTests;

	const FName WorldName = MakeUniqueObjectName(nullptr, UWorld::StaticClass(), NAME_None, EUniqueObjectNameOptions::GloballyUnique);
	FWorldContext& WorldContext = GEngine->CreateNewWorldContext(EWorldType::Game);
	UWorld* TestWorld = UWorld::CreateWorld(EWorldType::Game, false, WorldName, GetTransientPackage());
	if (!TestNotNull(TEXT("A Witness test world can be created"), TestWorld))
	{
		GEngine->DestroyWorldContext(TestWorld);
		return false;
	}

	TestWorld->AddToRoot();
	WorldContext.SetCurrentWorld(TestWorld);
	TestWorld->InitializeActorsForPlay(FURL());
	TestWorld->BeginPlay();

	URealityManagerSubsystem* Manager = TestWorld->GetSubsystem<URealityManagerSubsystem>();
	const URealitySuspicionSettings* Settings = GetDefault<URealitySuspicionSettings>();
	TestNotNull(TEXT("Witnesses share the world Reality Manager"), Manager);
	TestNotNull(TEXT("Witness tuning settings are available"), Settings);
	float HumanBonus = 0.0f;
	float CameraBonus = 0.0f;
	TestTrue(TEXT("Human Witness has configured evidence"), Settings->FindWitnessSuspicion(Tag(TEXT("Witness.Human")), HumanBonus));
	TestTrue(TEXT("Camera Witness has configured evidence"), Settings->FindWitnessSuspicion(Tag(TEXT("Witness.Camera")), CameraBonus));
	TestEqual(TEXT("Human bonus defaults to 10"), HumanBonus, 10.0f);
	TestEqual(TEXT("Camera bonus defaults to 15"), CameraBonus, 15.0f);
	TestEqual(TEXT("Per-event Witness cap defaults to 30"), Settings->MaximumWitnessBonusPerEvent, 30.0f);

	ARealityEditableTestActor* Target = TestWorld->SpawnActor<ARealityEditableTestActor>();
	Target->SetActorLocation(FVector(500.0f, 0.0f, 0.0f));
	Target->PrimitiveA->SetBoxExtent(FVector(50.0f));
	Target->PrimitiveA->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Target->PrimitiveB->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Target->PrimitiveA->UpdateBounds();
	Target->PrimitiveB->UpdateBounds();
	Target->UpdateComponentTransforms();

	ARealityWitnessTestActor* Human = SpawnWitness(TestWorld, FVector::ZeroVector, FRotator::ZeroRotator, TEXT("Witness.Human"));
	TestEqual(TEXT("An active Witness self-registers"), Manager->GetRegisteredWitnessCount(), 1);
	TestFalse(TEXT("Witness component never ticks"), Human->WitnessComponent->PrimaryComponentTick.bCanEverTick);
	FVector TestObservationOrigin;
	FVector TestObservationForward;
	TestTrue(TEXT("Witness supplies an observation viewpoint"), Human->WitnessComponent->GetObservationViewpoint(TestObservationOrigin, TestObservationForward));
	TestEqual(TEXT("Test Witness faces positive X"), TestObservationForward, FVector::ForwardVector);
	TestEqual(TEXT("Test target point uses Actor bounds center"), Human->WitnessComponent->GetObservationTargetPoint(Target), FVector(500.0f, 0.0f, 0.0f));
	TestTrue(TEXT("Direct clear observation succeeds"), Human->WitnessComponent->CanObserveRealityEvent(CollisionEvent(Target)));

	Manager->ResetRealityState();
	TestTrue(TEXT("Clear in-range Human observes collision-off target"), Manager->ProcessCheatEvent(CollisionEvent(Target)));
	TestEqual(TEXT("Collision plus Human equals 35"), Manager->GetSuspicion(), 35.0f);
	TestEqual(TEXT("Combined delta drives Questioning state"), Manager->GetRealityState(), ERealityState::Questioning);
	TestEqual(TEXT("Observed delegate fires once"), Human->ObservedEventCount, 1);
	TestEqual(TEXT("Observed payload preserves target"), Human->LastObservedEvent.TargetActor.Get(), static_cast<AActor*>(Target));
	const FRealityProcessedCheatRecord* HumanRecord = Manager->GetMostRecentEvent();
	if (TestNotNull(TEXT("Human observation creates history"), HumanRecord))
	{
		TestEqual(TEXT("History records base delta"), HumanRecord->BaseSuspicionDelta, 25.0f);
		TestEqual(TEXT("History records Human delta"), HumanRecord->WitnessSuspicionDelta, 10.0f);
		TestEqual(TEXT("History records one observer"), HumanRecord->ObservingWitnessCount, 1);
		TestEqual(TEXT("History records final applied delta"), HumanRecord->SuspicionDelta, 35.0f);
	}

	Human->WitnessComponent->bWitnessEnabled = false;
	Manager->ResetRealityState();
	TestTrue(TEXT("Disabled Witness leaves event valid"), Manager->ProcessCheatEvent(CollisionEvent(Target)));
	TestEqual(TEXT("Disabled Witness contributes nothing"), Manager->GetSuspicion(), 25.0f);
	TestEqual(TEXT("Disabled Witness emits no notification"), Human->ObservedEventCount, 1);

	Human->WitnessComponent->bWitnessEnabled = true;
	Human->WitnessComponent->ObservationRange = 100.0f;
	Manager->ResetRealityState();
	Manager->ProcessCheatEvent(CollisionEvent(Target));
	TestEqual(TEXT("Outside range gives base only"), Manager->GetSuspicion(), 25.0f);

	Human->WitnessComponent->ObservationRange = 1500.0f;
	Human->SetActorRotation(FRotator(0.0f, 180.0f, 0.0f));
	Manager->ResetRealityState();
	Manager->ProcessCheatEvent(CollisionEvent(Target));
	TestEqual(TEXT("Target behind FOV gives base only"), Manager->GetSuspicion(), 25.0f);

	Human->SetActorRotation(FRotator::ZeroRotator);
	AActor* Occluder = SpawnOccluder(TestWorld, FVector(250.0f, 0.0f, 0.0f));
	Manager->ResetRealityState();
	Manager->ProcessCheatEvent(CollisionEvent(Target));
	TestEqual(TEXT("Visibility blocker prevents observation"), Manager->GetSuspicion(), 25.0f);
	Occluder->Destroy();

	Human->WitnessComponent->bWitnessEnabled = false;
	ARealityWitnessTestActor* Camera = SpawnWitness(TestWorld, FVector(0.0f, 300.0f, 0.0f), FRotator(0.0f, -30.9638f, 0.0f), TEXT("Witness.Camera"));
	Manager->ResetRealityState();
	Manager->ProcessCheatEvent(CollisionEvent(Target));
	TestEqual(TEXT("Camera adds configured bonus"), Manager->GetSuspicion(), 40.0f);
	TestEqual(TEXT("Camera receives one notification"), Camera->ObservedEventCount, 1);

	Human->WitnessComponent->bWitnessEnabled = true;
	ARealityWitnessTestActor* HumanB = SpawnWitness(TestWorld, FVector(0.0f, -200.0f, 0.0f), FRotator(0.0f, 21.8014f, 0.0f), TEXT("Witness.Human"));
	Manager->ResetRealityState();
	Manager->ProcessCheatEvent(CollisionEvent(Target));
	TestEqual(TEXT("Human, Human, and Camera aggregate to capped Witness bonus"), Manager->GetSuspicion(), 55.0f);
	const FRealityProcessedCheatRecord* CappedRecord = Manager->GetMostRecentEvent();
	if (TestNotNull(TEXT("Capped event has history"), CappedRecord))
	{
		TestEqual(TEXT("Raw 35 Witness evidence caps at 30"), CappedRecord->WitnessSuspicionDelta, 30.0f);
		TestEqual(TEXT("All three successful observers are reported"), CappedRecord->ObservingWitnessCount, 3);
	}

	Human->WitnessComponent->bWitnessEnabled = true;
	Human->SetActorRotation(FRotator::ZeroRotator);
	HumanB->WitnessComponent->bWitnessEnabled = true;
	HumanB->SetActorRotation(FRotator(0.0f, 180.0f, 0.0f));
	Camera->WitnessComponent->bWitnessEnabled = true;
	Camera->WitnessComponent->ObservationRange = 100.0f;
	Manager->ResetRealityState();
	Manager->ProcessCheatEvent(CollisionEvent(Target));
	TestEqual(TEXT("Mixed observability counts only the visible in-range Human"), Manager->GetSuspicion(), 35.0f);
	const FRealityProcessedCheatRecord* MixedRecord = Manager->GetMostRecentEvent();
	if (TestNotNull(TEXT("Mixed observability creates history"), MixedRecord))
	{
		TestEqual(TEXT("Mixed event has one observer"), MixedRecord->ObservingWitnessCount, 1);
		TestEqual(TEXT("Mixed event applies only Human evidence"), MixedRecord->WitnessSuspicionDelta, 10.0f);
	}
	Camera->WitnessComponent->ObservationRange = 1500.0f;

	ARealityWitnessTestActor* Unknown = SpawnWitness(TestWorld, FVector(0.0f, 100.0f, 0.0f), FRotator(0.0f, -11.3099f, 0.0f), TEXT("Witness.Inspector"));
	Human->WitnessComponent->bWitnessEnabled = false;
	HumanB->WitnessComponent->bWitnessEnabled = false;
	Camera->WitnessComponent->bWitnessEnabled = false;
	Manager->ResetRealityState();
	Manager->ProcessCheatEvent(CollisionEvent(Target));
	TestEqual(TEXT("Unconfigured Witness type invents no bonus"), Manager->GetSuspicion(), 25.0f);
	TestEqual(TEXT("Unconfigured Witness receives no notification"), Unknown->ObservedEventCount, 0);

	Unknown->WitnessComponent->bWitnessEnabled = false;
	Human->WitnessComponent->bWitnessEnabled = true;
	Manager->ResetRealityState();
	Manager->ProcessCheatEvent(CollisionEvent(Target));
	const int32 ObservationsBeforeRestore = Human->ObservedEventCount;
	Manager->ProcessCheatEvent(CollisionEvent(Target, ERealityCheatOperation::Restore));
	TestEqual(TEXT("Restore remains exactly minus five"), Manager->GetSuspicion(), 30.0f);
	TestEqual(TEXT("Restore performs no Witness observation"), Human->ObservedEventCount, ObservationsBeforeRestore);
	const FRealityProcessedCheatRecord* RestoreRecord = Manager->GetMostRecentEvent();
	if (TestNotNull(TEXT("Restore has history"), RestoreRecord))
	{
		TestEqual(TEXT("Restore history has no Witness bonus"), RestoreRecord->WitnessSuspicionDelta, 0.0f);
		TestEqual(TEXT("Restore history has no observers"), RestoreRecord->ObservingWitnessCount, 0);
	}

	const int32 BeforeDestroyCount = Manager->GetRegisteredWitnessCount();
	HumanB->Destroy();
	TestEqual(TEXT("Destroyed Witness unregisters safely"), Manager->GetRegisteredWitnessCount(), BeforeDestroyCount - 1);
	Manager->UnregisterWitness(HumanB->WitnessComponent);
	TestEqual(TEXT("Duplicate unregister is safe"), Manager->GetRegisteredWitnessCount(), BeforeDestroyCount - 1);

	TestWorld->DestroyWorld(false);
	TestWorld->SetPhysicsScene(nullptr);
	GEngine->DestroyWorldContext(TestWorld);
	TestWorld->RemoveFromRoot();
	return true;
}

#endif
