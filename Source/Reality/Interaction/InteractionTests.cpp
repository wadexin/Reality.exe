// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "InputCoreTypes.h"
#include "Interaction/Interactable.h"
#include "Interaction/InteractionComponent.h"
#include "Interaction/InteractionTestActor.h"
#include "RealityCharacter.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRealityInteractionComponentTest,
	"Reality.Interaction.Component",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::EngineFilter)

bool FRealityInteractionComponentTest::RunTest(const FString& Parameters)
{
	UInteractionComponent* OrphanComponent = NewObject<UInteractionComponent>();
	TestFalse(TEXT("An interaction component without an owner fails safely"), OrphanComponent->InteractWithFocusedActor());
	OrphanComponent->UpdateFocus();
	TestNull(TEXT("An ownerless component has no focus"), OrphanComponent->GetFocusedActor());

	ARealityCharacter* CharacterCDO = ARealityCharacter::StaticClass()->GetDefaultObject<ARealityCharacter>();
	UInputAction* InteractionAction = FindObjectFast<UInputAction>(CharacterCDO, TEXT("Interaction Action"));
	UInputMappingContext* InteractionMappingContext = FindObjectFast<UInputMappingContext>(CharacterCDO, TEXT("Interaction Mapping Context"));
	TestNotNull(TEXT("The base character owns an Enhanced Input interaction action"), InteractionAction);
	TestNotNull(TEXT("The base character owns an interaction mapping context"), InteractionMappingContext);
	if (InteractionAction && InteractionMappingContext)
	{
		const bool bHasDefaultEKey = InteractionMappingContext->GetMappings().ContainsByPredicate(
			[InteractionAction](const FEnhancedActionKeyMapping& Mapping)
			{
				return Mapping.Action == InteractionAction && Mapping.Key == EKeys::E;
			});
		TestTrue(TEXT("The interaction action defaults to the E key"), bHasDefaultEKey);
	}

	UClass* FirstPersonCharacterClass = LoadClass<ARealityCharacter>(nullptr, TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter.BP_FirstPersonCharacter_C"));
	TestNotNull(TEXT("The existing First Person character Blueprint still loads"), FirstPersonCharacterClass);
	if (FirstPersonCharacterClass)
	{
		ARealityCharacter* FirstPersonCharacterCDO = FirstPersonCharacterClass->GetDefaultObject<ARealityCharacter>();
		TestNotNull(TEXT("The existing First Person character inherits the interaction component"), FirstPersonCharacterCDO->FindComponentByClass<UInteractionComponent>());
		UInputAction* FirstPersonInteractionAction = FirstPersonCharacterCDO->GetInteractionAction();
		UInputMappingContext* FirstPersonInteractionMappingContext = FirstPersonCharacterCDO->GetInteractionMappingContext();
		TestNotNull(TEXT("The existing First Person character inherits the interaction action"), FirstPersonInteractionAction);
		TestNotNull(TEXT("The existing First Person character inherits the interaction mapping context"), FirstPersonInteractionMappingContext);
		if (FirstPersonInteractionAction && FirstPersonInteractionMappingContext)
		{
			const bool bFirstPersonHasDefaultEKey = FirstPersonInteractionMappingContext->GetMappings().ContainsByPredicate(
				[FirstPersonInteractionAction](const FEnhancedActionKeyMapping& Mapping)
				{
					return Mapping.Action == FirstPersonInteractionAction && Mapping.Key == EKeys::E;
				});
			TestTrue(TEXT("The existing First Person character retains the default E interaction mapping"), bFirstPersonHasDefaultEKey);
		}
	}

	const FName WorldName = MakeUniqueObjectName(nullptr, UWorld::StaticClass(), NAME_None, EUniqueObjectNameOptions::GloballyUnique);
	FWorldContext& WorldContext = GEngine->CreateNewWorldContext(EWorldType::Game);
	UWorld* TestWorld = UWorld::CreateWorld(EWorldType::Game, false, WorldName, GetTransientPackage());
	if (!TestNotNull(TEXT("A test world can be created"), TestWorld))
	{
		return false;
	}

	TestWorld->AddToRoot();
	WorldContext.SetCurrentWorld(TestWorld);
	TestWorld->InitializeActorsForPlay(FURL());

	AActor* Interactor = TestWorld->SpawnActor<AActor>(FVector::ZeroVector, FRotator::ZeroRotator);
	UInteractionComponent* InteractionComponent = NewObject<UInteractionComponent>(Interactor, TEXT("TestInteractionComponent"));
	Interactor->AddInstanceComponent(InteractionComponent);
	InteractionComponent->RegisterComponent();
	InteractionComponent->InteractionDistance = 300.0f;

	AInteractionTestActor* TestInteractable = TestWorld->SpawnActor<AInteractionTestActor>(FVector(150.0f, 0.0f, 0.0f), FRotator::ZeroRotator);
	TestInteractable->Mesh->SetWorldScale3D(FVector(0.5f));
	TestInteractable->Mesh->RecreatePhysicsState();
	InteractionComponent->OnFocusGained.AddDynamic(TestInteractable, &AInteractionTestActor::HandleFocusGained);
	InteractionComponent->OnFocusLost.AddDynamic(TestInteractable, &AInteractionTestActor::HandleFocusLost);

	InteractionComponent->UpdateFocus();
	TestEqual(TEXT("A visible allowed interactable gains focus"), InteractionComponent->GetFocusedActor(), static_cast<AActor*>(TestInteractable));
	TestEqual(TEXT("Focus gained is broadcast exactly once"), TestInteractable->FocusGainedCount, 1);
	TestTrue(TEXT("The focused interactable executes"), InteractionComponent->InteractWithFocusedActor());
	TestEqual(TEXT("Interaction executes exactly once"), TestInteractable->InteractionCount, 1);
	TestInteractable->bInteractionAllowed = false;
	TestFalse(TEXT("An interactable that becomes disallowed rejects interaction"), InteractionComponent->InteractWithFocusedActor());
	TestNull(TEXT("A disallowed focused actor is cleared safely"), InteractionComponent->GetFocusedActor());
	TestEqual(TEXT("Losing eligibility broadcasts focus lost"), TestInteractable->FocusLostCount, 1);
	TestInteractable->bInteractionAllowed = true;
	InteractionComponent->UpdateFocus();
	TestEqual(TEXT("An interactable can regain focus after becoming allowed"), TestInteractable->FocusGainedCount, 2);

	TestInteractable->SetActorLocation(FVector(150.0f, 150.0f, 0.0f));
	InteractionComponent->UpdateFocus();
	TestNull(TEXT("Looking away from the interactable clears focus"), InteractionComponent->GetFocusedActor());
	TestEqual(TEXT("Looking away broadcasts focus lost"), TestInteractable->FocusLostCount, 2);

	AActor* BlockingActor = TestWorld->SpawnActor<AActor>(FVector(100.0f, 0.0f, 0.0f), FRotator::ZeroRotator);
	UStaticMeshComponent* BlockingMesh = NewObject<UStaticMeshComponent>(BlockingActor, TEXT("BlockingMesh"));
	BlockingActor->SetRootComponent(BlockingMesh);
	BlockingActor->AddInstanceComponent(BlockingMesh);
	BlockingMesh->SetStaticMesh(TestInteractable->Mesh->GetStaticMesh());
	BlockingMesh->SetCollisionProfileName(TEXT("BlockAll"));
	BlockingMesh->RegisterComponent();
	BlockingMesh->RecreatePhysicsState();
	InteractionComponent->UpdateFocus();
	TestNull(TEXT("A non-interactable hit is ignored"), InteractionComponent->GetFocusedActor());

	InteractionComponent->SetFocusTracingEnabled(false);
	TestFalse(TEXT("Focus Tick can be disabled"), InteractionComponent->IsComponentTickEnabled());

	TestWorld->DestroyWorld(false);
	TestWorld->SetPhysicsScene(nullptr);
	GEngine->DestroyWorldContext(TestWorld);
	TestWorld->RemoveFromRoot();
	return true;
}

#endif
