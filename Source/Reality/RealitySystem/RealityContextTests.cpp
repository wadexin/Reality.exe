// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"

#include "AI/RealityWitnessComponent.h"
#include "AI/RealityWitnessTestActor.h"
#include "Developer/RealityEditableTestActor.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "RealitySystem/RealityContextComponent.h"
#include "RealitySystem/RealityContextTestActor.h"
#include "RealitySystem/RealityManagerSubsystem.h"
#include "RealitySystem/RealitySuspicionSettings.h"

namespace RealityContextTests
{
	FGameplayTag Tag(const TCHAR* Name)
	{
		return FGameplayTag::RequestGameplayTag(Name);
	}

	FRealityCheatEvent Event(AActor* Target, const TCHAR* Cheat, const ERealityCheatOperation Operation = ERealityCheatOperation::Apply)
	{
		return FRealityCheatEvent(Target, Tag(Cheat), nullptr, Operation);
	}

	ARealityContextTestActor* SpawnContext(UWorld* World, const FVector& Location, const std::initializer_list<const TCHAR*> Tags, const float Range = 1000.0f, const bool bActive = true)
	{
		ARealityContextTestActor* Source = World->SpawnActor<ARealityContextTestActor>();
		Source->SetActorLocation(Location);
		FGameplayTagContainer ContextTags;
		for (const TCHAR* TagName : Tags)
		{
			ContextTags.AddTag(Tag(TagName));
		}
		Source->ContextComponent->ContextTags = ContextTags;
		Source->ContextComponent->EffectRange = Range;
		Source->ContextComponent->SetContextActive(bActive);
		if (!Source->HasActorBegunPlay())
		{
			Source->DispatchBeginPlay();
		}
		return Source;
	}

