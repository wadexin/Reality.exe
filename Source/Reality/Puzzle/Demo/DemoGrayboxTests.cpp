// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"

#include "AI/RealityWitnessComponent.h"
#include "Developer/RealityEditableComponent.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
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
	EditableRotor->ConfigureTimeMachinery(FText::FromString(TEXT("Editable Rotor")), 90.0f);
	ADemoTimeReferenceActor* ReferenceRotor = TestWorld->SpawnActor<ADemoTimeReferenceActor>();
	TestTrue(TEXT("Editable rotor supports local Time"), EditableRotor->EditableComponent->SupportsCheat(FGameplayTag::RequestGameplayTag(TEXT("Cheat.Time"))));
	TestNull(TEXT("Reference rotor is not a selectable Reality target"), ReferenceRotor->FindComponentByClass<URealityEditableComponent>());
	EditableRotor->Tick(0.25f);
	TestEqual(TEXT("Machinery consumes Actor-local Tick delta"), EditableRotor->AccumulatedTickSeconds, 0.25f);
	ReferenceRotor->Tick(0.25f);
	TestEqual(TEXT("Reference machinery provides the same baseline Tick cadence"), ReferenceRotor->AccumulatedTickSeconds, 0.25f);

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
	UPackage* DemoPackage = LoadPackage(nullptr, TEXT("/Game/Levels/Demo/Lvl_Demo_Graybox"), LOAD_None);
	return TestNotNull(TEXT("Public Demo graybox map package loads"), DemoPackage);
}

#endif
