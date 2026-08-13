// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"

#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Puzzle/Prototype/P0ContextToggleSwitch.h"
#include "Puzzle/Prototype/P0PrototypeExit.h"
#include "RealitySystem/RealityContextComponent.h"
#include "RealitySystem/RealityContextTestActor.h"
#include "RealitySystem/RealityManagerSubsystem.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FP0PrototypeGlueTest,
	"Reality.Puzzle.P0PrototypeGlue",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::EngineFilter)

bool FP0PrototypeGlueTest::RunTest(const FString& Parameters)
{
	const FName WorldName = MakeUniqueObjectName(nullptr, UWorld::StaticClass(), NAME_None, EUniqueObjectNameOptions::GloballyUnique);
	FWorldContext& WorldContext = GEngine->CreateNewWorldContext(EWorldType::Game);
	UWorld* TestWorld = UWorld::CreateWorld(EWorldType::Game, false, WorldName, GetTransientPackage());
	if (!TestNotNull(TEXT("A P0 glue test world can be created"), TestWorld))
	{
		GEngine->DestroyWorldContext(TestWorld);
		return false;
	}

	TestWorld->AddToRoot();
	WorldContext.SetCurrentWorld(TestWorld);
	TestWorld->InitializeActorsForPlay(FURL());
	TestWorld->BeginPlay();

	ARealityContextTestActor* ContextSource = TestWorld->SpawnActor<ARealityContextTestActor>();
	AP0ContextToggleSwitch* ContextSwitch = TestWorld->SpawnActor<AP0ContextToggleSwitch>();
	AActor* Interactor = TestWorld->SpawnActor<AActor>();
	ContextSwitch->SetContextSourceActor(ContextSource);
	TestTrue(TEXT("Context switch accepts a valid source and Interactor"), ContextSwitch->CanInteract_Implementation(Interactor));
	TestTrue(TEXT("Context starts active"), ContextSource->ContextComponent->IsContextActive());
	ContextSwitch->Interact_Implementation(Interactor);
	TestFalse(TEXT("First interaction deactivates Context"), ContextSource->ContextComponent->IsContextActive());
	ContextSwitch->Interact_Implementation(Interactor);
	TestTrue(TEXT("Second interaction reactivates Context"), ContextSource->ContextComponent->IsContextActive());
	ContextSwitch->SetContextSourceActor(nullptr);
	TestFalse(TEXT("Missing source fails safely"), ContextSwitch->CanInteract_Implementation(Interactor));

	AP0PrototypeExit* Exit = TestWorld->SpawnActor<AP0PrototypeExit>();
	TestFalse(TEXT("Exit starts incomplete"), Exit->HasCompletedPrototype());
	TestFalse(TEXT("Prototype helpers do not tick"), ContextSwitch->PrimaryActorTick.bCanEverTick || Exit->PrimaryActorTick.bCanEverTick);

	TestWorld->DestroyWorld(false);
	TestWorld->SetPhysicsScene(nullptr);
	GEngine->DestroyWorldContext(TestWorld);
	TestWorld->RemoveFromRoot();
	return true;
}

#endif
