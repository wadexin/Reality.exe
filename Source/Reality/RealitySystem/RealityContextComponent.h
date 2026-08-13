// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "RealityContextComponent.generated.h"

/**
 * Non-physical environmental evidence source evaluated by the world Reality Manager at event time.
 * The owning Actor supplies the spatial origin; external gameplay owns activation and duration.
 */
UCLASS(ClassGroup = (Reality), BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class REALITY_API URealityContextComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	URealityContextComponent();

	/** Semantic environmental conditions supplied by this source. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reality|Context", meta = (Categories = "Context"))
	FGameplayTagContainer ContextTags;

	/** Whether this source is currently available to explain new Reality events. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reality|Context")
	bool bContextActive = true;

	/** Maximum distance in centimeters from this source to an event target. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reality|Context", meta = (ClampMin = "0.0", Units = "cm"))
	float EffectRange = 1000.0f;

	/** Activates or deactivates future event evaluation without changing Suspicion. */
	UFUNCTION(BlueprintCallable, Category = "Reality|Context")
	void SetContextActive(bool bActive);

	UFUNCTION(BlueprintPure, Category = "Reality|Context")
	bool IsContextActive() const { return bContextActive; }

	UFUNCTION(BlueprintPure, Category = "Reality|Context")
	FGameplayTagContainer GetContextTags() const { return ContextTags; }

	UFUNCTION(BlueprintPure, Category = "Reality|Context")
	float GetEffectRange() const { return EffectRange; }

	/** Returns whether the target bounds center is within range; no LOS is involved. */
	UFUNCTION(BlueprintPure, Category = "Reality|Context")
	bool IsTargetWithinContext(const AActor* TargetActor) const;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	FVector GetContextOrigin() const;
	FVector GetTargetPoint(const AActor* TargetActor) const;
};
