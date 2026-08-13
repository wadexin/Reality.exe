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
#include "GameFramework/Actor.h"
#include "InputAction.h"
#include "InputCoreTypes.h"
#include "InputMappingContext.h"
#include "Interaction/Interactable.h"
#include "RealityCharacter.h"

namespace DeveloperModeTests
{
	FGameplayTag GetCollisionTag()
	{
		return FGameplayTag::RequestGameplayTag(TEXT("Cheat.Collision"));
	}

	FGameplayTag GetScaleTag()
	{
		return FGameplayTag::RequestGameplayTag(TEXT("Cheat.Scale"));
	}

	FGameplayTag GetGravityTag()
	{
		return FGameplayTag::RequestGameplayTag(TEXT("Cheat.Gravity"));
	}

	void EnableCollisionCapability(URealityEditableComponent* EditableComponent)
	{
		FGameplayTagContainer SupportedCheats;
		SupportedCheats.AddTag(GetCollisionTag());
		SupportedCheats.AddTag(GetScaleTag());
		EditableComponent->SetSupportedCheats(SupportedCheats);
	}

	ARealityEditableTestActor* SpawnEditable(UWorld* World, const FVector& Location, const bool bSupportsCollision = true)
	{
		ARealityEditableTestActor* TestActor = World->SpawnActor<ARealityEditableTestActor>(Location, FRotator::ZeroRotator);
		TestActor->PrimitiveA->SetBoxExtent(FVector(25.0f));
		TestActor->PrimitiveB->SetBoxExtent(FVector(20.0f));
		TestActor->PrimitiveB->SetRelativeLocation(FVector(0.0f, 0.0f, 55.0f));
		TestActor->PrimitiveA->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);
		TestActor->PrimitiveB->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);
		TestActor->PrimitiveA->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		TestActor->PrimitiveB->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		TestActor->PrimitiveA->RecreatePhysicsState();
		TestActor->PrimitiveB->RecreatePhysicsState();
		if (bSupportsCollision)
		{
			EnableCollisionCapability(TestActor->EditableComponent);
		}
		return TestActor;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FDeveloperModeComponentTest,
	"Reality.Developer.ModeShell",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::EngineFilter)

