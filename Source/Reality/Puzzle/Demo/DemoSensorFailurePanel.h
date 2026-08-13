// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/Interactable.h"
#include "DemoSensorFailurePanel.generated.h"

class URealityContextComponent;
class UStaticMeshComponent;
class UTextRenderComponent;
class UAudioComponent;
class USoundBase;

/** Interactable maintenance infrastructure that owns and visibly toggles one SensorFailure Context source. */
UCLASS(Blueprintable)
class REALITY_API ADemoSensorFailurePanel : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	ADemoSensorFailurePanel();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> PanelMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UTextRenderComponent> StatusText;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<URealityContextComponent> ContextComponent;

	/** Spatial infrastructure bed whose tone follows ONLINE versus FAILURE ACTIVE. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UAudioComponent> InfrastructureAudio;

	UFUNCTION(BlueprintPure, Category = "Demo|Context|Audio")
	float GetInfrastructurePitch() const;

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Demo|Context")
	void ConfigureSensorFailure(float EffectRange, bool bStartActive = false);

	virtual bool CanInteract_Implementation(AActor* Interactor) const override;
	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual FText GetInteractionPrompt_Implementation(AActor* Interactor) const override;

protected:
	virtual void BeginPlay() override;

private:
	void RefreshStatusFeedback();
	void RefreshAudioFeedback(bool bPlayToggleCue);
	UPROPERTY() TObjectPtr<USoundBase> ToggleSound;
};
