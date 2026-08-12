// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionComponent.generated.h"

class AActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractionFocusGainedSignature, AActor*, FocusedActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractionFocusLostSignature, AActor*, LostActor);

/**
 * Player-owned component that traces from the owning actor's viewpoint and manages one focused interactable.
 * The component performs one lightweight trace per enabled Tick and can be disabled when focus detection is unnecessary.
 */
UCLASS(ClassGroup = (Interaction), BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class REALITY_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInteractionComponent();

	/** Maximum distance, in centimeters, used for the interaction trace. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction", meta = (ClampMin = "0.0", Units = "cm"))
	float InteractionDistance = 300.0f;

	/** Collision channel used to find interaction targets. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Visibility;

	/** Whether continuous focus tracing should run. Disabling this also clears focus. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	bool bFocusTracingEnabled = true;

	/** Draws the interaction trace in non-shipping development builds. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|Debug")
	bool bDrawDebugTrace = false;

	/** Broadcast once when a valid interactable gains focus. */
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FInteractionFocusGainedSignature OnFocusGained;

	/** Broadcast once when the current interactable loses focus. */
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FInteractionFocusLostSignature OnFocusLost;

	/** Attempts to interact with the currently focused actor. Returns true when interaction executes. */
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	bool InteractWithFocusedActor();

	/** Enables or disables continuous focus tracing. Disabling clears the current focus. */
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void SetFocusTracingEnabled(bool bEnabled);

	/** Immediately performs one focus trace, independent of Tick enablement. */
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void UpdateFocus();

	/** Returns the currently focused interactable actor, or null. */
	UFUNCTION(BlueprintPure, Category = "Interaction")
	AActor* GetFocusedActor() const { return FocusedActor.Get(); }

	/** Returns the focused actor's current prompt, or empty text when interaction is unavailable. */
	UFUNCTION(BlueprintPure, Category = "Interaction")
	FText GetFocusedInteractionPrompt() const;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	/** Updates focus and emits transition events only when the focused actor changes. */
	void SetFocusedActor(AActor* NewFocusedActor);

	/** Clears focus when the focused actor is destroyed. */
	UFUNCTION()
	void HandleFocusedActorDestroyed(AActor* DestroyedActor);

	/** Traces from the owner's active viewpoint and returns an allowed interactable, or null. */
	AActor* FindInteractableFromViewpoint() const;

	UPROPERTY(Transient)
	TWeakObjectPtr<AActor> FocusedActor;
};
