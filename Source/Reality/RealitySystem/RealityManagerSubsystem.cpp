// Copyright Epic Games, Inc. All Rights Reserved.

#include "RealitySystem/RealityManagerSubsystem.h"

#include "Reality.h"
#include "RealitySystem/RealitySuspicionSettings.h"

bool URealityManagerSubsystem::ProcessCheatEvent(const FRealityCheatEvent& CheatEvent)
{
	if (CheatEvent.Operation != ERealityCheatOperation::Apply && CheatEvent.Operation != ERealityCheatOperation::Restore)
	{
		UE_LOG(LogReality, Warning, TEXT("Reality Manager: Ignored event for '%s' because operation value %d is invalid."), *GetNameSafe(CheatEvent.TargetActor), static_cast<uint8>(CheatEvent.Operation));
		return false;
	}

	const URealitySuspicionSettings* Settings = GetDefault<URealitySuspicionSettings>();
	float BaseSuspicion = 0.0f;
	if (!CheatEvent.CheatTag.IsValid() || !Settings || !Settings->FindBaseSuspicion(CheatEvent.CheatTag, BaseSuspicion))
	{
		UE_LOG(LogReality, Warning, TEXT("Reality Manager: Ignored event for '%s' because Cheat tag '%s' has no configured Suspicion rule."), *GetNameSafe(CheatEvent.TargetActor), *CheatEvent.CheatTag.ToString());
		return false;
	}

	const float SuspicionBefore = CurrentSuspicion;
	const float RequestedDelta = CheatEvent.Operation == ERealityCheatOperation::Apply
		? BaseSuspicion
		: -FMath::Max(0.0f, Settings->RestoreSuspicionReduction);
	const float SuspicionAfter = FMath::Clamp(SuspicionBefore + RequestedDelta, 0.0f, 100.0f);
	const float AppliedDelta = SuspicionAfter - SuspicionBefore;
	const ERealityState PreviousState = CurrentRealityState;

	CurrentSuspicion = SuspicionAfter;
	CurrentRealityState = CalculateRealityState(CurrentSuspicion);
	AddHistoryRecord(CheatEvent, AppliedDelta, SuspicionBefore, SuspicionAfter);

	if (!FMath::IsNearlyEqual(SuspicionBefore, SuspicionAfter))
	{
		OnRealitySuspicionChanged.Broadcast(SuspicionBefore, SuspicionAfter);
	}
	if (PreviousState != CurrentRealityState)
	{
		OnRealityStateChanged.Broadcast(PreviousState, CurrentRealityState);
	}

	UE_LOG(LogReality, Log, TEXT("Reality Manager: %s %s Target='%s' Delta=%+.1f Suspicion=%.1f->%.1f State=%s."),
		*CheatEvent.CheatTag.ToString(),
		CheatEvent.Operation == ERealityCheatOperation::Apply ? TEXT("Apply") : TEXT("Restore"),
		*GetNameSafe(CheatEvent.TargetActor),
		AppliedDelta,
		SuspicionBefore,
		SuspicionAfter,
		*StaticEnum<ERealityState>()->GetNameStringByValue(static_cast<int64>(CurrentRealityState)));
	return true;
}

const FRealityProcessedCheatRecord* URealityManagerSubsystem::GetMostRecentEvent() const
{
	return EventHistory.IsEmpty() ? nullptr : &EventHistory.Last();
}

void URealityManagerSubsystem::ResetRealityState()
{
	const float PreviousSuspicion = CurrentSuspicion;
	const ERealityState PreviousState = CurrentRealityState;
	CurrentSuspicion = 0.0f;
	CurrentRealityState = ERealityState::Stable;
	EventHistory.Reset();

	if (!FMath::IsNearlyZero(PreviousSuspicion))
	{
		OnRealitySuspicionChanged.Broadcast(PreviousSuspicion, CurrentSuspicion);
	}
	if (PreviousState != CurrentRealityState)
	{
		OnRealityStateChanged.Broadcast(PreviousState, CurrentRealityState);
	}
	UE_LOG(LogReality, Log, TEXT("Reality Manager: Reset Suspicion and cleared event history."));
}

ERealityState URealityManagerSubsystem::CalculateRealityState(const float Suspicion) const
{
	const URealitySuspicionSettings* Settings = GetDefault<URealitySuspicionSettings>();
	if (!Settings)
	{
		return ERealityState::Stable;
	}

	const float Questioning = FMath::Clamp(Settings->QuestioningThreshold, 0.0f, 100.0f);
	const float Investigating = FMath::Clamp(Settings->InvestigatingThreshold, Questioning, 100.0f);
	const float Unstable = FMath::Clamp(Settings->UnstableThreshold, Investigating, 100.0f);
	const float Correction = FMath::Clamp(Settings->CorrectionThreshold, Unstable, 100.0f);
	if (Suspicion >= Correction)
	{
		return ERealityState::Correction;
	}
	if (Suspicion >= Unstable)
	{
		return ERealityState::Unstable;
	}
	if (Suspicion >= Investigating)
	{
		return ERealityState::Investigating;
	}
	if (Suspicion >= Questioning)
	{
		return ERealityState::Questioning;
	}
	return ERealityState::Stable;
}

void URealityManagerSubsystem::AddHistoryRecord(const FRealityCheatEvent& CheatEvent, const float Delta, const float Before, const float After)
{
	FRealityProcessedCheatRecord& Record = EventHistory.Emplace_GetRef();
	Record.TargetActorName = GetNameSafe(CheatEvent.TargetActor);
	Record.CheatTag = CheatEvent.CheatTag;
	Record.Operation = CheatEvent.Operation;
	Record.SuspicionDelta = Delta;
	Record.SuspicionBefore = Before;
	Record.SuspicionAfter = After;
	Record.ResultingState = CurrentRealityState;

	const URealitySuspicionSettings* Settings = GetDefault<URealitySuspicionSettings>();
	const int32 MaxHistory = FMath::Max(1, Settings ? Settings->MaxEventHistory : 32);
	if (EventHistory.Num() > MaxHistory)
	{
		EventHistory.RemoveAt(0, EventHistory.Num() - MaxHistory, EAllowShrinking::No);
	}
}
