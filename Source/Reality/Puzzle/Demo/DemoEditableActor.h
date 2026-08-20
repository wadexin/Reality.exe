// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/Interactable.h"
#include "DemoEditableActor.generated.h"

class URealityEditableComponent;
class UStaticMeshComponent;
class UTextRenderComponent;
class UDemoRecoverableComponent;

/** Graybox-ready Reality target with explicit capabilities and an optional physical push interaction. */
UCLASS(Blueprintable)
class REALITY_API ADemoEditableActor : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	ADemoEditableActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UTextRenderComponent> EditableMarker;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<URealityEditableComponent> EditableComponent;

	/** Enabled only for authored simulated critical props; static puzzle structure is never auto-reset. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UDemoRecoverableComponent> RecoverableComponent;

	/** Configures one placed Demo target without exposing string-based capability logic to the level. */
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Demo|Editable")
	void ConfigureDemoTarget(
		FText DisplayName,
		bool bCollision,
		bool bScale,
		bool bGravity,
		bool bMass,
		bool bFriction,
		bool bTime,
		bool bSimulatePhysics,
		bool bDoor,
		bool bEnablePushInteraction);

	/** Physical impulse used by the optional E push affordance; ordinary editable barriers leave this disabled. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Demo|Editable", meta = (ClampMin = "0.0"))
	float PushImpulse = 30000.0f;

	virtual bool CanInteract_Implementation(AActor* Interactor) const override;
	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual FText GetInteractionPrompt_Implementation(AActor* Interactor) const override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleInstanceOnly, Category = "Demo|Editable")
	FText TargetDisplayName;

	UPROPERTY(VisibleInstanceOnly, Category = "Demo|Editable")
	bool bPushInteractionEnabled = false;
};
