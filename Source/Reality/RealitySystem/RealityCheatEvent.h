// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "RealityCheatEvent.generated.h"

class AActor;

/** Describes whether a Reality cheat operation applies or restores a property state. */
UENUM(BlueprintType)
enum class ERealityCheatOperation : uint8
{
	Apply,
	Restore
};

/**
 * Semantic message describing a Reality modification occurrence.
 * Future property implementations emit this after a successful Apply or Restore operation.
 */
USTRUCT(BlueprintType)
struct REALITY_API FRealityCheatEvent
{
	GENERATED_BODY()

	FRealityCheatEvent() = default;

	FRealityCheatEvent(
		AActor* InTargetActor,
		const FGameplayTag InCheatTag,
		AActor* InInstigatingActor,
		const ERealityCheatOperation InOperation)
		: TargetActor(InTargetActor)
		, InstigatingActor(InInstigatingActor)
		, CheatTag(InCheatTag)
		, Operation(InOperation)
	{
	}

	/** Actor whose Reality-editable property was affected. */
	UPROPERTY(BlueprintReadOnly, Category = "Reality|Cheat Event")
	TObjectPtr<AActor> TargetActor = nullptr;

	/** Actor responsible for requesting the operation, when applicable. */
	UPROPERTY(BlueprintReadOnly, Category = "Reality|Cheat Event")
	TObjectPtr<AActor> InstigatingActor = nullptr;

	/** Semantic cheat capability that identifies the affected property. */
	UPROPERTY(BlueprintReadOnly, Category = "Reality|Cheat Event", meta = (Categories = "Cheat"))
	FGameplayTag CheatTag;

	/** Whether the operation applied a modification or restored its prior state. */
	UPROPERTY(BlueprintReadOnly, Category = "Reality|Cheat Event")
	ERealityCheatOperation Operation = ERealityCheatOperation::Apply;
};
