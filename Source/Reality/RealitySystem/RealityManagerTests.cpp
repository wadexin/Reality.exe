// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"

#include "Components/BoxComponent.h"
#include "Developer/RealityEditableComponent.h"
#include "Developer/RealityEditableTestActor.h"
#include "Engine/CollisionProfile.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "RealitySystem/RealityManagerSubsystem.h"
#include "RealitySystem/RealitySuspicionSettings.h"

namespace RealityManagerTests
{
	FGameplayTag Tag(const TCHAR* Name)
	{
		return FGameplayTag::RequestGameplayTag(Name);
	}

	FRealityCheatEvent Event(AActor* Target, const TCHAR* TagName, const ERealityCheatOperation Operation = ERealityCheatOperation::Apply)
	{
		return FRealityCheatEvent(Target, Tag(TagName), nullptr, Operation);
	}

	void SetCapabilities(URealityEditableComponent* Editable, const std::initializer_list<FGameplayTag> Tags)
	{
		FGameplayTagContainer Capabilities;
		for (const FGameplayTag Capability : Tags)
		{
			Capabilities.AddTag(Capability);
		}
		Editable->SetSupportedCheats(Capabilities);
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRealityManagerSubsystemTest,
	"Reality.System.Manager",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::EngineFilter)

bool FRealityManagerSubsystemTest::RunTest(const FString& Parameters)
{
	using namespace RealityManagerTests;

	const FName WorldName = MakeUniqueObjectName(nullptr, UWorld::StaticClass(), NAME_None, EUniqueObjectNameOptions::GloballyUnique);
	FWorldContext& WorldContext = GEngine->CreateNewWorldContext(EWorldType::Game);
	UWorld* TestWorld = UWorld::CreateWorld(EWorldType::Game, false, WorldName, GetTransientPackage());
	if (!TestNotNull(TEXT("A Reality Manager test world can be created"), TestWorld))
	{
		GEngine->DestroyWorldContext(TestWorld);
		return false;
	}

	TestWorld->AddToRoot();
	WorldContext.SetCurrentWorld(TestWorld);
	TestWorld->InitializeActorsForPlay(FURL());

	URealityManagerSubsystem* Manager = TestWorld->GetSubsystem<URealityManagerSubsystem>();
	if (!TestNotNull(TEXT("A world owns one Reality Manager subsystem"), Manager))
	{
		TestWorld->DestroyWorld(false);
		GEngine->DestroyWorldContext(TestWorld);
		TestWorld->RemoveFromRoot();
		return false;
	}

	const URealitySuspicionSettings* Settings = GetDefault<URealitySuspicionSettings>();
	TestNotNull(TEXT("Config-backed Reality Suspicion settings are available"), Settings);
	TestEqual(TEXT("Initial Suspicion is zero"), Manager->GetSuspicion(), 0.0f);
	TestEqual(TEXT("Initial Reality state is Stable"), Manager->GetRealityState(), ERealityState::Stable);
	TestEqual(TEXT("Initial event history is empty"), Manager->GetEventHistory().Num(), 0);

	float CollisionValue = 0.0f;
	float ScaleValue = 0.0f;
	float GravityValue = 0.0f;
	float MassValue = 0.0f;
	float FrictionValue = 0.0f;
	TestTrue(TEXT("Collision has a configured rule"), Settings->FindBaseSuspicion(Tag(TEXT("Cheat.Collision")), CollisionValue));
	TestTrue(TEXT("Scale has a configured rule"), Settings->FindBaseSuspicion(Tag(TEXT("Cheat.Scale")), ScaleValue));
	TestTrue(TEXT("Gravity has a configured rule"), Settings->FindBaseSuspicion(Tag(TEXT("Cheat.Gravity")), GravityValue));
	TestTrue(TEXT("Mass has a configured rule"), Settings->FindBaseSuspicion(Tag(TEXT("Cheat.Mass")), MassValue));
	TestTrue(TEXT("Friction has a configured rule"), Settings->FindBaseSuspicion(Tag(TEXT("Cheat.Friction")), FrictionValue));
	TestEqual(TEXT("Collision prototype tuning is 25"), CollisionValue, 25.0f);
	TestEqual(TEXT("Scale prototype tuning is 20"), ScaleValue, 20.0f);
	TestEqual(TEXT("Gravity prototype tuning is 15"), GravityValue, 15.0f);
	TestEqual(TEXT("Mass prototype tuning is 10"), MassValue, 10.0f);
	TestEqual(TEXT("Friction prototype tuning is 10"), FrictionValue, 10.0f);
	TestEqual(TEXT("Restore prototype reduction is 5"), Settings->RestoreSuspicionReduction, 5.0f);

	ARealityEditableTestActor* DelegateRecorder = TestWorld->SpawnActor<ARealityEditableTestActor>();
	Manager->OnRealitySuspicionChanged.AddDynamic(DelegateRecorder, &ARealityEditableTestActor::HandleSuspicionChanged);
	Manager->OnRealityStateChanged.AddDynamic(DelegateRecorder, &ARealityEditableTestActor::HandleRealityStateChanged);

	TestTrue(TEXT("Scale Apply can reach the exact Questioning threshold"), Manager->ProcessCheatEvent(Event(DelegateRecorder, TEXT("Cheat.Scale"))));
	TestEqual(TEXT("20 Suspicion is Questioning"), Manager->GetSuspicion(), 20.0f);
	TestEqual(TEXT("The exact 20 threshold enters Questioning"), Manager->GetRealityState(), ERealityState::Questioning);
	Manager->ResetRealityState();
	DelegateRecorder->SuspicionChangedCount = 0;
	DelegateRecorder->RealityStateChangedCount = 0;

	TestTrue(TEXT("Collision Apply is processed"), Manager->ProcessCheatEvent(Event(DelegateRecorder, TEXT("Cheat.Collision"))));
	TestEqual(TEXT("Collision Apply adds configured Suspicion"), Manager->GetSuspicion(), 25.0f);
	TestEqual(TEXT("25 Suspicion is Questioning"), Manager->GetRealityState(), ERealityState::Questioning);
	TestEqual(TEXT("Suspicion delegate fires for Collision"), DelegateRecorder->SuspicionChangedCount, 1);
	TestEqual(TEXT("State delegate fires for Stable to Questioning"), DelegateRecorder->RealityStateChangedCount, 1);
	const TArray<FRealityProcessedCheatRecord> FirstHistory = Manager->GetEventHistory();
	if (TestEqual(TEXT("Collision Apply creates one history record"), FirstHistory.Num(), 1))
	{
		TestEqual(TEXT("History records the target name"), FirstHistory[0].TargetActorName, GetNameSafe(DelegateRecorder));
		TestEqual(TEXT("History records Suspicion before"), FirstHistory[0].SuspicionBefore, 0.0f);
		TestEqual(TEXT("History records the applied delta"), FirstHistory[0].SuspicionDelta, 25.0f);
		TestEqual(TEXT("History records Suspicion after"), FirstHistory[0].SuspicionAfter, 25.0f);
		TestEqual(TEXT("History records resulting state"), FirstHistory[0].ResultingState, ERealityState::Questioning);
	}

	TestTrue(TEXT("Gravity Apply is processed"), Manager->ProcessCheatEvent(Event(DelegateRecorder, TEXT("Cheat.Gravity"))));
	TestEqual(TEXT("Gravity Apply reaches exactly 40"), Manager->GetSuspicion(), 40.0f);
	TestEqual(TEXT("40 Suspicion is Investigating"), Manager->GetRealityState(), ERealityState::Investigating);
	TestTrue(TEXT("Scale Apply is processed"), Manager->ProcessCheatEvent(Event(DelegateRecorder, TEXT("Cheat.Scale"))));
	TestEqual(TEXT("Scale Apply reaches exactly 60"), Manager->GetSuspicion(), 60.0f);
	TestEqual(TEXT("60 Suspicion is Unstable"), Manager->GetRealityState(), ERealityState::Unstable);
	TestTrue(TEXT("A second Scale Apply is processed"), Manager->ProcessCheatEvent(Event(DelegateRecorder, TEXT("Cheat.Scale"))));
	TestEqual(TEXT("80 Suspicion is Correction"), Manager->GetRealityState(), ERealityState::Correction);

	const int32 StateEventsAtCorrection = DelegateRecorder->RealityStateChangedCount;
	const int32 SuspicionEventsAtCorrection = DelegateRecorder->SuspicionChangedCount;
	TestTrue(TEXT("Suspicion can change within Correction"), Manager->ProcessCheatEvent(Event(DelegateRecorder, TEXT("Cheat.Gravity"))));
	TestEqual(TEXT("Suspicion delegate fires within the same state"), DelegateRecorder->SuspicionChangedCount, SuspicionEventsAtCorrection + 1);
	TestEqual(TEXT("State delegate does not duplicate within Correction"), DelegateRecorder->RealityStateChangedCount, StateEventsAtCorrection);

	for (int32 Index = 0; Index < 8; ++Index)
	{
		Manager->ProcessCheatEvent(Event(DelegateRecorder, TEXT("Cheat.Collision")));
	}
	TestEqual(TEXT("Suspicion clamps at 100"), Manager->GetSuspicion(), 100.0f);

	const float BeforeUnknown = Manager->GetSuspicion();
	const int32 HistoryBeforeUnknown = Manager->GetEventHistory().Num();
	TestFalse(TEXT("A known but untuned Cheat tag is rejected"), Manager->ProcessCheatEvent(Event(DelegateRecorder, TEXT("Cheat.Time"))));
	TestEqual(TEXT("Unknown tuning does not change Suspicion"), Manager->GetSuspicion(), BeforeUnknown);
	TestEqual(TEXT("Unknown tuning does not add history"), Manager->GetEventHistory().Num(), HistoryBeforeUnknown);

	Manager->ResetRealityState();
	TestEqual(TEXT("Reset returns Suspicion to zero"), Manager->GetSuspicion(), 0.0f);
	TestEqual(TEXT("Reset returns state to Stable"), Manager->GetRealityState(), ERealityState::Stable);
	TestEqual(TEXT("Reset clears history"), Manager->GetEventHistory().Num(), 0);
	TestTrue(TEXT("Restore at zero is safely processed"), Manager->ProcessCheatEvent(Event(DelegateRecorder, TEXT("Cheat.Collision"), ERealityCheatOperation::Restore)));
	TestEqual(TEXT("Restore never reduces below zero"), Manager->GetSuspicion(), 0.0f);

	Manager->ResetRealityState();
	for (int32 Index = 0; Index < Settings->MaxEventHistory + 5; ++Index)
	{
		Manager->ProcessCheatEvent(Event(DelegateRecorder, TEXT("Cheat.Gravity")));
	}
	const TArray<FRealityProcessedCheatRecord> BoundedHistory = Manager->GetEventHistory();
	TestEqual(TEXT("Event history is bounded by settings"), BoundedHistory.Num(), Settings->MaxEventHistory);
	if (!BoundedHistory.IsEmpty())
	{
		const FRealityProcessedCheatRecord& Latest = BoundedHistory.Last();
		TestEqual(TEXT("History retains Cheat tag"), Latest.CheatTag, Tag(TEXT("Cheat.Gravity")));
		TestEqual(TEXT("History retains Before"), Latest.SuspicionBefore, 100.0f);
		TestEqual(TEXT("History retains actual clamped Delta"), Latest.SuspicionDelta, 0.0f);
		TestEqual(TEXT("History retains After"), Latest.SuspicionAfter, 100.0f);
		TestEqual(TEXT("History retains resulting state"), Latest.ResultingState, ERealityState::Correction);
	}

	Manager->ResetRealityState();
	ARealityEditableTestActor* IntegratedActor = TestWorld->SpawnActor<ARealityEditableTestActor>();
	SetCapabilities(IntegratedActor->EditableComponent, {
		Tag(TEXT("Cheat.Collision")),
		Tag(TEXT("Cheat.Scale")),
		Tag(TEXT("Cheat.Gravity"))
	});
	IntegratedActor->PrimitiveA->SetCollisionProfileName(UCollisionProfile::PhysicsActor_ProfileName);
	IntegratedActor->PrimitiveA->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	IntegratedActor->PrimitiveA->SetSimulatePhysics(true);
	IntegratedActor->PrimitiveA->SetEnableGravity(true);
	IntegratedActor->PrimitiveB->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	TestTrue(TEXT("Real Collision Apply succeeds"), IntegratedActor->EditableComponent->ApplyCollisionModification(DelegateRecorder));
	TestEqual(TEXT("Real Collision Apply reaches Manager exactly once"), Manager->GetEventHistory().Num(), 1);
	TestEqual(TEXT("Real Collision Apply adds 25"), Manager->GetSuspicion(), 25.0f);
	TestTrue(TEXT("Real Collision Restore succeeds"), IntegratedActor->EditableComponent->RestoreCollisionModification(DelegateRecorder));
	TestEqual(TEXT("Real Restore reaches Manager exactly once"), Manager->GetEventHistory().Num(), 2);
	TestEqual(TEXT("Real Restore applies cleanup reduction"), Manager->GetSuspicion(), 20.0f);
	TestTrue(TEXT("Real Scale Apply succeeds"), IntegratedActor->EditableComponent->ApplyScaleModification(ERealityScalePreset::Double, DelegateRecorder));
	TestEqual(TEXT("Real Scale event adds configured amount"), Manager->GetSuspicion(), 40.0f);
	TestTrue(TEXT("Real Gravity Apply succeeds"), IntegratedActor->EditableComponent->ApplyGravityModification(ERealityGravityPreset::Zero, DelegateRecorder));
	TestEqual(TEXT("Real Gravity event adds configured amount"), Manager->GetSuspicion(), 55.0f);
	TestEqual(TEXT("Three Applies plus one Restore produce four records"), Manager->GetEventHistory().Num(), 4);
	TestTrue(TEXT("Collision remains restored after other events"), !IntegratedActor->EditableComponent->IsCollisionModified());
	TestTrue(TEXT("Scale remains independently modified"), IntegratedActor->EditableComponent->IsScaleModified());
	TestTrue(TEXT("Gravity remains independently modified"), IntegratedActor->EditableComponent->IsGravityModified());

	TestWorld->DestroyWorld(false);
	TestWorld->SetPhysicsScene(nullptr);
	GEngine->DestroyWorldContext(TestWorld);
	TestWorld->RemoveFromRoot();
	return true;
}

#endif
