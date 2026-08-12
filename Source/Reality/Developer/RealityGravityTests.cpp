// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"

#include "Components/BoxComponent.h"
#include "Developer/RealityEditableComponent.h"
#include "Developer/RealityEditableTestActor.h"
#include "Engine/CollisionProfile.h"
#include "Engine/Engine.h"
#include "Engine/World.h"

namespace RealityGravityTests
{
	FGameplayTag GetGravityTag()
	{
		return FGameplayTag::RequestGameplayTag(TEXT("Cheat.Gravity"));
	}

	FGameplayTag GetCollisionTag()
	{
		return FGameplayTag::RequestGameplayTag(TEXT("Cheat.Collision"));
	}

	FGameplayTag GetScaleTag()
	{
		return FGameplayTag::RequestGameplayTag(TEXT("Cheat.Scale"));
	}

	void SetCapabilities(URealityEditableComponent* EditableComponent, const bool bGravity, const bool bCollision = false, const bool bScale = false)
	{
		FGameplayTagContainer SupportedCheats;
		if (bGravity)
		{
			SupportedCheats.AddTag(GetGravityTag());
		}
		if (bCollision)
		{
			SupportedCheats.AddTag(GetCollisionTag());
		}
		if (bScale)
		{
			SupportedCheats.AddTag(GetScaleTag());
		}
		EditableComponent->SetSupportedCheats(SupportedCheats);
	}

	void ConfigureSimulatedPrimitive(UBoxComponent* PrimitiveComponent, const bool bGravityEnabled)
	{
		PrimitiveComponent->SetCollisionProfileName(UCollisionProfile::PhysicsActor_ProfileName);
		PrimitiveComponent->SetSimulatePhysics(true);
		PrimitiveComponent->SetEnableGravity(bGravityEnabled);
	}

