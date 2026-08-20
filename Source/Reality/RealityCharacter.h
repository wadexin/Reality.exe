// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "RealityCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
class UInteractionComponent;
class UDeveloperModeComponent;
class UDeveloperTargetPresentationComponent;
class UDeveloperAudioFeedbackComponent;
class UDemoPlayerRecoveryComponent;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 *  A basic first person character
 */
UCLASS(abstract)
class ARealityCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: first person view (arms; seen only by self) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* FirstPersonMesh;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	/** Reusable viewpoint-based interaction component. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UInteractionComponent* InteractionComponent;

	/** Player-side Developer Mode state, focus, and prototype cheat shell. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UDeveloperModeComponent* DeveloperModeComponent;

	/** Render-only feedback for the frozen Developer Mode target. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UDeveloperTargetPresentationComponent* DeveloperTargetPresentationComponent;

	/** Restrained event-driven audio for Developer Mode and accepted Reality edits. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UDeveloperAudioFeedbackComponent* DeveloperAudioFeedbackComponent;

	/** Latest authored safe location used by Demo out-of-bounds recovery. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UDemoPlayerRecoveryComponent* DemoPlayerRecoveryComponent;

protected:

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	class UInputAction* LookAction;

	/** Mouse Look Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	class UInputAction* MouseLookAction;

	/** Interaction Input Action */
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* InteractionAction;

	/** Input Mapping Context containing the default interaction key mapping. */
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputMappingContext* InteractionMappingContext;

	/** Toggles the minimal Developer Mode shell. */
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* DeveloperModeAction;

	/** Invokes the temporary focused Collision Apply/Restore control. */
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* DeveloperCollisionAction;

	/** Input Mapping Context containing F6 Developer Mode and R Collision prototype bindings. */
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputMappingContext* DeveloperMappingContext;

	/** Prototype Scale preset inputs ordered 0.25x, 0.5x, 1.0x, 2.0x, and 4.0x. */
	UPROPERTY(EditAnywhere, Category = "Input")
	TArray<TObjectPtr<UInputAction>> DeveloperScaleActions;

	/** Restores the focused Actor's active Scale cycle. */
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* DeveloperScaleRestoreAction;

	/** Cycles the focused Actor's Gravity preset. */
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* DeveloperGravityCycleAction;

	/** Restores the focused Actor's Gravity cycle. */
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* DeveloperGravityRestoreAction;
	
public:
	/** Adds the interaction mapping context for the locally controlled player. */
	virtual void PawnClientRestart() override;

	ARealityCharacter();

protected:
	/** Rebuilds per-Pawn mappings so cooked Blueprint instances reference their runtime Input Actions. */
	void RebuildRuntimeInputMappings();

	/** Called from Input Actions for movement input */
	void MoveInput(const FInputActionValue& Value);

	/** Called from Input Actions for looking input */
	void LookInput(const FInputActionValue& Value);

	/** Handles aim inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoAim(float Yaw, float Pitch);

	/** Handles move inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	/** Handles jump start inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpStart();

	/** Handles jump end inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpEnd();

	/** Attempts to interact with the actor currently focused by the interaction component. */
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoInteract();

	/** Toggles the player-owned Developer Mode component. */
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoToggleDeveloperMode();

	/** Invokes the focused Developer Mode Collision prototype action. */
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoToggleDeveloperCollision();

	void DoApplyDeveloperScaleQuarter();
	void DoApplyDeveloperScaleHalf();
	void DoApplyDeveloperScaleOne();
	void DoApplyDeveloperScaleDouble();
	void DoApplyDeveloperScaleQuadruple();
	void DoRestoreDeveloperScale();
	void DoCycleDeveloperGravity();
	void DoRestoreDeveloperGravity();

protected:

	/** Set up input action bindings */
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	

public:

	/** Returns the first person mesh **/
	USkeletalMeshComponent* GetFirstPersonMesh() const { return FirstPersonMesh; }

	/** Returns first person camera component **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	/** Returns the player interaction component. */
	UInteractionComponent* GetInteractionComponent() const { return InteractionComponent; }

	/** Returns the Enhanced Input action used for interaction. */
	UInputAction* GetInteractionAction() const { return InteractionAction; }

	/** Returns the Enhanced Input mapping context containing the default interaction binding. */
	UInputMappingContext* GetInteractionMappingContext() const { return InteractionMappingContext; }

	/** Returns the player-owned Developer Mode component. */
	UDeveloperModeComponent* GetDeveloperModeComponent() const { return DeveloperModeComponent; }

	/** Returns the player-owned Developer Mode presentation layer. */
	UDeveloperTargetPresentationComponent* GetDeveloperTargetPresentationComponent() const { return DeveloperTargetPresentationComponent; }
	UDeveloperAudioFeedbackComponent* GetDeveloperAudioFeedbackComponent() const { return DeveloperAudioFeedbackComponent; }
	UDemoPlayerRecoveryComponent* GetDemoPlayerRecoveryComponent() const { return DemoPlayerRecoveryComponent; }

	/** Returns the Enhanced Input action used to toggle Developer Mode. */
	UInputAction* GetDeveloperModeAction() const { return DeveloperModeAction; }

	/** Returns the Enhanced Input action used for the temporary Collision control. */
	UInputAction* GetDeveloperCollisionAction() const { return DeveloperCollisionAction; }

	/** Returns the mapping context containing Developer Mode prototype controls. */
	UInputMappingContext* GetDeveloperMappingContext() const { return DeveloperMappingContext; }

	/** Returns the five Enhanced Input actions for the ordered Scale presets. */
	const TArray<TObjectPtr<UInputAction>>& GetDeveloperScaleActions() const { return DeveloperScaleActions; }

	/** Returns the Enhanced Input action used to restore Scale. */
	UInputAction* GetDeveloperScaleRestoreAction() const { return DeveloperScaleRestoreAction; }

	/** Returns the Enhanced Input action used to cycle Gravity. */
	UInputAction* GetDeveloperGravityCycleAction() const { return DeveloperGravityCycleAction; }

	/** Returns the Enhanced Input action used to restore Gravity. */
	UInputAction* GetDeveloperGravityRestoreAction() const { return DeveloperGravityRestoreAction; }

};

