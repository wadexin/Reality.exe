// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "RealitySystem/RealityCheatEvent.h"
#include "RealityManagerSubsystem.generated.h"

/** Global prototype escalation states derived exclusively from current Reality Suspicion. */
UENUM(BlueprintType)
enum class ERealityState : uint8
{
	Stable,
	Questioning,
	Investigating,
	Unstable,
	Correction
};

/** Lightweight bounded debug record describing one successfully processed Reality cheat event. */
USTRUCT(BlueprintType)
struct REALITY_API FRealityProcessedCheatRecord
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Reality|History")
	FString TargetActorName;

	UPROPERTY(BlueprintReadOnly, Category = "Reality|History", meta = (Categories = "Cheat"))
	FGameplayTag CheatTag;

	UPROPERTY(BlueprintReadOnly, Category = "Reality|History")
	ERealityCheatOperation Operation = ERealityCheatOperation::Apply;

	UPROPERTY(BlueprintReadOnly, Category = "Reality|History")
	float SuspicionDelta = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Reality|History")
	float SuspicionBefore = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Reality|History")
	float SuspicionAfter = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Reality|History")
	ERealityState ResultingState = ERealityState::Stable;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRealitySuspicionChangedSignature, float, OldValue, float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRealityStateChangedSignature, ERealityState, OldState, ERealityState, NewState);

/**
 * World-scoped authority for global Reality Suspicion, derived state, and recent processed-event history.
 * Gameplay systems submit semantic FRealityCheatEvent values; this subsystem owns all global evaluation state.
 */
UCLASS()
class REALITY_API URealityManagerSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	/** Processes one known Cheat event and returns whether it had a configured rule. */
	UFUNCTION(BlueprintCallable, Category = "Reality")
	bool ProcessCheatEvent(const FRealityCheatEvent& CheatEvent);

	UFUNCTION(BlueprintPure, Category = "Reality")
	float GetSuspicion() const { return CurrentSuspicion; }

	UFUNCTION(BlueprintPure, Category = "Reality")
	ERealityState GetRealityState() const { return CurrentRealityState; }

	/** Returns a copy of the bounded history for safe Blueprint inspection. */
	UFUNCTION(BlueprintPure, Category = "Reality|Debug")
	TArray<FRealityProcessedCheatRecord> GetEventHistory() const { return EventHistory; }

	/** Returns the latest processed event without exposing mutable history storage. */
	const FRealityProcessedCheatRecord* GetMostRecentEvent() const;

	/** Clears Suspicion and history, returning state to Stable for development and tests. */
	UFUNCTION(BlueprintCallable, Category = "Reality|Debug")
	void ResetRealityState();

	UPROPERTY(BlueprintAssignable, Category = "Reality|Events")
	FRealitySuspicionChangedSignature OnRealitySuspicionChanged;

	UPROPERTY(BlueprintAssignable, Category = "Reality|Events")
	FRealityStateChangedSignature OnRealityStateChanged;

private:
	ERealityState CalculateRealityState(float Suspicion) const;
	void AddHistoryRecord(const FRealityCheatEvent& CheatEvent, float Delta, float Before, float After);

	UPROPERTY(Transient)
	float CurrentSuspicion = 0.0f;

	UPROPERTY(Transient)
	ERealityState CurrentRealityState = ERealityState::Stable;

	UPROPERTY(Transient)
	TArray<FRealityProcessedCheatRecord> EventHistory;
};
