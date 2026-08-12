// Copyright Epic Games, Inc. All Rights Reserved.

#include "Interaction/Interactable.h"

bool IInteractable::CanInteract_Implementation(AActor* Interactor) const
{
	return true;
}

void IInteractable::Interact_Implementation(AActor* Interactor)
{
}

FText IInteractable::GetInteractionPrompt_Implementation(AActor* Interactor) const
{
	return NSLOCTEXT("RealityInteraction", "DefaultInteractionPrompt", "Interact");
}
