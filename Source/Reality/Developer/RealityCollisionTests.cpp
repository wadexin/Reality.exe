// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"

#include "Components/BoxComponent.h"
#include "Developer/RealityEditableComponent.h"
#include "Developer/RealityEditableTestActor.h"
#include "Engine/Engine.h"
#include "Engine/World.h"

namespace RealityCollisionTests
{
	FGameplayTag GetCollisionTag()
	{
		return FGameplayTag::RequestGameplayTag(TEXT("Cheat.Collision"));
	}

	void EnableCollisionCapability(URealityEditableComponent* EditableComponent)
	{
		FGameplayTagContainer SupportedCheats;
		SupportedCheats.AddTag(GetCollisionTag());
		EditableComponent->SetSupportedCheats(SupportedCheats);
	}

	ARealityEditableTestActor* CreateTestActor(UWorld* World)
	{
		ARealityEditableTestActor* TestActor = World->SpawnActor<ARealityEditableTestActor>();
		TestActor->EditableComponent->OnRealityCheatEvent.AddDynamic(TestActor, &ARealityEditableTestActor::HandleRealityCheatEvent);
		return TestActor;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRealityCollisionModificationTest,
	"Reality.Developer.CollisionModification",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::EngineFilter)

bool FRealityCollisionModificationTest::RunTest(const FString& Parameters)
{
	using namespace RealityCollisionTests;

	const FName WorldName = MakeUniqueObjectName(nullptr, UWorld::StaticClass(), NAME_None, EUniqueObjectNameOptions::GloballyUnique);
	FWorldContext& WorldContext = GEngine->CreateNewWorldContext(EWorldType::Game);
	UWorld* TestWorld = UWorld::CreateWorld(EWorldType::Game, false, WorldName, GetTransientPackage());
	if (!TestNotNull(TEXT("A collision test world can be created"), TestWorld))
	{
		GEngine->DestroyWorldContext(TestWorld);
		return false;
	}

	TestWorld->AddToRoot();
	WorldContext.SetCurrentWorld(TestWorld);
	TestWorld->InitializeActorsForPlay(FURL());

	AActor* InstigatingActor = TestWorld->SpawnActor<AActor>();
	ARealityEditableTestActor* TestActor = CreateTestActor(TestWorld);
	EnableCollisionCapability(TestActor->EditableComponent);
	TestActor->PrimitiveA->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	TestActor->PrimitiveB->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	TestTrue(TEXT("Supported collision Apply succeeds"), TestActor->EditableComponent->ApplyCollisionModification(InstigatingActor));
	TestEqual(TEXT("Primitive A collision is disabled"), TestActor->PrimitiveA->GetCollisionEnabled(), ECollisionEnabled::NoCollision);
	TestEqual(TEXT("Primitive B collision is disabled"), TestActor->PrimitiveB->GetCollisionEnabled(), ECollisionEnabled::NoCollision);
	TestTrue(TEXT("Collision reports modified after Apply"), TestActor->EditableComponent->IsCollisionModified());
	TestEqual(TEXT("Apply emits exactly one event"), TestActor->CheatEventCount, 1);
	TestEqual(TEXT("Apply event target is the editable owner"), TestActor->LastCheatEvent.TargetActor.Get(), static_cast<AActor*>(TestActor));
	TestEqual(TEXT("Apply event retains the caller-provided instigator"), TestActor->LastCheatEvent.InstigatingActor.Get(), InstigatingActor);
	TestEqual(TEXT("Apply event uses Cheat.Collision"), TestActor->LastCheatEvent.CheatTag, GetCollisionTag());
	TestEqual(TEXT("Apply event identifies Apply"), TestActor->LastCheatEvent.Operation, ERealityCheatOperation::Apply);

	TestFalse(TEXT("Repeated Apply is rejected without corrupting original state"), TestActor->EditableComponent->ApplyCollisionModification(InstigatingActor));
	TestEqual(TEXT("Repeated Apply emits no duplicate event"), TestActor->CheatEventCount, 1);
	TestTrue(TEXT("Restore succeeds after Apply"), TestActor->EditableComponent->RestoreCollisionModification(InstigatingActor));
	TestEqual(TEXT("Primitive A restores QueryAndPhysics exactly"), TestActor->PrimitiveA->GetCollisionEnabled(), ECollisionEnabled::QueryAndPhysics);
	TestEqual(TEXT("Primitive B restores QueryOnly exactly"), TestActor->PrimitiveB->GetCollisionEnabled(), ECollisionEnabled::QueryOnly);
	TestFalse(TEXT("Collision reports unmodified after Restore"), TestActor->EditableComponent->IsCollisionModified());
	TestEqual(TEXT("Restore emits exactly one additional event"), TestActor->CheatEventCount, 2);
	TestEqual(TEXT("Restore event identifies Restore"), TestActor->LastCheatEvent.Operation, ERealityCheatOperation::Restore);
	TestFalse(TEXT("Repeated Restore fails safely"), TestActor->EditableComponent->RestoreCollisionModification(InstigatingActor));
	TestEqual(TEXT("Repeated Restore emits no duplicate event"), TestActor->CheatEventCount, 2);

	TestActor->PrimitiveA->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	TestActor->PrimitiveB->SetCollisionEnabled(ECollisionEnabled::QueryAndProbe);
	TestTrue(TEXT("A second Apply cycle succeeds"), TestActor->EditableComponent->ApplyCollisionModification(nullptr));
	TestTrue(TEXT("A second Restore cycle succeeds"), TestActor->EditableComponent->RestoreCollisionModification(nullptr));
	TestEqual(TEXT("Second cycle restores the new PhysicsOnly state"), TestActor->PrimitiveA->GetCollisionEnabled(), ECollisionEnabled::PhysicsOnly);
	TestEqual(TEXT("Second cycle restores the new QueryAndProbe state"), TestActor->PrimitiveB->GetCollisionEnabled(), ECollisionEnabled::QueryAndProbe);
	TestNull(TEXT("An optional null instigator is retained safely"), TestActor->LastCheatEvent.InstigatingActor.Get());

	ARealityEditableTestActor* UnsupportedActor = CreateTestActor(TestWorld);
	UnsupportedActor->PrimitiveA->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TestFalse(TEXT("Unsupported collision Apply fails"), UnsupportedActor->EditableComponent->ApplyCollisionModification(InstigatingActor));
	TestEqual(TEXT("Unsupported Apply leaves collision unchanged"), UnsupportedActor->PrimitiveA->GetCollisionEnabled(), ECollisionEnabled::QueryOnly);
	TestEqual(TEXT("Unsupported Apply emits no event"), UnsupportedActor->CheatEventCount, 0);

	AActor* EmptyActor = TestWorld->SpawnActor<AActor>();
	URealityEditableComponent* EmptyEditableComponent = NewObject<URealityEditableComponent>(EmptyActor);
	EmptyActor->AddInstanceComponent(EmptyEditableComponent);
	EnableCollisionCapability(EmptyEditableComponent);
	TestFalse(TEXT("An actor without PrimitiveComponents fails Apply safely"), EmptyEditableComponent->ApplyCollisionModification(InstigatingActor));

	ARealityEditableTestActor* AlreadyDisabledActor = CreateTestActor(TestWorld);
	EnableCollisionCapability(AlreadyDisabledActor->EditableComponent);
	AlreadyDisabledActor->PrimitiveA->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AlreadyDisabledActor->PrimitiveB->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	TestFalse(TEXT("An actor with no collision state to change fails Apply safely"), AlreadyDisabledActor->EditableComponent->ApplyCollisionModification(InstigatingActor));
	TestEqual(TEXT("An already-disabled Actor emits no false Apply event"), AlreadyDisabledActor->CheatEventCount, 0);

	URealityEditableComponent* OwnerlessComponent = NewObject<URealityEditableComponent>();
	EnableCollisionCapability(OwnerlessComponent);
	TestFalse(TEXT("An ownerless component fails Apply safely"), OwnerlessComponent->ApplyCollisionModification(InstigatingActor));
	TestFalse(TEXT("An ownerless component fails Restore safely"), OwnerlessComponent->RestoreCollisionModification(InstigatingActor));

	ARealityEditableTestActor* DestroyedTargetActor = CreateTestActor(TestWorld);
	EnableCollisionCapability(DestroyedTargetActor->EditableComponent);
	DestroyedTargetActor->PrimitiveA->SetCollisionEnabled(ECollisionEnabled::ProbeOnly);
	DestroyedTargetActor->PrimitiveB->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TestTrue(TEXT("Apply succeeds before a collision target is destroyed"), DestroyedTargetActor->EditableComponent->ApplyCollisionModification(InstigatingActor));
	DestroyedTargetActor->PrimitiveB->DestroyComponent();
	TestTrue(TEXT("Restore safely skips a destroyed target and restores survivors"), DestroyedTargetActor->EditableComponent->RestoreCollisionModification(InstigatingActor));
	TestEqual(TEXT("Surviving target restores its exact state"), DestroyedTargetActor->PrimitiveA->GetCollisionEnabled(), ECollisionEnabled::ProbeOnly);
	TestEqual(TEXT("Destroyed-target cycle still emits one Apply and one Restore event"), DestroyedTargetActor->CheatEventCount, 2);

	TestWorld->DestroyWorld(false);
	TestWorld->SetPhysicsScene(nullptr);
	GEngine->DestroyWorldContext(TestWorld);
	TestWorld->RemoveFromRoot();

	return true;
}

#endif
