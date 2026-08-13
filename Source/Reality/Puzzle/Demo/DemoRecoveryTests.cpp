// Copyright Epic Games, Inc. All Rights Reserved.
#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"

#include "Developer/RealityEditableComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Puzzle/Demo/DemoCheckpointVolume.h"
#include "Puzzle/Demo/DemoEditableActor.h"
#include "Puzzle/Demo/DemoPlayerRecoveryComponent.h"
#include "Puzzle/Demo/DemoRecoverableComponent.h"
#include "Puzzle/Demo/DemoRecoveryVolume.h"
#include "Puzzle/Demo/DemoSensorFailurePanel.h"
#include "Puzzle/Demo/DemoTimeMachineryActor.h"
#include "Puzzle/Demo/DemoTimeReferenceActor.h"
#include "RealityPlayerController.h"
#include "RealitySystem/RealityManagerSubsystem.h"
#include "RealitySystem/RealityContextComponent.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDemoRecoveryContractsTest, "Reality.Puzzle.Demo.RecoveryContracts", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FDemoRecoveryContractsTest::RunTest(const FString& Parameters)
{
	const FName WorldName = MakeUniqueObjectName(nullptr, UWorld::StaticClass(), NAME_None, EUniqueObjectNameOptions::GloballyUnique);
	FWorldContext& Context = GEngine->CreateNewWorldContext(EWorldType::Game);
	UWorld* World = UWorld::CreateWorld(EWorldType::Game, false, WorldName, GetTransientPackage());
	if (!TestNotNull(TEXT("A recovery test world can be created"), World)) return false;
	World->AddToRoot();
	Context.SetCurrentWorld(World);
	World->InitializeActorsForPlay(FURL());
	World->BeginPlay();

	TestEqual(TEXT("Restart uses UE current-level restart travel semantics"), GetDefault<ARealityPlayerController>()->GetRestartTravelURL(), FString(TEXT("?restart")));

	AActor* Player = World->SpawnActor<AActor>(FVector(10, 20, 30), FRotator::ZeroRotator);
	USceneComponent* PlayerRoot = NewObject<USceneComponent>(Player); Player->SetRootComponent(PlayerRoot); PlayerRoot->RegisterComponent();
	Player->SetActorLocation(FVector(10, 20, 30));
	UDemoPlayerRecoveryComponent* PlayerRecovery = NewObject<UDemoPlayerRecoveryComponent>(Player);
	Player->AddInstanceComponent(PlayerRecovery); PlayerRecovery->RegisterComponent();
	if (!Player->HasActorBegunPlay()) Player->DispatchBeginPlay();
	const FTransform CheckpointOne(FRotator(0, 20, 0), FVector(500, 0, 100));
	const FTransform CheckpointTwo(FRotator(0, 30, 0), FVector(900, 0, 120));
	TestTrue(TEXT("First authored checkpoint registers"), PlayerRecovery->RegisterCheckpoint(1, CheckpointOne));
	TestFalse(TEXT("Duplicate checkpoint registration is ignored"), PlayerRecovery->RegisterCheckpoint(1, CheckpointOne));
	TestTrue(TEXT("Later checkpoint replaces safe position"), PlayerRecovery->RegisterCheckpoint(2, CheckpointTwo));
	Player->SetActorLocation(FVector(-1000));
	TestTrue(TEXT("Player recovery succeeds"), PlayerRecovery->RecoverPlayer());
	TestEqual(TEXT("Player uses latest valid recovery point"), Player->GetActorLocation(), CheckpointTwo.GetLocation());

	ADemoEditableActor* CriticalProp = World->SpawnActor<ADemoEditableActor>(FVector(200, 0, 100), FRotator::ZeroRotator);
	CriticalProp->ConfigureDemoTarget(FText::FromString(TEXT("Critical")), false, true, true, false, false, false, true, false, true);
	if (!CriticalProp->HasActorBegunPlay()) CriticalProp->DispatchBeginPlay();
	const FVector BaselineLocation = CriticalProp->GetActorLocation();
	URealityManagerSubsystem* Manager = World->GetSubsystem<URealityManagerSubsystem>();
	TestTrue(TEXT("Critical prop accepts Scale before invalid recovery"), CriticalProp->EditableComponent->ApplyScaleModification(ERealityScalePreset::Double, Player));
	const float SuspicionBeforeRecovery = Manager->GetSuspicion();
	CriticalProp->SetActorLocation(FVector(3000, 3000, -1000));
	ADemoRecoveryVolume* RecoveryVolume = World->SpawnActor<ADemoRecoveryVolume>();
	TestTrue(TEXT("Authored invalid-state volume recovers critical prop"), RecoveryVolume->RecoverActor(CriticalProp));
	TestEqual(TEXT("Critical prop returns to authored location"), CriticalProp->GetActorLocation(), BaselineLocation);
	TestEqual(TEXT("Critical prop recovery restores baseline Scale"), CriticalProp->GetActorScale3D(), FVector::OneVector);
	TestFalse(TEXT("Critical prop recovery ends active Reality cycle"), CriticalProp->EditableComponent->IsScaleModified());
	TestEqual(TEXT("Invalid-state recovery preserves session Suspicion"), Manager->GetSuspicion(), SuspicionBeforeRecovery);
	TestTrue(TEXT("Repeated recovery is safe"), RecoveryVolume->RecoverActor(CriticalProp));

	ADemoEditableActor* UnrelatedProp = World->SpawnActor<ADemoEditableActor>(FVector(400, 0, 100), FRotator::ZeroRotator);
	UnrelatedProp->ConfigureDemoTarget(FText::FromString(TEXT("Static")), false, true, false, false, false, false, false, false, false);
	if (!UnrelatedProp->HasActorBegunPlay()) UnrelatedProp->DispatchBeginPlay();
	const FVector UnrelatedDisplaced(700, 700, 100);
	UnrelatedProp->SetActorLocation(UnrelatedDisplaced);
	TestFalse(TEXT("Unrelated non-recoverable prop is not reset"), RecoveryVolume->RecoverActor(UnrelatedProp));
	TestEqual(TEXT("Unrelated prop retains emergent position"), UnrelatedProp->GetActorLocation(), UnrelatedDisplaced);

	ADemoSensorFailurePanel* Sensor = World->SpawnActor<ADemoSensorFailurePanel>();
	Sensor->ConfigureSensorFailure(1500.0f, true);
	ADemoCheckpointVolume* Checkpoint = World->SpawnActor<ADemoCheckpointVolume>(); Checkpoint->CheckpointIndex = 3;
	const float SuspicionBeforeCheckpoint = Manager->GetSuspicion();
	TestTrue(TEXT("Checkpoint activates for player recovery component"), Checkpoint->ActivateCheckpoint(Player));
	TestTrue(TEXT("Checkpoint preserves active SensorFailure state"), Sensor->ContextComponent->IsContextActive());
	TestEqual(TEXT("Checkpoint preserves current Suspicion"), Manager->GetSuspicion(), SuspicionBeforeCheckpoint);

	ADemoTimeReferenceActor* ReferenceRotor = World->SpawnActor<ADemoTimeReferenceActor>();
	ADemoTimeMachineryActor* SecurityRotor = World->SpawnActor<ADemoTimeMachineryActor>();
	TestNull(TEXT("Reference Rotor remains non-editable"), ReferenceRotor->FindComponentByClass<URealityEditableComponent>());
	TestTrue(TEXT("Security Rotor remains Time-editable"), SecurityRotor->EditableComponent->SupportsCheat(FGameplayTag::RequestGameplayTag(TEXT("Cheat.Time"))));

	World->DestroyWorld(false); World->SetPhysicsScene(nullptr); GEngine->DestroyWorldContext(World); World->RemoveFromRoot();
	return true;
}

#endif
