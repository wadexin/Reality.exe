// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/Interactable.h"
#include "RealitySystem/RealityManagerSubsystem.h"
#include "DemoExitTerminal.generated.h"

class UStaticMeshComponent;
class UTextRenderComponent;

/** Non-editable, one-shot Demo completion terminal that reports the final Reality outcome. */
UCLASS(Blueprintable)
class REALITY_API ADemoExitTerminal : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	ADemoExitTerminal();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> TerminalMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UTextRenderComponent> TerminalLabel;

	UFUNCTION(BlueprintPure, Category = "Demo|Exit")
	bool HasCompletedDemo() const { return bDemoCompleted; }

	UFUNCTION(BlueprintPure, Category = "Demo|Exit")
	float GetFinalSuspicion() const { return FinalSuspicion; }

	UFUNCTION(BlueprintPure, Category = "Demo|Exit")
	ERealityState GetFinalRealityState() const { return FinalRealityState; }

	/** Completes once for a valid Pawn and is shared by interaction and focused automation. */
	UFUNCTION(BlueprintCallable, Category = "Demo|Exit")
	bool CompleteDemo(AActor* CompletingActor);

	virtual bool CanInteract_Implementation(AActor* Interactor) const override;
	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual FText GetInteractionPrompt_Implementation(AActor* Interactor) const override;

private:
	UPROPERTY(Transient)
	bool bDemoCompleted = false;

	UPROPERTY(Transient)
	float FinalSuspicion = 0.0f;

	UPROPERTY(Transient)
	ERealityState FinalRealityState = ERealityState::Stable;
};
