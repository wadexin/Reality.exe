// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "GameplayTagContainer.h"
#include "RealitySuspicionSettings.generated.h"

/** One configurable base-Suspicion rule keyed by an exact Cheat Gameplay Tag. */
USTRUCT(BlueprintType)
struct REALITY_API FRealityCheatSuspicionRule
{
	GENERATED_BODY()

	FRealityCheatSuspicionRule() = default;
	FRealityCheatSuspicionRule(const FGameplayTag InCheatTag, const float InSuspicionValue)
		: CheatTag(InCheatTag)
		, SuspicionValue(InSuspicionValue)
	{
	}

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Suspicion", meta = (Categories = "Cheat"))
	FGameplayTag CheatTag;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Suspicion", meta = (ClampMin = "0.0", ClampMax = "100.0"))
	float SuspicionValue = 0.0f;
};

/** Lightweight project settings for prototype Suspicion rules, thresholds, and debug-history capacity. */
UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "Reality Suspicion"))
class REALITY_API URealitySuspicionSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	URealitySuspicionSettings();

	/** Finds the exact configured rule for a Cheat tag. */
	bool FindBaseSuspicion(FGameplayTag CheatTag, float& OutSuspicionValue) const;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Suspicion")
	TArray<FRealityCheatSuspicionRule> CheatSuspicionRules;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Suspicion", meta = (ClampMin = "0.0", ClampMax = "100.0"))
	float RestoreSuspicionReduction = 5.0f;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "State Thresholds", meta = (ClampMin = "0.0", ClampMax = "100.0"))
	float QuestioningThreshold = 20.0f;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "State Thresholds", meta = (ClampMin = "0.0", ClampMax = "100.0"))
	float InvestigatingThreshold = 40.0f;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "State Thresholds", meta = (ClampMin = "0.0", ClampMax = "100.0"))
	float UnstableThreshold = 60.0f;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "State Thresholds", meta = (ClampMin = "0.0", ClampMax = "100.0"))
	float CorrectionThreshold = 80.0f;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Debug", meta = (ClampMin = "1", ClampMax = "256"))
	int32 MaxEventHistory = 32;
};