bool FDeveloperModeComponentTest::RunTest(const FString& Parameters)
{
	using namespace DeveloperModeTests;

	ARealityCharacter* CharacterCDO = ARealityCharacter::StaticClass()->GetDefaultObject<ARealityCharacter>();
	UDeveloperModeComponent* DeveloperModeCDO = CharacterCDO->GetDeveloperModeComponent();
	TestNotNull(TEXT("The base character owns a Developer Mode component"), DeveloperModeCDO);
	TestFalse(TEXT("Developer Mode defaults to inactive"), DeveloperModeCDO->IsDeveloperModeActive());

	UInputAction* DeveloperModeAction = CharacterCDO->GetDeveloperModeAction();
	UInputAction* DeveloperCollisionAction = CharacterCDO->GetDeveloperCollisionAction();
	const TArray<TObjectPtr<UInputAction>>& DeveloperScaleActions = CharacterCDO->GetDeveloperScaleActions();
	UInputAction* DeveloperScaleRestoreAction = CharacterCDO->GetDeveloperScaleRestoreAction();
	UInputAction* DeveloperGravityCycleAction = CharacterCDO->GetDeveloperGravityCycleAction();
	UInputAction* DeveloperGravityRestoreAction = CharacterCDO->GetDeveloperGravityRestoreAction();
	UInputMappingContext* DeveloperMappingContext = CharacterCDO->GetDeveloperMappingContext();
	TestNotNull(TEXT("The character owns a Developer Mode Enhanced Input action"), DeveloperModeAction);
	TestNotNull(TEXT("The character owns a Developer Collision Enhanced Input action"), DeveloperCollisionAction);
	TestEqual(TEXT("The character owns five Developer Scale preset actions"), DeveloperScaleActions.Num(), 5);
	TestNotNull(TEXT("The character owns a Developer Scale Restore action"), DeveloperScaleRestoreAction);
	TestNotNull(TEXT("The character owns a Developer Gravity Cycle action"), DeveloperGravityCycleAction);
	TestNotNull(TEXT("The character owns a Developer Gravity Restore action"), DeveloperGravityRestoreAction);
	TestNotNull(TEXT("The character owns a Developer mapping context"), DeveloperMappingContext);
	if (DeveloperModeAction && DeveloperCollisionAction && DeveloperMappingContext)
	{
		const bool bHasF6Mapping = DeveloperMappingContext->GetMappings().ContainsByPredicate(
			[DeveloperModeAction](const FEnhancedActionKeyMapping& Mapping)
			{
				return Mapping.Action == DeveloperModeAction && Mapping.Key == EKeys::F6;
			});
		const bool bHasF1Mapping = DeveloperMappingContext->GetMappings().ContainsByPredicate(
			[DeveloperModeAction](const FEnhancedActionKeyMapping& Mapping)
			{
				return Mapping.Action == DeveloperModeAction && Mapping.Key == EKeys::F1;
			});
		const bool bHasCollisionMapping = DeveloperMappingContext->GetMappings().ContainsByPredicate(
			[DeveloperCollisionAction](const FEnhancedActionKeyMapping& Mapping)
			{
				return Mapping.Action == DeveloperCollisionAction && Mapping.Key == EKeys::R;
			});
		TestTrue(TEXT("Developer Mode defaults to F6"), bHasF6Mapping);
		TestFalse(TEXT("Reality leaves F1 unused"), bHasF1Mapping);
		TestTrue(TEXT("The prototype Collision action defaults to R"), bHasCollisionMapping);
		const FKey ExpectedScaleKeys[] = {EKeys::One, EKeys::Two, EKeys::Three, EKeys::Four, EKeys::Five};
		for (int32 ScaleActionIndex = 0; ScaleActionIndex < DeveloperScaleActions.Num(); ++ScaleActionIndex)
		{
			const bool bHasScaleMapping = DeveloperMappingContext->GetMappings().ContainsByPredicate(
				[&DeveloperScaleActions, &ExpectedScaleKeys, ScaleActionIndex](const FEnhancedActionKeyMapping& Mapping)
				{
					return Mapping.Action == DeveloperScaleActions[ScaleActionIndex] && Mapping.Key == ExpectedScaleKeys[ScaleActionIndex];
				});
			TestTrue(*FString::Printf(TEXT("Scale preset action %d uses its expected number key"), ScaleActionIndex), bHasScaleMapping);
		}
		TestTrue(
			TEXT("Scale Restore defaults to T"),
			DeveloperMappingContext->GetMappings().ContainsByPredicate(
				[DeveloperScaleRestoreAction](const FEnhancedActionKeyMapping& Mapping)
				{
					return Mapping.Action == DeveloperScaleRestoreAction && Mapping.Key == EKeys::T;
				}));
		TestTrue(
			TEXT("Gravity Cycle defaults to G"),
			DeveloperMappingContext->GetMappings().ContainsByPredicate(
				[DeveloperGravityCycleAction](const FEnhancedActionKeyMapping& Mapping)
				{
					return Mapping.Action == DeveloperGravityCycleAction && Mapping.Key == EKeys::G;
				}));
		TestTrue(
			TEXT("Gravity Restore defaults to H"),
			DeveloperMappingContext->GetMappings().ContainsByPredicate(
				[DeveloperGravityRestoreAction](const FEnhancedActionKeyMapping& Mapping)
				{
					return Mapping.Action == DeveloperGravityRestoreAction && Mapping.Key == EKeys::H;
				}));
	}

	UClass* FirstPersonCharacterClass = LoadClass<ARealityCharacter>(nullptr, TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter.BP_FirstPersonCharacter_C"));
	TestNotNull(TEXT("The existing First Person character Blueprint still loads"), FirstPersonCharacterClass);
	if (FirstPersonCharacterClass)
	{
		ARealityCharacter* FirstPersonCDO = FirstPersonCharacterClass->GetDefaultObject<ARealityCharacter>();
		TestNotNull(TEXT("The First Person Blueprint inherits Developer Mode"), FirstPersonCDO->GetDeveloperModeComponent());
		UInputAction* FirstPersonModeAction = FirstPersonCDO->GetDeveloperModeAction();
		UInputAction* FirstPersonCollisionAction = FirstPersonCDO->GetDeveloperCollisionAction();
		const TArray<TObjectPtr<UInputAction>>& FirstPersonScaleActions = FirstPersonCDO->GetDeveloperScaleActions();
		UInputAction* FirstPersonScaleRestoreAction = FirstPersonCDO->GetDeveloperScaleRestoreAction();
		UInputAction* FirstPersonGravityCycleAction = FirstPersonCDO->GetDeveloperGravityCycleAction();
		UInputAction* FirstPersonGravityRestoreAction = FirstPersonCDO->GetDeveloperGravityRestoreAction();
		UInputMappingContext* FirstPersonMappingContext = FirstPersonCDO->GetDeveloperMappingContext();
		TestNotNull(TEXT("The First Person Blueprint inherits the F6 action"), FirstPersonModeAction);
		TestNotNull(TEXT("The First Person Blueprint inherits the R action"), FirstPersonCollisionAction);
		TestEqual(TEXT("The First Person Blueprint inherits five Scale actions"), FirstPersonScaleActions.Num(), 5);
		TestNotNull(TEXT("The First Person Blueprint inherits the Scale Restore action"), FirstPersonScaleRestoreAction);
		TestNotNull(TEXT("The First Person Blueprint inherits the Gravity Cycle action"), FirstPersonGravityCycleAction);
		TestNotNull(TEXT("The First Person Blueprint inherits the Gravity Restore action"), FirstPersonGravityRestoreAction);
		TestNotNull(TEXT("The First Person Blueprint inherits the Developer mapping context"), FirstPersonMappingContext);
		if (FirstPersonModeAction && FirstPersonCollisionAction && FirstPersonMappingContext)
		{
			TestTrue(
				TEXT("The First Person Blueprint retains the F6 mapping"),
				FirstPersonMappingContext->GetMappings().ContainsByPredicate(
					[FirstPersonModeAction](const FEnhancedActionKeyMapping& Mapping)
					{
						return Mapping.Action == FirstPersonModeAction && Mapping.Key == EKeys::F6;
					}));
			TestTrue(
				TEXT("The First Person Blueprint retains the R mapping"),
				FirstPersonMappingContext->GetMappings().ContainsByPredicate(
					[FirstPersonCollisionAction](const FEnhancedActionKeyMapping& Mapping)
					{
						return Mapping.Action == FirstPersonCollisionAction && Mapping.Key == EKeys::R;
					}));
			const FKey ExpectedFirstPersonScaleKeys[] = {EKeys::One, EKeys::Two, EKeys::Three, EKeys::Four, EKeys::Five};
			for (int32 ScaleActionIndex = 0; ScaleActionIndex < FirstPersonScaleActions.Num(); ++ScaleActionIndex)
			{
				TestTrue(
					*FString::Printf(TEXT("The First Person Blueprint retains Scale mapping %d"), ScaleActionIndex),
					FirstPersonMappingContext->GetMappings().ContainsByPredicate(
						[&FirstPersonScaleActions, &ExpectedFirstPersonScaleKeys, ScaleActionIndex](const FEnhancedActionKeyMapping& Mapping)
						{
							return Mapping.Action == FirstPersonScaleActions[ScaleActionIndex]
								&& Mapping.Key == ExpectedFirstPersonScaleKeys[ScaleActionIndex];
						}));
			}
			TestTrue(
				TEXT("The First Person Blueprint retains Scale Restore on T"),
				FirstPersonMappingContext->GetMappings().ContainsByPredicate(
					[FirstPersonScaleRestoreAction](const FEnhancedActionKeyMapping& Mapping)
					{
						return Mapping.Action == FirstPersonScaleRestoreAction && Mapping.Key == EKeys::T;
					}));
			TestTrue(
				TEXT("The First Person Blueprint retains Gravity Cycle on G"),
				FirstPersonMappingContext->GetMappings().ContainsByPredicate(
					[FirstPersonGravityCycleAction](const FEnhancedActionKeyMapping& Mapping)
					{
						return Mapping.Action == FirstPersonGravityCycleAction && Mapping.Key == EKeys::G;
					}));
			TestTrue(
				TEXT("The First Person Blueprint retains Gravity Restore on H"),
				FirstPersonMappingContext->GetMappings().ContainsByPredicate(
					[FirstPersonGravityRestoreAction](const FEnhancedActionKeyMapping& Mapping)
					{
						return Mapping.Action == FirstPersonGravityRestoreAction && Mapping.Key == EKeys::H;
					}));
		}
	}

	const FName WorldName = MakeUniqueObjectName(nullptr, UWorld::StaticClass(), NAME_None, EUniqueObjectNameOptions::GloballyUnique);
	FWorldContext& WorldContext = GEngine->CreateNewWorldContext(EWorldType::Game);
	UWorld* TestWorld = UWorld::CreateWorld(EWorldType::Game, false, WorldName, GetTransientPackage());
	if (!TestNotNull(TEXT("A Developer Mode test world can be created"), TestWorld))
	{
		GEngine->DestroyWorldContext(TestWorld);
		return false;
	}

	TestWorld->AddToRoot();
	WorldContext.SetCurrentWorld(TestWorld);
	TestWorld->InitializeActorsForPlay(FURL());

	AActor* PlayerActor = TestWorld->SpawnActor<AActor>(FVector::ZeroVector, FRotator::ZeroRotator);
	UDeveloperModeComponent* DeveloperComponent = NewObject<UDeveloperModeComponent>(PlayerActor, TEXT("DeveloperModeComponent"));
	PlayerActor->AddInstanceComponent(DeveloperComponent);
	DeveloperComponent->RegisterComponent();
	DeveloperComponent->InspectionDistance = 500.0f;
	TestFalse(TEXT("A runtime Developer Mode component starts inactive"), DeveloperComponent->IsDeveloperModeActive());
	TestFalse(TEXT("Developer scanning Tick is disabled while mode is inactive"), DeveloperComponent->IsComponentTickEnabled());
	TestFalse(TEXT("Collision invocation does nothing while Developer Mode is inactive"), DeveloperComponent->ToggleFocusedCollisionModification());
	TestFalse(TEXT("Scale invocation does nothing while Developer Mode is inactive"), DeveloperComponent->ApplyFocusedScaleModification(ERealityScalePreset::Half));
	TestFalse(TEXT("Scale Restore does nothing while Developer Mode is inactive"), DeveloperComponent->RestoreFocusedScaleModification());
	TestFalse(TEXT("Gravity Cycle does nothing while Developer Mode is inactive"), DeveloperComponent->CycleFocusedGravityModification());
	TestFalse(TEXT("Gravity Restore does nothing while Developer Mode is inactive"), DeveloperComponent->RestoreFocusedGravityModification());

	ARealityEditableTestActor* EditableActorA = SpawnEditable(TestWorld, FVector(150.0f, 0.0f, 0.0f));
	DeveloperComponent->OnDeveloperFocusGained.AddDynamic(EditableActorA, &ARealityEditableTestActor::HandleDeveloperFocusGained);
	DeveloperComponent->OnDeveloperFocusLost.AddDynamic(EditableActorA, &ARealityEditableTestActor::HandleDeveloperFocusLost);
	EditableActorA->EditableComponent->OnRealityCheatEvent.AddDynamic(EditableActorA, &ARealityEditableTestActor::HandleRealityCheatEvent);
	TestFalse(TEXT("Editable-only developer targets need not implement IInteractable"), EditableActorA->Implements<UInteractable>());

	TestTrue(TEXT("Toggle activates Developer Mode"), DeveloperComponent->ToggleDeveloperMode());
	TestFalse(TEXT("Console selection freezes after its one opening trace"), DeveloperComponent->IsComponentTickEnabled());
	TestEqual(TEXT("An editable-only Actor gains developer focus"), DeveloperComponent->GetFocusedDeveloperActor(), static_cast<AActor*>(EditableActorA));
	TestEqual(TEXT("Developer focus exposes the editable component"), DeveloperComponent->GetFocusedEditableComponent(), EditableActorA->EditableComponent.Get());
	TestEqual(TEXT("Focus gained broadcasts exactly once"), EditableActorA->DeveloperFocusGainedCount, 1);

	TestTrue(TEXT("Focused Collision prototype invokes Apply"), DeveloperComponent->ToggleFocusedCollisionModification());
	TestTrue(TEXT("Focused Collision is now modified"), EditableActorA->EditableComponent->IsCollisionModified());
	TestEqual(TEXT("Developer Apply emits one cheat event"), EditableActorA->CheatEventCount, 1);
	TestEqual(TEXT("Developer Apply uses the player-side owner as instigator"), EditableActorA->LastCheatEvent.InstigatingActor.Get(), PlayerActor);
	DeveloperComponent->UpdateDeveloperFocus();
	TestEqual(TEXT("A collision-disabled target remains inspectable through its visual bounds"), DeveloperComponent->GetFocusedDeveloperActor(), static_cast<AActor*>(EditableActorA));
	AActor* OccludingActor = TestWorld->SpawnActor<AActor>(FVector(75.0f, 0.0f, 0.0f), FRotator::ZeroRotator);
	UBoxComponent* OccludingBox = NewObject<UBoxComponent>(OccludingActor, TEXT("OccludingBox"));
	OccludingActor->SetRootComponent(OccludingBox);
	OccludingActor->AddInstanceComponent(OccludingBox);
	OccludingBox->SetBoxExtent(FVector(20.0f));
	OccludingBox->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);
	OccludingBox->RegisterComponent();
	OccludingBox->RecreatePhysicsState();
	DeveloperComponent->UpdateDeveloperFocus();
	TestNull(TEXT("A nearer blocking Actor occludes a collision-disabled developer target"), DeveloperComponent->GetFocusedDeveloperActor());
	OccludingActor->SetActorLocation(FVector(75.0f, 200.0f, 0.0f));
	DeveloperComponent->UpdateDeveloperFocus();
	TestEqual(TEXT("Removing the occluder reacquires the modified target"), DeveloperComponent->GetFocusedDeveloperActor(), static_cast<AActor*>(EditableActorA));
	TestTrue(TEXT("Second prototype invocation restores Collision"), DeveloperComponent->ToggleFocusedCollisionModification());
	TestFalse(TEXT("Focused Collision is restored"), EditableActorA->EditableComponent->IsCollisionModified());
	TestEqual(TEXT("Developer Restore emits one additional event"), EditableActorA->CheatEventCount, 2);
	TestEqual(TEXT("Developer Restore retains the player instigator"), EditableActorA->LastCheatEvent.InstigatingActor.Get(), PlayerActor);
	TestTrue(TEXT("Focused Scale prototype invokes Apply"), DeveloperComponent->ApplyFocusedScaleModification(ERealityScalePreset::Half));
	TestEqual(TEXT("Developer Scale Apply changes the focused Actor"), EditableActorA->GetActorScale3D(), FVector(0.5f));
	TestEqual(TEXT("Developer Scale Apply uses the player-side owner as instigator"), EditableActorA->LastCheatEvent.InstigatingActor.Get(), PlayerActor);
	TestEqual(TEXT("Developer Scale Apply uses Cheat.Scale"), EditableActorA->LastCheatEvent.CheatTag, GetScaleTag());
	TestTrue(TEXT("Focused Scale prototype invokes Restore"), DeveloperComponent->RestoreFocusedScaleModification());
	TestEqual(TEXT("Developer Scale Restore returns the focused Actor to baseline"), EditableActorA->GetActorScale3D(), FVector::OneVector);
	FGameplayTagContainer AllPrototypeCheats = EditableActorA->EditableComponent->GetSupportedCheats();
	AllPrototypeCheats.AddTag(GetGravityTag());
	EditableActorA->EditableComponent->SetSupportedCheats(AllPrototypeCheats);
	EditableActorA->PrimitiveA->SetCollisionProfileName(UCollisionProfile::PhysicsActor_ProfileName);
	EditableActorA->PrimitiveA->SetSimulatePhysics(true);
	EditableActorA->PrimitiveA->SetEnableGravity(true);
	TestTrue(TEXT("Focused Gravity Cycle invokes Low"), DeveloperComponent->CycleFocusedGravityModification());
	TestEqual(TEXT("First Gravity cycle step is Low"), EditableActorA->EditableComponent->GetCurrentGravityPreset(), ERealityGravityPreset::Low);
	TestTrue(TEXT("Developer Low Gravity enables its local Tick"), EditableActorA->EditableComponent->IsComponentTickEnabled());
	TestEqual(TEXT("Developer Gravity Apply uses the player-side owner as instigator"), EditableActorA->LastCheatEvent.InstigatingActor.Get(), PlayerActor);
	TestEqual(TEXT("Developer Gravity Apply uses Cheat.Gravity"), EditableActorA->LastCheatEvent.CheatTag, GetGravityTag());
	TestTrue(TEXT("Second Gravity cycle step invokes Zero"), DeveloperComponent->CycleFocusedGravityModification());
	TestEqual(TEXT("Second Gravity cycle step is Zero"), EditableActorA->EditableComponent->GetCurrentGravityPreset(), ERealityGravityPreset::Zero);
	TestTrue(TEXT("Console-style direct Gravity Normal succeeds"), DeveloperComponent->ApplyFocusedGravityModification(ERealityGravityPreset::Normal));
	TestEqual(TEXT("Direct Gravity preset reaches the editable implementation"), EditableActorA->EditableComponent->GetCurrentGravityPreset(), ERealityGravityPreset::Normal);
	TestTrue(TEXT("Console-style direct Gravity Zero succeeds"), DeveloperComponent->ApplyFocusedGravityModification(ERealityGravityPreset::Zero));
	TestFalse(TEXT("Developer Zero Gravity disables the Low Tick"), EditableActorA->EditableComponent->IsComponentTickEnabled());
	TestTrue(TEXT("Third Gravity cycle step invokes active Normal"), DeveloperComponent->CycleFocusedGravityModification());
	TestEqual(TEXT("Third Gravity cycle step is Normal"), EditableActorA->EditableComponent->GetCurrentGravityPreset(), ERealityGravityPreset::Normal);
	TestTrue(TEXT("Focused Gravity prototype invokes Restore"), DeveloperComponent->RestoreFocusedGravityModification());
	TestFalse(TEXT("Developer Gravity Restore ends the cycle"), EditableActorA->EditableComponent->IsGravityModified());
	EditableActorA->PrimitiveA->SetSimulatePhysics(false);

	// Multiple simultaneously modified targets must remain independently reacquirable and restorable.
	ARealityEditableTestActor* EditableActorMultiB = SpawnEditable(TestWorld, FVector(150.0f, 250.0f, 0.0f));
	EditableActorMultiB->EditableComponent->OnRealityCheatEvent.AddDynamic(EditableActorMultiB, &ARealityEditableTestActor::HandleRealityCheatEvent);
	DeveloperComponent->UpdateDeveloperFocus();
	TestEqual(TEXT("Multi-target Actor A can be focused"), DeveloperComponent->GetFocusedDeveloperActor(), static_cast<AActor*>(EditableActorA));
	TestTrue(TEXT("Multi-target Actor A can be collision-modified"), DeveloperComponent->ToggleFocusedCollisionModification());
	EditableActorA->SetActorLocation(FVector(150.0f, 250.0f, 0.0f));
	EditableActorMultiB->SetActorLocation(FVector(150.0f, 0.0f, 0.0f));
	DeveloperComponent->UpdateDeveloperFocus();
	TestEqual(TEXT("Multi-target Actor B can be focused"), DeveloperComponent->GetFocusedDeveloperActor(), static_cast<AActor*>(EditableActorMultiB));
	TestTrue(TEXT("Multi-target Actor B can be collision-modified"), DeveloperComponent->ToggleFocusedCollisionModification());
	TestTrue(TEXT("Actor A remains modified while Actor B is modified"), EditableActorA->EditableComponent->IsCollisionModified());
	TestTrue(TEXT("Actor B is independently modified"), EditableActorMultiB->EditableComponent->IsCollisionModified());
	EditableActorMultiB->SetActorLocation(FVector(150.0f, 250.0f, 0.0f));
	EditableActorA->SetActorLocation(FVector(150.0f, 0.0f, 0.0f));
	DeveloperComponent->UpdateDeveloperFocus();
	TestEqual(TEXT("Modified Actor A is independently reacquired"), DeveloperComponent->GetFocusedDeveloperActor(), static_cast<AActor*>(EditableActorA));
	TestTrue(TEXT("Modified Actor A restores independently"), DeveloperComponent->ToggleFocusedCollisionModification());
	EditableActorA->SetActorLocation(FVector(150.0f, 250.0f, 0.0f));
	EditableActorMultiB->SetActorLocation(FVector(150.0f, 0.0f, 0.0f));
	DeveloperComponent->UpdateDeveloperFocus();
	TestEqual(TEXT("Modified Actor B is independently reacquired"), DeveloperComponent->GetFocusedDeveloperActor(), static_cast<AActor*>(EditableActorMultiB));
	TestTrue(TEXT("Modified Actor B restores independently"), DeveloperComponent->ToggleFocusedCollisionModification());
	TestFalse(TEXT("Actor A finishes restored"), EditableActorA->EditableComponent->IsCollisionModified());
	TestFalse(TEXT("Actor B finishes restored"), EditableActorMultiB->EditableComponent->IsCollisionModified());
	EditableActorMultiB->SetActorLocation(FVector(150.0f, 250.0f, 0.0f));

	EditableActorA->SetActorLocation(FVector(150.0f, 150.0f, 0.0f));
	DeveloperComponent->UpdateDeveloperFocus();
	TestNull(TEXT("Looking away clears developer focus"), DeveloperComponent->GetFocusedDeveloperActor());
	TestEqual(TEXT("Looking away adds one focus-lost transition"), EditableActorA->DeveloperFocusLostCount, 3);

	AActor* NonEditableActor = TestWorld->SpawnActor<AActor>(FVector(100.0f, 0.0f, 0.0f), FRotator::ZeroRotator);
	UBoxComponent* NonEditableBox = NewObject<UBoxComponent>(NonEditableActor, TEXT("NonEditableBox"));
	NonEditableActor->SetRootComponent(NonEditableBox);
	NonEditableActor->AddInstanceComponent(NonEditableBox);
	NonEditableBox->SetBoxExtent(FVector(25.0f));
	NonEditableBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	NonEditableBox->SetCollisionResponseToAllChannels(ECR_Block);
	NonEditableBox->RegisterComponent();
	DeveloperComponent->UpdateDeveloperFocus();
	TestNull(TEXT("A non-editable Actor is ignored"), DeveloperComponent->GetFocusedDeveloperActor());
	NonEditableActor->SetActorLocation(FVector(100.0f, 200.0f, 0.0f));

	ARealityEditableTestActor* EditableActorB = SpawnEditable(TestWorld, FVector(180.0f, 0.0f, 0.0f), false);
	DeveloperComponent->OnDeveloperFocusGained.AddDynamic(EditableActorB, &ARealityEditableTestActor::HandleDeveloperFocusGained);
	DeveloperComponent->OnDeveloperFocusLost.AddDynamic(EditableActorB, &ARealityEditableTestActor::HandleDeveloperFocusLost);
	DeveloperComponent->UpdateDeveloperFocus();
	TestEqual(TEXT("A second editable Actor gains focus"), DeveloperComponent->GetFocusedDeveloperActor(), static_cast<AActor*>(EditableActorB));
	TestEqual(TEXT("Actor B receives focus gained"), EditableActorB->DeveloperFocusGainedCount, 1);
	EditableActorA->SetActorLocation(FVector(120.0f, 0.0f, 0.0f));
	DeveloperComponent->UpdateDeveloperFocus();
	TestEqual(TEXT("Focus switches from Actor B to Actor A"), DeveloperComponent->GetFocusedDeveloperActor(), static_cast<AActor*>(EditableActorA));
	TestEqual(TEXT("Switching emits focus lost for Actor B"), EditableActorB->DeveloperFocusLostCount, 1);
	TestEqual(TEXT("Switching emits a new focus gained for Actor A"), EditableActorA->DeveloperFocusGainedCount, 4);

	EditableActorA->Destroy();
	TestNull(TEXT("Destroying the target clears weak developer focus"), DeveloperComponent->GetFocusedDeveloperActor());
	TestEqual(TEXT("Target destruction broadcasts focus lost"), EditableActorA->DeveloperFocusLostCount, 4);

	EditableActorB->SetActorLocation(FVector(160.0f, 0.0f, 0.0f));
	DeveloperComponent->UpdateDeveloperFocus();
	TestEqual(TEXT("Actor B can regain focus"), DeveloperComponent->GetFocusedDeveloperActor(), static_cast<AActor*>(EditableActorB));
	TestFalse(TEXT("Unsupported focused Collision fails safely"), DeveloperComponent->ToggleFocusedCollisionModification());
	TestFalse(TEXT("Unsupported focused Scale fails safely"), DeveloperComponent->ApplyFocusedScaleModification(ERealityScalePreset::Half));
	TestFalse(TEXT("Unsupported focused Scale Restore fails safely"), DeveloperComponent->RestoreFocusedScaleModification());
	TestFalse(TEXT("Unsupported focused Gravity Cycle fails safely"), DeveloperComponent->CycleFocusedGravityModification());
	TestFalse(TEXT("Unsupported focused Gravity Restore fails safely"), DeveloperComponent->RestoreFocusedGravityModification());

	TestFalse(TEXT("Second mode toggle deactivates Developer Mode"), DeveloperComponent->ToggleDeveloperMode());
	TestFalse(TEXT("Developer scanning Tick disables again"), DeveloperComponent->IsComponentTickEnabled());
	TestNull(TEXT("Exiting Developer Mode clears focus"), DeveloperComponent->GetFocusedDeveloperActor());
	TestFalse(TEXT("Collision invocation remains disabled after mode exit"), DeveloperComponent->ToggleFocusedCollisionModification());
	TestFalse(TEXT("Scale invocation remains disabled after mode exit"), DeveloperComponent->ApplyFocusedScaleModification(ERealityScalePreset::Half));
	TestFalse(TEXT("Scale Restore remains disabled after mode exit"), DeveloperComponent->RestoreFocusedScaleModification());
	TestFalse(TEXT("Gravity Cycle remains disabled after mode exit"), DeveloperComponent->CycleFocusedGravityModification());
	TestFalse(TEXT("Gravity Restore remains disabled after mode exit"), DeveloperComponent->RestoreFocusedGravityModification());

	TestWorld->DestroyWorld(false);
	TestWorld->SetPhysicsScene(nullptr);
	GEngine->DestroyWorldContext(TestWorld);
	TestWorld->RemoveFromRoot();
	return true;
}

#endif
