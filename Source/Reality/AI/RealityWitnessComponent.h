// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "RealitySystem/RealityCheatEvent.h"
#include "RealityWitnessComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRealityEventObservedSignature, const FRealityCheatEvent&, CheatEvent);

/**
 * Passive event-driven observer attached to a Human or Camera Actor.
 * It registers with the world Reality Manager and evaluates range, FOV, and optional LOS only when an Apply event occurs.
 */
UCLASS(ClassGroup = (Reality), BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class REALITY_API URealityWitnessComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	URealityWitnessComponent();

	/** Witness category used for configurable evidence tuning. Task 008 supports Human and Camera. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reality|Witness", meta = (Categories = "Witness"))
	FGameplayTag WitnessType;

	/** Disabled Witnesses remain registered safely but cannot observe or notify. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reality|Witness")
	bool bWitnessEnabled = true;

	/** Maximum observation distance in centimeters. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reality|Witness", meta = (ClampMin = "0.0", Units = "cm"))
	float ObservationRange = 1500.0f;

	/** Total horizontal field of view in degrees. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reality|Witness", meta = (ClampMin = "0.0", ClampMax = "360.0", Units = "deg"))
	float FieldOfViewDegrees = 90.0f;

	/** Requires an unobstructed visibility segment to the target bounds center. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reality|Witness")
	bool bRequireLineOfSight = true;

	/** Broadcast exactly once when this Witness successfully observes an Apply event. */
	UPROPERTY(BlueprintAssignable, Category = "Reality|Witness|Events")
	FRealityEventObservedSignature OnRealityEventObserved;

	/** Evaluates this Witness against one event without modifying Suspicion. Expected caller is Reality Manager. */
	bool CanObserveRealityEvent(const FRealityCheatEvent& CheatEvent) const;

	/** Notifies Blueprint/C++ listeners after the Manager accepts this Witness as an observer. */
	void NotifyRealityEventObserved(const FRealityCheatEvent& CheatEvent);

	UFUNCTION(BlueprintPure, Category = "Reality|Witness")
	bool IsWitnessEnabled() const { return bWitnessEnabled; }

	UFUNCTION(BlueprintPure, Category = "Reality|Witness")
	FGameplayTag GetWitnessType() const { return WitnessType; }

	/** Returns the current deterministic origin and forward direction used for observation. */
	bool GetObservationViewpoint(FVector& OutOrigin, FVector& OutForward) const;

	/** Returns the deterministic bounds-center target point used by Witness evaluation. */
	FVector GetObservationTargetPoint(const AActor* TargetActor) const;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
