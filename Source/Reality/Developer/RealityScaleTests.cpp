// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"

#include "Components/BoxComponent.h"
#include "Developer/RealityEditableComponent.h"
#include "Developer/RealityEditableTestActor.h"
#include "Engine/Engine.h"
#include "Engine/World.h"

namespace RealityScaleTests
{
	FGameplayTag GetScaleTag()
	{
		return FGameplayTag::RequestGameplayTag(TEXT("Cheat.Scale"));
	}

	FGameplayTag GetCollisionTag()
	{
		return FGameplayTag::RequestGameplayTag(TEXT("Cheat.Collision"));
	}

	void SetCapabilities(URealityEditableComponent* EditableComponent, const bool bScale, const bool bCollision = false)
	{
		FGameplayTagContainer SupportedCheats;
		if (bScale)
		{
			SupportedCheats.AddTag(GetScaleTag());
		}
		if (bCollision)
		{
			SupportedCheats.AddTag(GetCollisionTag());
		}
		EditableComponent->SetSupportedCheats(SupportedCheats);
	}

	ARealityEditableTestActor* CreateTestActor(UWorld* World, const FVector& Scale, const bool bCollision = false)
	{
		ARealityEditableTestActor* TestActor = World->SpawnActor<ARealityEditableTestActor>();
		TestActor->SetActorScale3D(Scale);
		SetCapabilities(TestActor->EditableComponent, true, bCollision);
		TestActor->EditableComponent->OnRealityCheatEvent.AddDynamic(TestActor, &ARealityEditableTestActor::HandleRealityCheatEvent);
		return TestActor;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRealityScaleModificationTest,
	"Reality.Developer.ScaleModification",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::EngineFilter)

bool FRealityScaleModificationTest::RunTest(const FString& Parameters)
{
	using namespace RealityScaleTests;

	const FName WorldName = MakeUniqueObjectName(nullptr, UWorld::StaticClass(), NAME_None, EUniqueObjectNameOptions::GloballyUnique);
	FWorldContext& WorldContext = GEngine->CreateNewWorldContext(EWorldType::Game);
	UWorld* TestWorld = UWorld::CreateWorld(EWorldType::Game, false, WorldName, GetTransientPackage());
	if (!TestNotNull(TEXT("A Scale test world can be created"), TestWorld))
	{
		GEngine->DestroyWorldContext(TestWorld);
		return false;
	}

	TestWorld->AddToRoot();
	WorldContext.SetCurrentWorld(TestWorld);
	TestWorld->InitializeActorsForPlay(FURL());

	AActor* InstigatingActor = TestWorld->SpawnActor<AActor>();
	const FVector OriginalScale(1.0f, 2.0f, 0.5f);
	ARealityEditableTestActor* TestActor = CreateTestActor(TestWorld, OriginalScale, true);

	TestFalse(TEXT("Scale starts unmodified"), TestActor->EditableComponent->IsScaleModified());
	TestEqual(TEXT("Unmodified original scale reports the legitimate Actor scale"), TestActor->EditableComponent->GetOriginalScale(), OriginalScale);
	TestFalse(TEXT("Applying 1.0x to an unmodified Actor is rejected as a no-op"), TestActor->EditableComponent->ApplyScaleModification(ERealityScalePreset::One, InstigatingActor));
	TestEqual(TEXT("A rejected initial 1.0x emits no event"), TestActor->CheatEventCount, 0);

	TestTrue(TEXT("Supported 0.5x Scale Apply succeeds"), TestActor->EditableComponent->ApplyScaleModification(ERealityScalePreset::Half, InstigatingActor));
	TestEqual(TEXT("0.5x uses the non-uniform original scale"), TestActor->GetActorScale3D(), OriginalScale * 0.5f);
	TestTrue(TEXT("Scale reports modified after Apply"), TestActor->EditableComponent->IsScaleModified());
	TestEqual(TEXT("The exact original scale is captured"), TestActor->EditableComponent->GetOriginalScale(), OriginalScale);
	TestEqual(TEXT("The current Scale preset is 0.5x"), TestActor->EditableComponent->GetCurrentScalePreset(), ERealityScalePreset::Half);
	TestEqual(TEXT("Scale Apply emits one event"), TestActor->CheatEventCount, 1);
	TestEqual(TEXT("Scale Apply event target is correct"), TestActor->LastCheatEvent.TargetActor.Get(), static_cast<AActor*>(TestActor));
	TestEqual(TEXT("Scale Apply event instigator is correct"), TestActor->LastCheatEvent.InstigatingActor.Get(), InstigatingActor);
	TestEqual(TEXT("Scale Apply event uses Cheat.Scale"), TestActor->LastCheatEvent.CheatTag, GetScaleTag());
	TestEqual(TEXT("Scale Apply event identifies Apply"), TestActor->LastCheatEvent.Operation, ERealityCheatOperation::Apply);

	TestFalse(TEXT("The same active preset is rejected"), TestActor->EditableComponent->ApplyScaleModification(ERealityScalePreset::Half, InstigatingActor));
	TestEqual(TEXT("The same active preset emits no duplicate event"), TestActor->CheatEventCount, 1);
	TestEqual(TEXT("A rejected duplicate leaves scale unchanged"), TestActor->GetActorScale3D(), OriginalScale * 0.5f);

	TestTrue(TEXT("Switching to 2.0x succeeds"), TestActor->EditableComponent->ApplyScaleModification(ERealityScalePreset::Double, InstigatingActor));
	TestEqual(TEXT("2.0x is calculated from the original, not compounded"), TestActor->GetActorScale3D(), OriginalScale * 2.0f);
	TestTrue(TEXT("Switching to 4.0x succeeds"), TestActor->EditableComponent->ApplyScaleModification(ERealityScalePreset::Quadruple, InstigatingActor));
	TestEqual(TEXT("4.0x is calculated from the original, not compounded"), TestActor->GetActorScale3D(), OriginalScale * 4.0f);
	TestEqual(TEXT("Preset switches each emit one Apply event"), TestActor->CheatEventCount, 3);

	TestTrue(TEXT("Switching an active cycle to 1.0x succeeds"), TestActor->EditableComponent->ApplyScaleModification(ERealityScalePreset::One, InstigatingActor));
	TestEqual(TEXT("Active 1.0x returns visually to the original scale"), TestActor->GetActorScale3D(), OriginalScale);
	TestTrue(TEXT("The 1.0x preset keeps the Scale cycle active"), TestActor->EditableComponent->IsScaleModified());
	TestEqual(TEXT("The active preset reports 1.0x"), TestActor->EditableComponent->GetCurrentScalePreset(), ERealityScalePreset::One);
	TestFalse(TEXT("Repeating active 1.0x is rejected"), TestActor->EditableComponent->ApplyScaleModification(ERealityScalePreset::One, InstigatingActor));
	TestEqual(TEXT("Repeated active 1.0x emits no duplicate event"), TestActor->CheatEventCount, 4);

	TestTrue(TEXT("Explicit Scale Restore succeeds from active 1.0x"), TestActor->EditableComponent->RestoreScaleModification(InstigatingActor));
	TestEqual(TEXT("Restore returns the exact non-uniform original scale"), TestActor->GetActorScale3D(), OriginalScale);
	TestFalse(TEXT("Scale reports unmodified after Restore"), TestActor->EditableComponent->IsScaleModified());
	TestEqual(TEXT("Restore resets the public preset to 1.0x"), TestActor->EditableComponent->GetCurrentScalePreset(), ERealityScalePreset::One);
	TestEqual(TEXT("Scale Restore emits one event"), TestActor->CheatEventCount, 5);
	TestEqual(TEXT("Scale Restore event uses Cheat.Scale"), TestActor->LastCheatEvent.CheatTag, GetScaleTag());
	TestEqual(TEXT("Scale Restore event identifies Restore"), TestActor->LastCheatEvent.Operation, ERealityCheatOperation::Restore);
	TestFalse(TEXT("Repeated Scale Restore fails safely"), TestActor->EditableComponent->RestoreScaleModification(InstigatingActor));
	TestEqual(TEXT("Repeated Restore emits no event"), TestActor->CheatEventCount, 5);

	const FVector NewLegitimateScale(3.0f, 1.5f, 0.75f);
	TestActor->SetActorScale3D(NewLegitimateScale);
	TestTrue(TEXT("A second Scale cycle captures a new legitimate baseline"), TestActor->EditableComponent->ApplyScaleModification(ERealityScalePreset::Quarter, nullptr));
	TestEqual(TEXT("The second cycle applies against the new baseline"), TestActor->GetActorScale3D(), NewLegitimateScale * 0.25f);
	TestEqual(TEXT("The second cycle exposes the new baseline"), TestActor->EditableComponent->GetOriginalScale(), NewLegitimateScale);
	TestTrue(TEXT("The second Scale cycle restores"), TestActor->EditableComponent->RestoreScaleModification(nullptr));
	TestEqual(TEXT("The second cycle restores its exact new baseline"), TestActor->GetActorScale3D(), NewLegitimateScale);
	TestNull(TEXT("A null instigator is retained safely"), TestActor->LastCheatEvent.InstigatingActor.Get());

	TestActor->PrimitiveA->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	TestActor->PrimitiveB->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TestTrue(TEXT("Collision can be modified before Scale"), TestActor->EditableComponent->ApplyCollisionModification(InstigatingActor));
	TestTrue(TEXT("Scale can be modified while Collision is active"), TestActor->EditableComponent->ApplyScaleModification(ERealityScalePreset::Half, InstigatingActor));
	TestTrue(TEXT("Restoring Collision succeeds independently"), TestActor->EditableComponent->RestoreCollisionModification(InstigatingActor));
	TestTrue(TEXT("Restoring Collision leaves Scale active"), TestActor->EditableComponent->IsScaleModified());
	TestEqual(TEXT("Restoring Collision leaves scaled transform intact"), TestActor->GetActorScale3D(), NewLegitimateScale * 0.5f);
	TestTrue(TEXT("Scale restores after Collision"), TestActor->EditableComponent->RestoreScaleModification(InstigatingActor));

	TestTrue(TEXT("Scale can be modified before Collision"), TestActor->EditableComponent->ApplyScaleModification(ERealityScalePreset::Double, InstigatingActor));
	TestTrue(TEXT("Collision can be modified while Scale is active"), TestActor->EditableComponent->ApplyCollisionModification(InstigatingActor));
	TestTrue(TEXT("Restoring Scale succeeds independently"), TestActor->EditableComponent->RestoreScaleModification(InstigatingActor));
	TestTrue(TEXT("Restoring Scale leaves Collision active"), TestActor->EditableComponent->IsCollisionModified());
	TestTrue(TEXT("Collision restores after Scale"), TestActor->EditableComponent->RestoreCollisionModification(InstigatingActor));

	ARealityEditableTestActor* UnsupportedActor = TestWorld->SpawnActor<ARealityEditableTestActor>();
	UnsupportedActor->SetActorScale3D(FVector(1.5f));
	UnsupportedActor->EditableComponent->OnRealityCheatEvent.AddDynamic(UnsupportedActor, &ARealityEditableTestActor::HandleRealityCheatEvent);
	TestFalse(TEXT("Unsupported Scale Apply fails"), UnsupportedActor->EditableComponent->ApplyScaleModification(ERealityScalePreset::Half, InstigatingActor));
	TestEqual(TEXT("Unsupported Scale Apply leaves scale unchanged"), UnsupportedActor->GetActorScale3D(), FVector(1.5f));
	TestEqual(TEXT("Unsupported Scale Apply emits no event"), UnsupportedActor->CheatEventCount, 0);

	TestFalse(TEXT("An invalid Scale preset fails safely"), TestActor->EditableComponent->ApplyScaleModification(static_cast<ERealityScalePreset>(255), InstigatingActor));
	URealityEditableComponent* OwnerlessComponent = NewObject<URealityEditableComponent>();
	SetCapabilities(OwnerlessComponent, true);
	TestFalse(TEXT("An ownerless component fails Scale Apply safely"), OwnerlessComponent->ApplyScaleModification(ERealityScalePreset::Half, InstigatingActor));
	TestFalse(TEXT("An ownerless component fails Scale Restore safely"), OwnerlessComponent->RestoreScaleModification(InstigatingActor));

	ARealityEditableTestActor* ActorA = CreateTestActor(TestWorld, FVector(2.0f));
	ARealityEditableTestActor* ActorB = CreateTestActor(TestWorld, FVector(0.75f, 1.0f, 1.25f));
	TestTrue(TEXT("Actor A accepts an independent 0.5x Scale"), ActorA->EditableComponent->ApplyScaleModification(ERealityScalePreset::Half, InstigatingActor));
	TestTrue(TEXT("Actor B accepts an independent 4.0x Scale"), ActorB->EditableComponent->ApplyScaleModification(ERealityScalePreset::Quadruple, InstigatingActor));
	TestEqual(TEXT("Actor A retains its own scaled value"), ActorA->GetActorScale3D(), FVector(1.0f));
	TestEqual(TEXT("Actor B retains its own scaled value"), ActorB->GetActorScale3D(), FVector(3.0f, 4.0f, 5.0f));
	TestTrue(TEXT("Actor A restores independently"), ActorA->EditableComponent->RestoreScaleModification(InstigatingActor));
	TestTrue(TEXT("Restoring Actor A leaves Actor B modified"), ActorB->EditableComponent->IsScaleModified());
	TestEqual(TEXT("Restoring Actor A leaves Actor B scale unchanged"), ActorB->GetActorScale3D(), FVector(3.0f, 4.0f, 5.0f));
	TestTrue(TEXT("Actor B restores independently"), ActorB->EditableComponent->RestoreScaleModification(InstigatingActor));

	TestWorld->DestroyWorld(false);
	TestWorld->SetPhysicsScene(nullptr);
	GEngine->DestroyWorldContext(TestWorld);
	TestWorld->RemoveFromRoot();
	return true;
}

#endif
