// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/Interactable.h"
#include "InteractionTestActor.generated.h"

class UStaticMeshComponent;

/** Minimal concrete interactable used to validate focus and execution during prototype development. */
UCLASS(Blueprintable)
class REALITY_API AInteractionTestActor : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	AInteractionTestActor();

	/** Visible collision-bearing mesh used by the interaction trace. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Mesh;

	/** Allows test levels to exercise permitted and denied interaction states. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	bool bInteractionAllowed = true;

	/** Prompt returned through IInteractable. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	FText InteractionPrompt;

	/** Number of successful interactions received by this actor instance. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Interaction")
	int32 InteractionCount = 0;

	/** Number of focus-gained delegate calls observed during validation. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Interaction|Validation")
	int32 FocusGainedCount = 0;

	/** Number of focus-lost delegate calls observed during validation. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Interaction|Validation")
	int32 FocusLostCount = 0;

	virtual bool CanInteract_Implementation(AActor* Interactor) const override;
	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual FText GetInteractionPrompt_Implementation(AActor* Interactor) const override;

	/** Validation callback for UInteractionComponent::OnFocusGained. */
	UFUNCTION()
	void HandleFocusGained(AActor* FocusedActor);

	/** Validation callback for UInteractionComponent::OnFocusLost. */
	UFUNCTION()
	void HandleFocusLost(AActor* UnfocusedActor);

protected:
	/** Blueprint hook invoked after the native test interaction counter and log are updated. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Interaction", meta = (DisplayName = "Test Actor Interacted"))
	void BP_OnInteracted(AActor* Interactor, int32 NewInteractionCount);
};