	ARealityWitnessTestActor* SpawnHuman(UWorld* World)
	{
		ARealityWitnessTestActor* Witness = World->SpawnActor<ARealityWitnessTestActor>();
		Witness->SetActorLocation(FVector::ZeroVector);
		Witness->WitnessComponent->WitnessType = Tag(TEXT("Witness.Human"));
		Witness->WitnessComponent->bRequireLineOfSight = false;
		Witness->WitnessComponent->OnRealityEventObserved.AddDynamic(Witness, &ARealityWitnessTestActor::HandleRealityEventObserved);
		if (!Witness->HasActorBegunPlay())
		{
			Witness->DispatchBeginPlay();
		}
		return Witness;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRealityContextSystemTest,
	"Reality.System.Context",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::EngineFilter)

bool FRealityContextSystemTest::RunTest(const FString& Parameters)
{
	using namespace RealityContextTests;

	const FName WorldName = MakeUniqueObjectName(nullptr, UWorld::StaticClass(), NAME_None, EUniqueObjectNameOptions::GloballyUnique);
	FWorldContext& WorldContext = GEngine->CreateNewWorldContext(EWorldType::Game);
	UWorld* TestWorld = UWorld::CreateWorld(EWorldType::Game, false, WorldName, GetTransientPackage());
	if (!TestNotNull(TEXT("A Context test world can be created"), TestWorld))
	{
		GEngine->DestroyWorldContext(TestWorld);
		return false;
	}

	TestWorld->AddToRoot();
	WorldContext.SetCurrentWorld(TestWorld);
	TestWorld->InitializeActorsForPlay(FURL());
	TestWorld->BeginPlay();

	URealityManagerSubsystem* Manager = TestWorld->GetSubsystem<URealityManagerSubsystem>();
	const URealitySuspicionSettings* Settings = GetDefault<URealitySuspicionSettings>();
	TestNotNull(TEXT("Context uses the world Reality Manager"), Manager);
	TestNotNull(TEXT("Context tuning settings are available"), Settings);
	TestEqual(TEXT("Context cap defaults to 25"), Settings->MaximumContextReductionPerEvent, 25.0f);

	float Reduction = 0.0f;
	TestTrue(TEXT("Gravity plus Explosion is configured"), Settings->FindContextReduction(Tag(TEXT("Cheat.Gravity")), Tag(TEXT("Context.Explosion")), Reduction));
	TestEqual(TEXT("Gravity plus Explosion reduces 15"), Reduction, 15.0f);
	TestFalse(TEXT("Collision plus Explosion is not invented"), Settings->FindContextReduction(Tag(TEXT("Cheat.Collision")), Tag(TEXT("Context.Explosion")), Reduction));

	ARealityEditableTestActor* Target = TestWorld->SpawnActor<ARealityEditableTestActor>();
	Target->SetActorLocation(FVector(500.0f, 0.0f, 0.0f));
	ARealityContextTestActor* Explosion = SpawnContext(TestWorld, FVector::ZeroVector, {TEXT("Context.Explosion")});
	TestEqual(TEXT("Active Context self-registers"), Manager->GetRegisteredContextCount(), 1);
	TestFalse(TEXT("Context component never ticks"), Explosion->ContextComponent->PrimaryComponentTick.bCanEverTick);
	TestTrue(TEXT("Nearby target is within Context"), Explosion->ContextComponent->IsTargetWithinContext(Target));

	Manager->ResetRealityState();
	TestTrue(TEXT("Perfectly explained Gravity is processed"), Manager->ProcessCheatEvent(Event(Target, TEXT("Cheat.Gravity"))));
	TestEqual(TEXT("Explosion fully explains Gravity base"), Manager->GetSuspicion(), 0.0f);
	const FRealityProcessedCheatRecord* PerfectRecord = Manager->GetMostRecentEvent();
	if (TestNotNull(TEXT("Zero-delta Apply still creates history"), PerfectRecord))
	{
		TestEqual(TEXT("Perfect history base"), PerfectRecord->BaseSuspicionDelta, 15.0f);
		TestEqual(TEXT("Perfect history Context reduction"), PerfectRecord->ContextSuspicionReduction, 15.0f);
		TestEqual(TEXT("Perfect history matched count"), PerfectRecord->MatchedContextCount, 1);
		TestEqual(TEXT("Perfect history final delta"), PerfectRecord->SuspicionDelta, 0.0f);
	}

	Explosion->ContextComponent->SetContextActive(false);
	Manager->ResetRealityState();
	Manager->ProcessCheatEvent(Event(Target, TEXT("Cheat.Gravity")));
	TestEqual(TEXT("Inactive Context gives base only"), Manager->GetSuspicion(), 15.0f);
	Explosion->ContextComponent->SetContextActive(true);
	Explosion->ContextComponent->EffectRange = 100.0f;
	Manager->ResetRealityState();
	Manager->ProcessCheatEvent(Event(Target, TEXT("Cheat.Gravity")));
	TestEqual(TEXT("Out-of-range Context gives base only"), Manager->GetSuspicion(), 15.0f);
	Explosion->ContextComponent->EffectRange = 1000.0f;

	Manager->ResetRealityState();
	Manager->ProcessCheatEvent(Event(Target, TEXT("Cheat.Collision")));
	TestEqual(TEXT("Mismatched Explosion does not explain Collision"), Manager->GetSuspicion(), 25.0f);

	ARealityContextTestActor* SensorFailure = SpawnContext(TestWorld, FVector(500.0f, 0.0f, 0.0f), {TEXT("Context.SensorFailure")});
	Manager->ResetRealityState();
	Manager->ProcessCheatEvent(Event(Target, TEXT("Cheat.Collision")));
	TestEqual(TEXT("SensorFailure explains part of Collision"), Manager->GetSuspicion(), 10.0f);

	ARealityWitnessTestActor* Human = SpawnHuman(TestWorld);
	Manager->ResetRealityState();
	Manager->ProcessCheatEvent(Event(Target, TEXT("Cheat.Gravity")));
	TestEqual(TEXT("Base plus Human minus Explosion equals 10"), Manager->GetSuspicion(), 10.0f);
	TestEqual(TEXT("Context does not suppress Witness notification"), Human->ObservedEventCount, 1);
	const FRealityProcessedCheatRecord* CombinedRecord = Manager->GetMostRecentEvent();
	if (TestNotNull(TEXT("Combined transaction has history"), CombinedRecord))
	{
		TestEqual(TEXT("Combined history Witness"), CombinedRecord->WitnessSuspicionDelta, 10.0f);
		TestEqual(TEXT("Combined history observer count"), CombinedRecord->ObservingWitnessCount, 1);
		TestEqual(TEXT("Combined history Context"), CombinedRecord->ContextSuspicionReduction, 15.0f);
		TestEqual(TEXT("Combined history final"), CombinedRecord->SuspicionDelta, 10.0f);
	}

	ARealityContextTestActor* DuplicateExplosion = SpawnContext(TestWorld, FVector(400.0f, 0.0f, 0.0f), {TEXT("Context.Explosion")});
	Human->WitnessComponent->bWitnessEnabled = false;
	Manager->ResetRealityState();
	Manager->ProcessCheatEvent(Event(Target, TEXT("Cheat.Gravity")));
	TestEqual(TEXT("Duplicate Explosion sources apply once"), Manager->GetSuspicion(), 0.0f);
	TestEqual(TEXT("Duplicate same-tag sources report one match"), Manager->GetMostRecentEvent()->MatchedContextCount, 1);

	ARealityContextTestActor* Fire = SpawnContext(TestWorld, FVector(500.0f, 0.0f, 0.0f), {TEXT("Context.Fire")});
	Human->WitnessComponent->bWitnessEnabled = true;
	Manager->ResetRealityState();
	Manager->ProcessCheatEvent(Event(Target, TEXT("Cheat.Gravity")));
	TestEqual(TEXT("Distinct Explosion and Fire combine"), Manager->GetSuspicion(), 5.0f);
	TestEqual(TEXT("Two distinct matching Context tags are recorded"), Manager->GetMostRecentEvent()->MatchedContextCount, 2);

	ARealityWitnessTestActor* Camera = SpawnHuman(TestWorld);
	Camera->WitnessComponent->WitnessType = Tag(TEXT("Witness.Camera"));
	URealitySuspicionSettings* MutableSettings = GetMutableDefault<URealitySuspicionSettings>();
	const float OriginalContextCap = MutableSettings->MaximumContextReductionPerEvent;
	MutableSettings->MaximumContextReductionPerEvent = 10.0f;
	Manager->ResetRealityState();
	Manager->ProcessCheatEvent(Event(Target, TEXT("Cheat.Gravity")));
	TestEqual(TEXT("Configured Context cap limits combined reduction"), Manager->GetMostRecentEvent()->ContextSuspicionReduction, 10.0f);
	TestEqual(TEXT("Capped Context leaves remaining raw risk"), Manager->GetSuspicion(), 30.0f);
	TestEqual(TEXT("Final post-Context value drives Questioning"), Manager->GetRealityState(), ERealityState::Questioning);
	MutableSettings->MaximumContextReductionPerEvent = OriginalContextCap;

	ARealityContextTestActor* Unknown = SpawnContext(TestWorld, FVector(500.0f, 0.0f, 0.0f), {TEXT("Context.Blackout")});
	Explosion->ContextComponent->SetContextActive(false);
	DuplicateExplosion->ContextComponent->SetContextActive(false);
	Fire->ContextComponent->SetContextActive(false);
	SensorFailure->ContextComponent->SetContextActive(false);
	Human->WitnessComponent->bWitnessEnabled = false;
	Camera->WitnessComponent->bWitnessEnabled = false;
	Manager->ResetRealityState();
	Manager->ProcessCheatEvent(Event(Target, TEXT("Cheat.Gravity")));
	TestEqual(TEXT("Unknown Context combination contributes zero"), Manager->GetSuspicion(), 15.0f);

	Explosion->ContextComponent->SetContextActive(true);
	Manager->ResetRealityState();
	Manager->ProcessCheatEvent(Event(Target, TEXT("Cheat.Collision")));
	Manager->ProcessCheatEvent(Event(Target, TEXT("Cheat.Collision"), ERealityCheatOperation::Restore));
	TestEqual(TEXT("Restore remains exactly minus five"), Manager->GetSuspicion(), 20.0f);
	TestEqual(TEXT("Restore has zero Context"), Manager->GetMostRecentEvent()->ContextSuspicionReduction, 0.0f);
	TestEqual(TEXT("Restore has zero matched Contexts"), Manager->GetMostRecentEvent()->MatchedContextCount, 0);

	ARealityEditableTestActor* DistantTarget = TestWorld->SpawnActor<ARealityEditableTestActor>();
	DistantTarget->SetActorLocation(FVector(5000.0f, 0.0f, 0.0f));
	Manager->ResetRealityState();
	Manager->ProcessCheatEvent(Event(DistantTarget, TEXT("Cheat.Gravity")));
	TestEqual(TEXT("Context is evaluated per target"), Manager->GetSuspicion(), 15.0f);

	const int32 BeforeDestroyCount = Manager->GetRegisteredContextCount();
	DuplicateExplosion->Destroy();
	TestEqual(TEXT("Destroyed Context unregisters"), Manager->GetRegisteredContextCount(), BeforeDestroyCount - 1);
	Manager->UnregisterContext(DuplicateExplosion->ContextComponent);
	TestEqual(TEXT("Duplicate unregister is safe"), Manager->GetRegisteredContextCount(), BeforeDestroyCount - 1);

	TestWorld->DestroyWorld(false);
	TestWorld->SetPhysicsScene(nullptr);
	GEngine->DestroyWorldContext(TestWorld);
	TestWorld->RemoveFromRoot();
	return true;
}

#endif
