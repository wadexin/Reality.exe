// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"

#include "Components/BoxComponent.h"
#include "Developer/DeveloperModeComponent.h"
#include "Developer/RealityEditableComponent.h"
#include "Developer/RealityEditableTestActor.h"
#include "Engine/CollisionProfile.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "RealitySystem/RealityManagerSubsystem.h"
#include "UI/DeveloperConsoleWidget.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FDeveloperConsoleWidgetTest,
	"Reality.UI.DeveloperConsole",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::EngineFilter)

bool FDeveloperConsoleWidgetTest::RunTest(const FString& Parameters)
{
	const FGameplayTag CollisionTag = FGameplayTag::RequestGameplayTag(TEXT("Cheat.Collision"));
	const FGameplayTag ScaleTag = FGameplayTag::RequestGameplayTag(TEXT("Cheat.Scale"));
	const FGameplayTag GravityTag = FGameplayTag::RequestGameplayTag(TEXT("Cheat.Gravity"));
	const FGameplayTag MassTag = FGameplayTag::RequestGameplayTag(TEXT("Cheat.Mass"));
	const FGameplayTag FrictionTag = FGameplayTag::RequestGameplayTag(TEXT("Cheat.Friction"));

	const FName WorldName = MakeUniqueObjectName(nullptr, UWorld::StaticClass(), NAME_None, EUniqueObjectNameOptions::GloballyUnique);
	FWorldContext& WorldContext = GEngine->CreateNewWorldContext(EWorldType::Game);
	UWorld* TestWorld = UWorld::CreateWorld(EWorldType::Game, false, WorldName, GetTransientPackage());
	if (!TestNotNull(TEXT("A Developer Console test world can be created"), TestWorld))
	{
		GEngine->DestroyWorldContext(TestWorld);
		return false;
	}

	TestWorld->AddToRoot();
	WorldContext.SetCurrentWorld(TestWorld);
	TestWorld->InitializeActorsForPlay(FURL());
	TestWorld->BeginPlay();

	AActor* PlayerActor = TestWorld->SpawnActor<AActor>(FVector::ZeroVector, FRotator::ZeroRotator);
	UDeveloperModeComponent* DeveloperComponent = NewObject<UDeveloperModeComponent>(PlayerActor, TEXT("DeveloperModeComponent"));
	PlayerActor->AddInstanceComponent(DeveloperComponent);
	DeveloperComponent->RegisterComponent();

	UDeveloperConsoleWidget* Widget = NewObject<UDeveloperConsoleWidget>(TestWorld, TEXT("DeveloperConsoleWidget"));
	Widget->SetDeveloperModeComponent(DeveloperComponent);
	TestFalse(TEXT("Console safely reports no target before Developer Mode selects one"), Widget->HasEditableTarget());
	TestFalse(TEXT("No target exposes no Collision section"), Widget->IsCheatSectionAvailable(CollisionTag));
	TestFalse(TEXT("No target rejects actions"), Widget->ExecuteCollisionToggle());

	ARealityEditableTestActor* Target = TestWorld->SpawnActor<ARealityEditableTestActor>(FVector(150.0f, 0.0f, 0.0f), FRotator::ZeroRotator);
	FGameplayTagContainer Capabilities;
	Capabilities.AddTag(CollisionTag);
	Capabilities.AddTag(ScaleTag);
	Capabilities.AddTag(GravityTag);
	Capabilities.AddTag(MassTag);
	Capabilities.AddTag(FrictionTag);
	Target->EditableComponent->SetSupportedCheats(Capabilities);
	Target->PrimitiveA->SetCollisionProfileName(UCollisionProfile::PhysicsActor_ProfileName);
	Target->PrimitiveA->SetSimulatePhysics(true);
	Target->PrimitiveA->SetEnableGravity(true);
	Target->EditableComponent->OnRealityCheatEvent.AddDynamic(Target, &ARealityEditableTestActor::HandleRealityCheatEvent);

	DeveloperComponent->EnterDeveloperMode();
	TestTrue(TEXT("Console reports the selected editable target"), Widget->HasEditableTarget());
	TestTrue(TEXT("Collision capability drives section availability"), Widget->IsCheatSectionAvailable(CollisionTag));
	TestTrue(TEXT("Scale capability drives section availability"), Widget->IsCheatSectionAvailable(ScaleTag));
	TestTrue(TEXT("Gravity capability drives section availability"), Widget->IsCheatSectionAvailable(GravityTag));
	TestTrue(TEXT("Mass capability drives section availability"), Widget->IsCheatSectionAvailable(MassTag));
	TestTrue(TEXT("Friction capability drives section availability"), Widget->IsCheatSectionAvailable(FrictionTag));

	TestTrue(TEXT("Console Collision action invokes existing implementation"), Widget->ExecuteCollisionToggle());
	TestTrue(TEXT("Collision is modified once"), Target->EditableComponent->IsCollisionModified());
	TestEqual(TEXT("One console action emits one event"), Target->CheatEventCount, 1);
	TestTrue(TEXT("Console Collision restores independently"), Widget->ExecuteCollisionToggle());
	TestFalse(TEXT("Collision restore completes"), Target->EditableComponent->IsCollisionModified());

	TestTrue(TEXT("Console typed Scale preset succeeds"), Widget->ExecuteScalePreset(ERealityScalePreset::Half));
	TestEqual(TEXT("Console Scale reaches existing implementation"), Target->GetActorScale3D(), FVector(0.5f));
	TestTrue(TEXT("Console typed Gravity preset succeeds"), Widget->ExecuteGravityPreset(ERealityGravityPreset::Zero));
	TestEqual(TEXT("Console Gravity reaches existing implementation"), Target->EditableComponent->GetCurrentGravityPreset(), ERealityGravityPreset::Zero);
	TestTrue(TEXT("Scale restore remains independent"), Widget->ExecuteScaleRestore());
	TestFalse(TEXT("Scale cycle ends while Gravity remains active"), Target->EditableComponent->IsScaleModified());
	TestTrue(TEXT("Gravity remains independently modified"), Target->EditableComponent->IsGravityModified());
	TestTrue(TEXT("Gravity restore succeeds independently"), Widget->ExecuteGravityRestore());
	TestFalse(TEXT("Gravity cycle ends"), Target->EditableComponent->IsGravityModified());
	TestTrue(TEXT("Console typed Mass preset succeeds"), Widget->ExecuteMassPreset(ERealityMassPreset::Half));
	TestTrue(TEXT("Console Mass reaches existing implementation"), Target->EditableComponent->IsMassModified());
	TestTrue(TEXT("Console Mass restore succeeds"), Widget->ExecuteMassRestore());
	TestFalse(TEXT("Mass cycle ends"), Target->EditableComponent->IsMassModified());
	TestTrue(TEXT("Console typed Friction preset succeeds"), Widget->ExecuteFrictionPreset(ERealityFrictionPreset::Low));
	TestTrue(TEXT("Console Friction reaches existing implementation"), Target->EditableComponent->IsFrictionModified());
	TestTrue(TEXT("Console Friction restore succeeds"), Widget->ExecuteFrictionRestore());
	TestFalse(TEXT("Friction cycle ends"), Target->EditableComponent->IsFrictionModified());

	URealityManagerSubsystem* Manager = TestWorld->GetSubsystem<URealityManagerSubsystem>();
	TestNotNull(TEXT("Console actions retain Reality Manager integration"), Manager);
	TestTrue(TEXT("Reality display source reflects accumulated Suspicion"), Manager && Manager->GetSuspicion() > 0.0f);
	TestNotNull(TEXT("Reality display source exposes the latest processed event"), Manager ? Manager->GetMostRecentEvent() : nullptr);

	Target->PrimitiveA->SetSimulatePhysics(false);
	Target->Destroy();
	TestFalse(TEXT("Destroyed target is cleared safely"), Widget->HasEditableTarget());
	TestFalse(TEXT("Destroyed target actions fail safely"), Widget->ExecuteScalePreset(ERealityScalePreset::Double));
	DeveloperComponent->ExitDeveloperMode();

	TestWorld->DestroyWorld(false);
	TestWorld->SetPhysicsScene(nullptr);
	GEngine->DestroyWorldContext(TestWorld);
	TestWorld->RemoveFromRoot();
	return true;
}

#endif