	ARealityEditableTestActor* CreateGravityActor(UWorld* World, const bool bMixedGravity = false)
	{
		ARealityEditableTestActor* TestActor = World->SpawnActor<ARealityEditableTestActor>();
		SetCapabilities(TestActor->EditableComponent, true);
		ConfigureSimulatedPrimitive(TestActor->PrimitiveA, true);
		ConfigureSimulatedPrimitive(TestActor->PrimitiveB, !bMixedGravity);
		TestActor->EditableComponent->OnRealityCheatEvent.AddDynamic(TestActor, &ARealityEditableTestActor::HandleRealityCheatEvent);
		return TestActor;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRealityGravityModificationTest,
	"Reality.Developer.GravityModification",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::EngineFilter)

bool FRealityGravityModificationTest::RunTest(const FString& Parameters)
{
	using namespace RealityGravityTests;

	const FName WorldName = MakeUniqueObjectName(nullptr, UWorld::StaticClass(), NAME_None, EUniqueObjectNameOptions::GloballyUnique);
	FWorldContext& WorldContext = GEngine->CreateNewWorldContext(EWorldType::Game);
	UWorld* TestWorld = UWorld::CreateWorld(EWorldType::Game, false, WorldName, GetTransientPackage());
	if (!TestNotNull(TEXT("A Gravity test world can be created"), TestWorld))
	{
		GEngine->DestroyWorldContext(TestWorld);
		return false;
	}

	TestWorld->AddToRoot();
	WorldContext.SetCurrentWorld(TestWorld);
	TestWorld->InitializeActorsForPlay(FURL());

	AActor* InstigatingActor = TestWorld->SpawnActor<AActor>();

	ARealityEditableTestActor* UnsupportedActor = TestWorld->SpawnActor<ARealityEditableTestActor>();
	ConfigureSimulatedPrimitive(UnsupportedActor->PrimitiveA, true);
	UnsupportedActor->EditableComponent->OnRealityCheatEvent.AddDynamic(UnsupportedActor, &ARealityEditableTestActor::HandleRealityCheatEvent);
	TestFalse(TEXT("Unsupported Gravity Apply fails"), UnsupportedActor->EditableComponent->ApplyGravityModification(ERealityGravityPreset::Zero, InstigatingActor));
	TestTrue(TEXT("Unsupported Gravity Apply leaves gravity enabled"), UnsupportedActor->PrimitiveA->IsGravityEnabled());
	TestEqual(TEXT("Unsupported Gravity Apply emits no event"), UnsupportedActor->CheatEventCount, 0);

	ARealityEditableTestActor* NonSimulatedActor = TestWorld->SpawnActor<ARealityEditableTestActor>();
	SetCapabilities(NonSimulatedActor->EditableComponent, true);
	NonSimulatedActor->PrimitiveA->SetSimulatePhysics(false);
	NonSimulatedActor->PrimitiveB->SetSimulatePhysics(false);
	TestFalse(TEXT("An Actor without simulated primitives rejects Gravity Apply"), NonSimulatedActor->EditableComponent->ApplyGravityModification(ERealityGravityPreset::Zero, InstigatingActor));
	TestFalse(TEXT("Gravity Apply does not enable simulation on Primitive A"), NonSimulatedActor->PrimitiveA->IsAnySimulatingPhysics());
	TestFalse(TEXT("Gravity Apply does not enable simulation on Primitive B"), NonSimulatedActor->PrimitiveB->IsAnySimulatingPhysics());
	TestEqual(TEXT("An Actor without simulated primitives reports zero eligible components"), NonSimulatedActor->EditableComponent->GetEligibleGravityComponentCount(), 0);

	ARealityEditableTestActor* TestActor = CreateGravityActor(TestWorld, true);
	TestEqual(TEXT("Both directly owned simulated primitives are eligible"), TestActor->EditableComponent->GetEligibleGravityComponentCount(), 2);
	TestFalse(TEXT("Gravity starts unmodified"), TestActor->EditableComponent->IsGravityModified());
	TestFalse(TEXT("Gravity Tick starts disabled"), TestActor->EditableComponent->IsComponentTickEnabled());
	TestFalse(TEXT("Normal on an unmodified Actor is a no-op"), TestActor->EditableComponent->ApplyGravityModification(ERealityGravityPreset::Normal, InstigatingActor));
	TestEqual(TEXT("Initial Normal emits no event"), TestActor->CheatEventCount, 0);

	TestTrue(TEXT("Zero Gravity Apply succeeds"), TestActor->EditableComponent->ApplyGravityModification(ERealityGravityPreset::Zero, InstigatingActor));
	TestFalse(TEXT("Zero disables gravity on originally enabled Primitive A"), TestActor->PrimitiveA->IsGravityEnabled());
	TestFalse(TEXT("Zero leaves originally disabled Primitive B disabled"), TestActor->PrimitiveB->IsGravityEnabled());
	TestTrue(TEXT("Gravity reports modified after Zero"), TestActor->EditableComponent->IsGravityModified());
	TestEqual(TEXT("Current Gravity preset reports Zero"), TestActor->EditableComponent->GetCurrentGravityPreset(), ERealityGravityPreset::Zero);
	TestFalse(TEXT("Zero requires no Gravity Tick"), TestActor->EditableComponent->IsComponentTickEnabled());
	TestEqual(TEXT("Zero emits one Apply event"), TestActor->CheatEventCount, 1);
	TestEqual(TEXT("Gravity event target is correct"), TestActor->LastCheatEvent.TargetActor.Get(), static_cast<AActor*>(TestActor));
	TestEqual(TEXT("Gravity event instigator is correct"), TestActor->LastCheatEvent.InstigatingActor.Get(), InstigatingActor);
	TestEqual(TEXT("Gravity event uses Cheat.Gravity"), TestActor->LastCheatEvent.CheatTag, GetGravityTag());
	TestEqual(TEXT("Zero event identifies Apply"), TestActor->LastCheatEvent.Operation, ERealityCheatOperation::Apply);
	TestFalse(TEXT("Repeated Zero is rejected"), TestActor->EditableComponent->ApplyGravityModification(ERealityGravityPreset::Zero, InstigatingActor));
	TestEqual(TEXT("Repeated Zero emits no event"), TestActor->CheatEventCount, 1);

	TestTrue(TEXT("Switching Zero to Low succeeds"), TestActor->EditableComponent->ApplyGravityModification(ERealityGravityPreset::Low, InstigatingActor));
	TestEqual(TEXT("Current Gravity preset reports Low"), TestActor->EditableComponent->GetCurrentGravityPreset(), ERealityGravityPreset::Low);
	TestTrue(TEXT("Low Gravity enables local force Tick"), TestActor->EditableComponent->IsComponentTickEnabled());
	TestFalse(TEXT("Low keeps built-in gravity disabled on Primitive A"), TestActor->PrimitiveA->IsGravityEnabled());
	TestFalse(TEXT("Low keeps authored gravity disabled on Primitive B"), TestActor->PrimitiveB->IsGravityEnabled());
	TestFalse(TEXT("Repeated Low is rejected"), TestActor->EditableComponent->ApplyGravityModification(ERealityGravityPreset::Low, InstigatingActor));
	TestEqual(TEXT("Repeated Low emits no event"), TestActor->CheatEventCount, 2);

	TestTrue(TEXT("Switching Low to Normal succeeds"), TestActor->EditableComponent->ApplyGravityModification(ERealityGravityPreset::Normal, InstigatingActor));
	TestTrue(TEXT("Active Normal restores Primitive A baseline behavior"), TestActor->PrimitiveA->IsGravityEnabled());
	TestFalse(TEXT("Active Normal restores Primitive B baseline behavior"), TestActor->PrimitiveB->IsGravityEnabled());
	TestTrue(TEXT("Active Normal keeps the Gravity cycle active"), TestActor->EditableComponent->IsGravityModified());
	TestFalse(TEXT("Active Normal stops Low Gravity Tick"), TestActor->EditableComponent->IsComponentTickEnabled());

	TestTrue(TEXT("Normal can switch back to Low without recapturing baseline"), TestActor->EditableComponent->ApplyGravityModification(ERealityGravityPreset::Low, InstigatingActor));
	TestTrue(TEXT("Low Tick restarts only while Low is active"), TestActor->EditableComponent->IsComponentTickEnabled());
	TestTrue(TEXT("Low can switch to Zero"), TestActor->EditableComponent->ApplyGravityModification(ERealityGravityPreset::Zero, InstigatingActor));
	TestFalse(TEXT("Switching Low to Zero immediately stops Low Tick"), TestActor->EditableComponent->IsComponentTickEnabled());
	TestTrue(TEXT("Zero can switch back to Low"), TestActor->EditableComponent->ApplyGravityModification(ERealityGravityPreset::Low, InstigatingActor));
	TestTrue(TEXT("Low Tick restarts after Zero"), TestActor->EditableComponent->IsComponentTickEnabled());

	TestTrue(TEXT("Gravity Restore succeeds"), TestActor->EditableComponent->RestoreGravityModification(InstigatingActor));
	TestTrue(TEXT("Restore returns Primitive A to gravity enabled"), TestActor->PrimitiveA->IsGravityEnabled());
	TestFalse(TEXT("Restore returns Primitive B to gravity disabled"), TestActor->PrimitiveB->IsGravityEnabled());
	TestFalse(TEXT("Restore ends the Gravity cycle"), TestActor->EditableComponent->IsGravityModified());
	TestEqual(TEXT("Restore resets public preset to Normal"), TestActor->EditableComponent->GetCurrentGravityPreset(), ERealityGravityPreset::Normal);
	TestFalse(TEXT("Restore stops Low Gravity Tick"), TestActor->EditableComponent->IsComponentTickEnabled());
	TestEqual(TEXT("Restore event identifies Restore"), TestActor->LastCheatEvent.Operation, ERealityCheatOperation::Restore);
	TestFalse(TEXT("Repeated Gravity Restore fails safely"), TestActor->EditableComponent->RestoreGravityModification(InstigatingActor));

	TestActor->PrimitiveA->SetEnableGravity(false);
	TestActor->PrimitiveB->SetEnableGravity(true);
	TestTrue(TEXT("A later cycle captures new legitimate gravity states"), TestActor->EditableComponent->ApplyGravityModification(ERealityGravityPreset::Zero, nullptr));
	TestTrue(TEXT("The later cycle restores"), TestActor->EditableComponent->RestoreGravityModification(nullptr));
	TestFalse(TEXT("The later cycle restores new Primitive A baseline"), TestActor->PrimitiveA->IsGravityEnabled());
	TestTrue(TEXT("The later cycle restores new Primitive B baseline"), TestActor->PrimitiveB->IsGravityEnabled());
	TestNull(TEXT("A null instigator is retained safely"), TestActor->LastCheatEvent.InstigatingActor.Get());

	ARealityEditableTestActor* DestroyedPrimitiveActor = CreateGravityActor(TestWorld, false);
	TestTrue(TEXT("Gravity Apply succeeds before a target is destroyed"), DestroyedPrimitiveActor->EditableComponent->ApplyGravityModification(ERealityGravityPreset::Zero, InstigatingActor));
	DestroyedPrimitiveActor->PrimitiveB->DestroyComponent();
	TestTrue(TEXT("Restore skips a destroyed primitive and restores survivors"), DestroyedPrimitiveActor->EditableComponent->RestoreGravityModification(InstigatingActor));
	TestTrue(TEXT("Surviving primitive restores its gravity state"), DestroyedPrimitiveActor->PrimitiveA->IsGravityEnabled());

	ARealityEditableTestActor* AllDestroyedActor = CreateGravityActor(TestWorld, false);
	TestTrue(TEXT("Gravity Apply succeeds before every target is destroyed"), AllDestroyedActor->EditableComponent->ApplyGravityModification(ERealityGravityPreset::Low, InstigatingActor));
	const int32 EventCountBeforeAllTargetsAreDestroyed = AllDestroyedActor->CheatEventCount;
	AllDestroyedActor->PrimitiveB->DestroyComponent();
	AllDestroyedActor->PrimitiveA->DestroyComponent();
	TestFalse(TEXT("Restore fails safely when every captured primitive was destroyed"), AllDestroyedActor->EditableComponent->RestoreGravityModification(InstigatingActor));
	TestFalse(TEXT("All-invalid Restore clears the Gravity cycle"), AllDestroyedActor->EditableComponent->IsGravityModified());
	TestFalse(TEXT("All-invalid Restore leaves Tick disabled"), AllDestroyedActor->EditableComponent->IsComponentTickEnabled());
	TestEqual(TEXT("All-invalid Restore emits no false event"), AllDestroyedActor->CheatEventCount, EventCountBeforeAllTargetsAreDestroyed);

	ARealityEditableTestActor* ActorA = CreateGravityActor(TestWorld, false);
	ARealityEditableTestActor* ActorB = CreateGravityActor(TestWorld, false);
	TestTrue(TEXT("Actor A accepts independent Low Gravity"), ActorA->EditableComponent->ApplyGravityModification(ERealityGravityPreset::Low, InstigatingActor));
	TestTrue(TEXT("Actor B accepts independent Zero Gravity"), ActorB->EditableComponent->ApplyGravityModification(ERealityGravityPreset::Zero, InstigatingActor));
	TestTrue(TEXT("Actor A Low Gravity remains active"), ActorA->EditableComponent->IsComponentTickEnabled());
	TestFalse(TEXT("Actor B Zero Gravity needs no Tick"), ActorB->EditableComponent->IsComponentTickEnabled());
	TestTrue(TEXT("Actor A restores independently"), ActorA->EditableComponent->RestoreGravityModification(InstigatingActor));
	TestTrue(TEXT("Restoring Actor A leaves Actor B modified"), ActorB->EditableComponent->IsGravityModified());
	TestTrue(TEXT("Actor B restores independently"), ActorB->EditableComponent->RestoreGravityModification(InstigatingActor));

	ARealityEditableTestActor* MultiCheatActor = TestWorld->SpawnActor<ARealityEditableTestActor>();
	SetCapabilities(MultiCheatActor->EditableComponent, true, true, true);
	ConfigureSimulatedPrimitive(MultiCheatActor->PrimitiveA, true);
	ConfigureSimulatedPrimitive(MultiCheatActor->PrimitiveB, true);
	MultiCheatActor->PrimitiveA->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MultiCheatActor->PrimitiveB->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	TestTrue(TEXT("Collision can activate on the multi-cheat Actor"), MultiCheatActor->EditableComponent->ApplyCollisionModification(InstigatingActor));
	TestTrue(TEXT("Scale can activate alongside Collision"), MultiCheatActor->EditableComponent->ApplyScaleModification(ERealityScalePreset::Half, InstigatingActor));
	TestTrue(TEXT("Gravity can activate alongside Collision and Scale"), MultiCheatActor->EditableComponent->ApplyGravityModification(ERealityGravityPreset::Low, InstigatingActor));
	TestTrue(TEXT("All three modification states are active"), MultiCheatActor->EditableComponent->IsCollisionModified() && MultiCheatActor->EditableComponent->IsScaleModified() && MultiCheatActor->EditableComponent->IsGravityModified());
	TestTrue(TEXT("Restoring Scale succeeds independently"), MultiCheatActor->EditableComponent->RestoreScaleModification(InstigatingActor));
	TestTrue(TEXT("Collision remains active after Scale Restore"), MultiCheatActor->EditableComponent->IsCollisionModified());
	TestTrue(TEXT("Gravity remains active after Scale Restore"), MultiCheatActor->EditableComponent->IsGravityModified());
	TestTrue(TEXT("Restoring Collision succeeds independently"), MultiCheatActor->EditableComponent->RestoreCollisionModification(InstigatingActor));
	TestTrue(TEXT("Gravity remains active after Collision Restore"), MultiCheatActor->EditableComponent->IsGravityModified());
	TestTrue(TEXT("Restoring Gravity succeeds last"), MultiCheatActor->EditableComponent->RestoreGravityModification(InstigatingActor));

	TestFalse(TEXT("An invalid Gravity preset fails safely"), TestActor->EditableComponent->ApplyGravityModification(static_cast<ERealityGravityPreset>(255), InstigatingActor));
	URealityEditableComponent* OwnerlessComponent = NewObject<URealityEditableComponent>();
	SetCapabilities(OwnerlessComponent, true);
	TestFalse(TEXT("An ownerless component fails Gravity Apply safely"), OwnerlessComponent->ApplyGravityModification(ERealityGravityPreset::Zero, InstigatingActor));
	TestFalse(TEXT("An ownerless component fails Gravity Restore safely"), OwnerlessComponent->RestoreGravityModification(InstigatingActor));

	TestWorld->DestroyWorld(false);
	TestWorld->SetPhysicsScene(nullptr);
	GEngine->DestroyWorldContext(TestWorld);
	TestWorld->RemoveFromRoot();
	return true;
}

#endif
