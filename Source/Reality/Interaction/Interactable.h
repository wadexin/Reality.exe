// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interactable.generated.h"

/**
 * Marks an actor as a generic interaction target.
 * Player-owned interaction components call this interface after acquiring focus.
 */
UINTERFACE(BlueprintType)
class REALITY_API UInteractable : public UInterface
{
	GENERATED_BODY()
};

/** Generic interaction contract implemented by C++ actors or Blueprint classes. */
class REALITY_API IInteractable
{
	GENERATED_BODY()

public:
	/** Returns whether Interactor may interact with this object at the time of the call. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction")
	bool CanInteract(AActor* Interactor) const;
	virtual bool CanInteract_Implementation(AActor* Interactor) const;

	/** Executes this object's interaction behavior for Interactor. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction")
	void Interact(AActor* Interactor);
	virtual void Interact_Implementation(AActor* Interactor);

	/** Returns the short prompt text to present for Interactor. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction")
	FText GetInteractionPrompt(AActor* Interactor) const;
	virtual FText GetInteractionPrompt_Implementation(AActor* Interactor) const;
};
