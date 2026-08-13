// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/Interactable.h"
#include "P0ContextToggleSwitch.generated.h"

class URealityContextComponent;
class UStaticMeshComponent;
class UTextRenderComponent;

/** Map-facing prototype switch that toggles one existing Context source without changing Suspicion itself. */
UCLASS(Blueprintable)
class REALITY_API AP0ContextToggleSwitch : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	AP0ContextToggleSwitch();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UTextRenderComponent> StatusText;

	/** Actor containing the Context component controlled by this map switch. */
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Prototype|Context")
	TObjectPtr<AActor> ContextSourceActor;

	/** Assigns the map-local Context source and refreshes switch feedback. */
	UFUNCTION(BlueprintCallable, Category = "Prototype|Context")
	void SetContextSourceActor(AActor* NewContextSourceActor);

	UFUNCTION(BlueprintPure, Category = "Prototype|Context")
	URealityContextComponent* GetTargetContextComponent() const;

	virtual bool CanInteract_Implementation(AActor* Interactor) const override;
	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual FText GetInteractionPrompt_Implementation(AActor* Interactor) const override;

protected:
	virtual void BeginPlay() override;

private:
	void RefreshStatusFeedback();
};
