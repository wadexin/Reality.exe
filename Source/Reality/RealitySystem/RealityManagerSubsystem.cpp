// Copyright Epic Games, Inc. All Rights Reserved.

#include "RealitySystem/RealityManagerSubsystem.h"

#include "AI/RealityWitnessComponent.h"
#include "Reality.h"
#include "RealitySystem/RealitySuspicionSettings.h"

void URealityManagerSubsystem::RegisterWitness(URealityWitnessComponent* WitnessComponent)
{
	if (IsValid(WitnessComponent) && WitnessComponent->GetWorld() == GetWorld())
	{
		RegisteredWitnesses.AddUnique(WitnessComponent);
	}
}

void URealityManagerSubsystem::UnregisterWitness(URealityWitnessComponent* WitnessComponent)
{
	RegisteredWitnesses.RemoveAllSwap(
		[WitnessComponent](const TWeakObjectPtr<URealityWitnessComponent>& RegisteredWitness)
		{
			return !RegisteredWitness.IsValid() || RegisteredWitness.Get() == WitnessComponent;
		},
		EAllowShrinking::No);
}

int32 URealityManagerSubsystem::GetRegisteredWitnessCount()
{
	RegisteredWitnesses.RemoveAllSwap(
		[](const TWeakObjectPtr<URealityWitnessComponent>& RegisteredWitness)
		{
			return !RegisteredWitness.IsValid();
		},
		EAllowShrinking::No);
	return RegisteredWitnesses.Num();
}

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

	const float BaseDelta = CheatEvent.Operation == ERealityCheatOperation::Apply
		? BaseSuspicion
		: -FMath::Max(0.0f, Settings->RestoreSuspicionReduction);
	TArray<TWeakObjectPtr<URealityWitnessComponent>> ObservingWitnesses;
	const float WitnessDelta = CheatEvent.Operation == ERealityCheatOperation::Apply
		? EvaluateWitnesses(CheatEvent, ObservingWitnesses)
		: 0.0f;
	const float SuspicionBefore = CurrentSuspicion;
	const float RequestedDelta = BaseDelta + WitnessDelta;
	const float SuspicionAfter = FMath::Clamp(SuspicionBefore + RequestedDelta, 0.0f, 100.0f);
	const float AppliedDelta = SuspicionAfter - SuspicionBefore;
	const ERealityState PreviousState = CurrentRealityState;

	CurrentSuspicion = SuspicionAfter;
	CurrentRealityState = CalculateRealityState(CurrentSuspicion);
	AddHistoryRecord(CheatEvent, BaseDelta, WitnessDelta, ObservingWitnesses.Num(), AppliedDelta, SuspicionBefore, SuspicionAfter);

	for (const TWeakObjectPtr<URealityWitnessComponent>& ObservingWitness : ObservingWitnesses)
	{
		if (URealityWitnessComponent* WitnessComponent = ObservingWitness.Get())
		{
			WitnessComponent->NotifyRealityEventObserved(CheatEvent);
		}
	}

	if (!FMath::IsNearlyEqual(SuspicionBefore, SuspicionAfter))
	{
		OnRealitySuspicionChanged.Broadcast(SuspicionBefore, SuspicionAfter);
	}
	if (PreviousState != CurrentRealityState)
	{
		OnRealityStateChanged.Broadcast(PreviousState, CurrentRealityState);
	}

	UE_LOG(LogReality, Log, TEXT("Reality Manager: %s %s Target='%s' Base=%+.1f Witness=%+.1f Observers=%d Delta=%+.1f Suspicion=%.1f->%.1f State=%s."),
		*CheatEvent.CheatTag.ToString(),
		CheatEvent.Operation == ERealityCheatOperation::Apply ? TEXT("Apply") : TEXT("Restore"),
		*GetNameSafe(CheatEvent.TargetActor),
		BaseDelta,
		WitnessDelta,
		ObservingWitnesses.Num(),
		AppliedDelta,
		SuspicionBefore,
		SuspicionAfter,
		*StaticEnum<ERealityState>()->GetNameStringByValue(static_cast<int64>(CurrentRealityState)));
	return true;
}

float URealityManagerSubsystem::EvaluateWitnesses(
	const FRealityCheatEvent& CheatEvent,
	TArray<TWeakObjectPtr<URealityWitnessComponent>>& OutObservingWitnesses)
{
	const URealitySuspicionSettings* Settings = GetDefault<URealitySuspicionSettings>();
	if (!Settings || !IsValid(CheatEvent.TargetActor))
	{
		return 0.0f;
	}

	float RawWitnessBonus = 0.0f;
	for (int32 WitnessIndex = RegisteredWitnesses.Num() - 1; WitnessIndex >= 0; --WitnessIndex)
	{
		URealityWitnessComponent* WitnessComponent = RegisteredWitnesses[WitnessIndex].Get();
		if (!IsValid(WitnessComponent) || WitnessComponent->GetWorld() != GetWorld())
		{
			RegisteredWitnesses.RemoveAtSwap(WitnessIndex, 1, EAllowShrinking::No);
			continue;
		}

		float WitnessSuspicion = 0.0f;
		if (!Settings->FindWitnessSuspicion(WitnessComponent->GetWitnessType(), WitnessSuspicion))
		{
			if (WitnessComponent->IsWitnessEnabled())
			{
				UE_LOG(LogReality, Warning, TEXT("Reality Manager: Witness '%s' has unconfigured type '%s' and contributes no evidence."), *GetNameSafe(WitnessComponent->GetOwner()), *WitnessComponent->GetWitnessType().ToString());
			}
			continue;
		}

		if (WitnessComponent->CanObserveRealityEvent(CheatEvent))
		{
			OutObservingWitnesses.Add(WitnessComponent);
			RawWitnessBonus += WitnessSuspicion;
		}
	}

	return FMath::Min(RawWitnessBonus, FMath::Max(0.0f, Settings->MaximumWitnessBonusPerEvent));
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

void URealityManagerSubsystem::AddHistoryRecord(
	const FRealityCheatEvent& CheatEvent,
	const float BaseDelta,
	const float WitnessDelta,
	const int32 ObserverCount,
	const float AppliedDelta,
	const float Before,
	const float After)
{
	FRealityProcessedCheatRecord& Record = EventHistory.Emplace_GetRef();
	Record.TargetActorName = GetNameSafe(CheatEvent.TargetActor);
	Record.CheatTag = CheatEvent.CheatTag;
	Record.Operation = CheatEvent.Operation;
	Record.SuspicionDelta = AppliedDelta;
	Record.BaseSuspicionDelta = BaseDelta;
	Record.WitnessSuspicionDelta = WitnessDelta;
	Record.ObservingWitnessCount = ObserverCount;
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
