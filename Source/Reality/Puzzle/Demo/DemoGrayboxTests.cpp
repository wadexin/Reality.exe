// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"

#include "AI/RealityWitnessComponent.h"
#include "Developer/RealityEditableComponent.h"
#include "Components/AudioComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "Misc/ConfigCacheIni.h"
#include "Misc/PackageName.h"
#include "Puzzle/Demo/DemoEditableActor.h"
#include "Puzzle/Demo/DemoExitTerminal.h"
#include "Puzzle/Demo/DemoSensorFailurePanel.h"
#include "Puzzle/Demo/DemoTimeMachineryActor.h"
#include "Puzzle/Demo/DemoTimeReferenceActor.h"
#include "Puzzle/Demo/DemoWitnessActor.h"
#include "RealitySystem/RealityContextComponent.h"
#include "UObject/Package.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FDemoGrayboxGlueTest,
	"Reality.Puzzle.Demo.GrayboxGlue",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::EngineFilter)

bool FDemoGrayboxGlueTest::RunTest(const FString& Parameters)
{
	const FName WorldName = MakeUniqueObjectName(nullptr, UWorld::StaticClass(), NAME_None, EUniqueObjectNameOptions::GloballyUnique);
	FWorldContext& WorldContext = GEngine->CreateNewWorldContext(EWorldType::Game);
	UWorld* TestWorld = UWorld::CreateWorld(EWorldType::Game, false, WorldName, GetTransientPackage());
	if (!TestNotNull(TEXT("A Demo glue test world can be created"), TestWorld))
	{
		GEngine->DestroyWorldContext(TestWorld);
		return false;
	}

	TestWorld->AddToRoot();
	WorldContext.SetCurrentWorld(TestWorld);
	TestWorld->InitializeActorsForPlay(FURL());
	TestWorld->BeginPlay();

	ADemoEditableActor* Editable = TestWorld->SpawnActor<ADemoEditableActor>();
	Editable->ConfigureDemoTarget(FText::FromString(TEXT("Test Prop")), true, true, true, true, true, false, true, false, true);
	TestEqual(TEXT("Configured target exposes its player-facing name"), Editable->EditableComponent->GetPlayerFacingName().ToString(), FString(TEXT("Test Prop")));
	TestTrue(TEXT("Configured target exposes Collision"), Editable->EditableComponent->SupportsCheat(FGameplayTag::RequestGameplayTag(TEXT("Cheat.Collision"))));
	TestTrue(TEXT("Configured target exposes Scale"), Editable->EditableComponent->SupportsCheat(FGameplayTag::RequestGameplayTag(TEXT("Cheat.Scale"))));
	TestTrue(TEXT("Configured target exposes Gravity"), Editable->EditableComponent->SupportsCheat(FGameplayTag::RequestGameplayTag(TEXT("Cheat.Gravity"))));
	TestTrue(TEXT("Configured target exposes Mass"), Editable->EditableComponent->SupportsCheat(FGameplayTag::RequestGameplayTag(TEXT("Cheat.Mass"))));
	TestTrue(TEXT("Configured target exposes Friction"), Editable->EditableComponent->SupportsCheat(FGameplayTag::RequestGameplayTag(TEXT("Cheat.Friction"))));
	TestFalse(TEXT("Unconfigured Time remains unavailable"), Editable->EditableComponent->SupportsCheat(FGameplayTag::RequestGameplayTag(TEXT("Cheat.Time"))));

	AActor* NonPawn = TestWorld->SpawnActor<AActor>();
	TestTrue(TEXT("Physical push target accepts a valid Interactor"), Editable->CanInteract_Implementation(NonPawn));

	ADemoSensorFailurePanel* SensorPanel = TestWorld->SpawnActor<ADemoSensorFailurePanel>();
	SensorPanel->ConfigureSensorFailure(1800.0f, false);
	TestFalse(TEXT("SensorFailure begins inactive"), SensorPanel->ContextComponent->IsContextActive());
	SensorPanel->Interact_Implementation(NonPawn);
	TestTrue(TEXT("Panel activates SensorFailure"), SensorPanel->ContextComponent->IsContextActive());
	SensorPanel->Interact_Implementation(NonPawn);
	TestFalse(TEXT("Panel restores the sensor array"), SensorPanel->ContextComponent->IsContextActive());
	TestTrue(TEXT("Panel owns only SensorFailure"), SensorPanel->ContextComponent->GetContextTags().HasTagExact(FGameplayTag::RequestGameplayTag(TEXT("Context.SensorFailure"))));

	ADemoTimeMachineryActor* EditableRotor = TestWorld->SpawnActor<ADemoTimeMachineryActor>();
	EditableRotor->ConfigureTimeMachinery(FText::FromString(TEXT("Editable Rotor")), 720.0f);
	TestEqual(TEXT("Configured machinery exposes its player-facing name"), EditableRotor->EditableComponent->GetPlayerFacingName().ToString(), FString(TEXT("Editable Rotor")));
	ADemoTimeReferenceActor* ReferenceRotor = TestWorld->SpawnActor<ADemoTimeReferenceActor>();
	if (!EditableRotor->HasActorBegunPlay()) EditableRotor->DispatchBeginPlay();
	if (!ReferenceRotor->HasActorBegunPlay()) ReferenceRotor->DispatchBeginPlay();
	TestTrue(TEXT("Editable rotor supports local Time"), EditableRotor->EditableComponent->SupportsCheat(FGameplayTag::RequestGameplayTag(TEXT("Cheat.Time"))));
	TestNull(TEXT("Reference rotor is not a selectable Reality target"), ReferenceRotor->FindComponentByClass<URealityEditableComponent>());
	TestTrue(TEXT("Editable positive marker follows the authoritative rotor"), EditableRotor->RotorTipPositive->GetAttachParent() == EditableRotor->RotorMesh.Get());
	TestTrue(TEXT("Editable negative marker follows the authoritative rotor"), EditableRotor->RotorTipNegative->GetAttachParent() == EditableRotor->RotorMesh.Get());
	TestTrue(TEXT("Reference positive marker follows the authoritative rotor"), ReferenceRotor->RotorTipPositive->GetAttachParent() == ReferenceRotor->RotorMesh.Get());
	TestTrue(TEXT("Reference negative marker follows the authoritative rotor"), ReferenceRotor->RotorTipNegative->GetAttachParent() == ReferenceRotor->RotorMesh.Get());
	TestEqual(TEXT("Primary light curtain spans passage width"), EditableRotor->PrimaryScannerPlane->GetRelativeScale3D().Y, 6.3);
	TestEqual(TEXT("Primary light curtain spans floor to ceiling"), EditableRotor->PrimaryScannerPlane->GetRelativeScale3D().Z, 3.0);
	TestEqual(TEXT("Visible curtain matches detection width"), EditableRotor->PrimaryScannerDetection->GetScaledBoxExtent().Y, 315.0);
	TestEqual(TEXT("Visible curtain matches detection height"), EditableRotor->PrimaryScannerDetection->GetScaledBoxExtent().Z, 150.0);
	TestEqual(TEXT("Scanner planes break the baseline direct-run timing alignment"), EditableRotor->SecondaryScannerPlane->GetRelativeLocation().X - EditableRotor->PrimaryScannerPlane->GetRelativeLocation().X, 300.0);
	TestEqual(TEXT("Visible scanner planes never physically block traversal"), EditableRotor->PrimaryScannerPlane->GetCollisionEnabled(), ECollisionEnabled::NoCollision);
	TestEqual(TEXT("Scanner detection overlaps Pawn"), EditableRotor->PrimaryScannerDetection->GetCollisionResponseToChannel(ECC_Pawn), ECR_Overlap);
	TestEqual(TEXT("Scanner detection ignores Developer Visibility traces"), EditableRotor->PrimaryScannerDetection->GetCollisionResponseToChannel(ECC_Visibility), ECR_Ignore);
	TestEqual(TEXT("Scanner detection ignores Camera traces"), EditableRotor->PrimaryScannerDetection->GetCollisionResponseToChannel(ECC_Camera), ECR_Ignore);
	TestEqual(TEXT("Security Rotor remains queryable through the light curtain"), EditableRotor->RotorMesh->GetCollisionResponseToChannel(ECC_Visibility), ECR_Block);
	TestEqual(TEXT("Security Rotor query shape does not physically block the player"), EditableRotor->RotorMesh->GetCollisionResponseToChannel(ECC_Pawn), ECR_Ignore);
	TestEqual(TEXT("Targeting proxy provides forgiving machine-body bounds"), EditableRotor->TargetingProxy->GetUnscaledBoxExtent(), FVector(110.0, 160.0, 140.0));
	TestEqual(TEXT("Targeting proxy blocks only Visibility"), EditableRotor->TargetingProxy->GetCollisionResponseToChannel(ECC_Visibility), ECR_Block);
	TestEqual(TEXT("Targeting proxy ignores Pawn"), EditableRotor->TargetingProxy->GetCollisionResponseToChannel(ECC_Pawn), ECR_Ignore);
	TestEqual(TEXT("Targeting proxy ignores Camera"), EditableRotor->TargetingProxy->GetCollisionResponseToChannel(ECC_Camera), ECR_Ignore);
	TestTrue(TEXT("Primary scanner begins active"), EditableRotor->IsPrimaryScannerActive());
	TestTrue(TEXT("Secondary scanner begins phase-offset active"), EditableRotor->IsSecondaryScannerActive());
	TestEqual(TEXT("Editable rotor uses the authored 1.0x cadence"), EditableRotor->RotationDegreesPerSecond, 720.0f);
	TestEqual(TEXT("Reference rotor shares the authored 1.0x cadence"), ReferenceRotor->RotationDegreesPerSecond, 720.0f);
	TestEqual(TEXT("Editable cadence markers do not add collision"), EditableRotor->RotorTipPositive->GetCollisionEnabled(), ECollisionEnabled::NoCollision);
	TestEqual(TEXT("Reference cadence markers do not add collision"), ReferenceRotor->RotorTipPositive->GetCollisionEnabled(), ECollisionEnabled::NoCollision);
	EditableRotor->Tick(0.25f);
	TestEqual(TEXT("Machinery consumes Actor-local Tick delta"), EditableRotor->AccumulatedTickSeconds, 0.25f);
	TestTrue(TEXT("Primary scanner follows the active half-revolution rotor phase"), EditableRotor->IsPrimaryScannerActive());
	TestFalse(TEXT("Secondary scanner uses a 90-degree rotor phase offset"), EditableRotor->IsSecondaryScannerActive());
	ReferenceRotor->Tick(0.25f);
	TestEqual(TEXT("Reference machinery provides the same baseline Tick cadence"), ReferenceRotor->AccumulatedTickSeconds, 0.25f);
	TestEqual(TEXT("Editable rotor audio begins at baseline"), EditableRotor->GetPresentationAudioPitch(), 1.0f);
	TestEqual(TEXT("Reference rotor owns an independent stable audio baseline"), ReferenceRotor->GetPresentationAudioPitch(), 1.0f);
	AActor* AudioInstigator = TestWorld->SpawnActor<AActor>();
	const int32 InitialAudioUpdates = EditableRotor->GetAudioStateUpdateCount();
	TArray<UAudioComponent*> EditableAudioComponents;
	EditableRotor->GetComponents<UAudioComponent>(EditableAudioComponents);
	TestEqual(TEXT("Editable rotor owns exactly one reusable audio loop"), EditableAudioComponents.Num(), 1);
	TestTrue(TEXT("Time Quarter updates editable presentation audio"), EditableRotor->EditableComponent->ApplyTimeModification(ERealityTimePreset::Quarter, AudioInstigator));
	TestEqual(TEXT("Quarter uses a controlled half-pitch cadence"), EditableRotor->GetPresentationAudioPitch(), 0.5f);
	TestTrue(TEXT("Time Half updates editable presentation audio"), EditableRotor->EditableComponent->ApplyTimeModification(ERealityTimePreset::Half, AudioInstigator));
	TestEqual(TEXT("Half uses a controlled 0.7 pitch cadence"), EditableRotor->GetPresentationAudioPitch(), 0.7f);
	TestTrue(TEXT("Time One returns audible baseline while remaining logically modified"), EditableRotor->EditableComponent->ApplyTimeModification(ERealityTimePreset::One, AudioInstigator));
	TestEqual(TEXT("One uses exact baseline pitch"), EditableRotor->GetPresentationAudioPitch(), 1.0f);
	TestTrue(TEXT("Time Double updates editable presentation audio"), EditableRotor->EditableComponent->ApplyTimeModification(ERealityTimePreset::Double, AudioInstigator));
	TestEqual(TEXT("Double uses a controlled 1.4 pitch cadence"), EditableRotor->GetPresentationAudioPitch(), 1.4f);
	TestTrue(TEXT("Time Quadruple updates editable presentation audio"), EditableRotor->EditableComponent->ApplyTimeModification(ERealityTimePreset::Quadruple, AudioInstigator));
	TestEqual(TEXT("Quadruple uses a capped 2.0 pitch cadence"), EditableRotor->GetPresentationAudioPitch(), 2.0f);
	const int32 SuccessfulTimeUpdates = EditableRotor->GetAudioStateUpdateCount();
	TestTrue(TEXT("Successful presets update one existing loop"), SuccessfulTimeUpdates >= InitialAudioUpdates + 5);
	TestFalse(TEXT("Repeated Time preset is a no-op"), EditableRotor->EditableComponent->ApplyTimeModification(ERealityTimePreset::Quadruple, AudioInstigator));
	TestEqual(TEXT("Repeated Time preset does not update or duplicate audio"), EditableRotor->GetAudioStateUpdateCount(), SuccessfulTimeUpdates);
	EditableAudioComponents.Reset();
	EditableRotor->GetComponents<UAudioComponent>(EditableAudioComponents);
	TestEqual(TEXT("Repeated state changes do not create duplicate loops"), EditableAudioComponents.Num(), 1);
	TestEqual(TEXT("Reference rotor remains fixed while editable rotor changes"), ReferenceRotor->GetPresentationAudioPitch(), 1.0f);
	TestTrue(TEXT("Time Restore updates editable presentation audio"), EditableRotor->EditableComponent->RestoreTimeModification(AudioInstigator));
	TestEqual(TEXT("Restore returns exact audio baseline"), EditableRotor->GetPresentationAudioPitch(), 1.0f);

	ADemoWitnessActor* Witness = TestWorld->SpawnActor<ADemoWitnessActor>();
	Witness->ConfigureDemoWitness(true, 1200.0f, 90.0f, true);
	TestEqual(TEXT("Visible Camera uses the existing Camera tag"), Witness->WitnessComponent->GetWitnessType(), FGameplayTag::RequestGameplayTag(TEXT("Witness.Camera")));

	ADemoExitTerminal* Exit = TestWorld->SpawnActor<ADemoExitTerminal>();
	APawn* Pawn = TestWorld->SpawnActor<APawn>();
	TestFalse(TEXT("Demo exit begins incomplete"), Exit->HasCompletedDemo());
	TestFalse(TEXT("Non-Pawn cannot complete the Demo"), Exit->CompleteDemo(NonPawn));
	TestTrue(TEXT("Pawn completes the Demo once"), Exit->CompleteDemo(Pawn));
	TestTrue(TEXT("Demo exit records completion"), Exit->HasCompletedDemo());
	TestFalse(TEXT("Repeated completion is a safe no-op"), Exit->CompleteDemo(Pawn));
	TestFalse(TEXT("Only inherent machinery uses Tick"), Editable->PrimaryActorTick.bCanEverTick || SensorPanel->PrimaryActorTick.bCanEverTick || Witness->PrimaryActorTick.bCanEverTick || Exit->PrimaryActorTick.bCanEverTick);

	TestWorld->DestroyWorld(false);
	TestWorld->SetPhysicsScene(nullptr);
	GEngine->DestroyWorldContext(TestWorld);
	TestWorld->RemoveFromRoot();
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FDemoGrayboxMapLoadTest,
	"Reality.Puzzle.Demo.MapLoad",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::EngineFilter)

bool FDemoGrayboxMapLoadTest::RunTest(const FString& Parameters)
{
	FString StartupMap;
	GConfig->GetString(TEXT("/Script/EngineSettings.GameMapsSettings"), TEXT("GameDefaultMap"), StartupMap, GEngineIni);
	TestEqual(
		TEXT("Standalone startup enters the public Demo"),
		FPackageName::ObjectPathToPackageName(StartupMap),
		FString(TEXT("/Game/Levels/Demo/Lvl_Demo_Graybox")));
	UPackage* DemoPackage = LoadPackage(nullptr, TEXT("/Game/Levels/Demo/Lvl_Demo_Graybox"), LOAD_None);
	return TestNotNull(TEXT("Public Demo graybox map package loads"), DemoPackage);
}

#endif
