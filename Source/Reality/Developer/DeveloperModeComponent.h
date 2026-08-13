// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Developer/RealityEditableComponent.h"
#include "RealitySystem/RealityCheatEvent.h"
#include "RealitySystem/RealityManagerSubsystem.h"
#include "DeveloperModeComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDeveloperFocusGainedSignature, AActor*, FocusedActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDeveloperFocusLostSignature, AActor*, LostActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDeveloperModeChangedSignature, bool, bIsActive);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDeveloperConsoleRefreshSignature);

/** Short-lived player-facing confirmation generated only by a successful Reality operation. */
UENUM(BlueprintType)
enum class EDeveloperOperationFeedback : uint8
{
	None,
	Applied,
	Restored
};

/**
 * Player-side shell for Developer Mode state, editable-object focus, and prototype cheat invocation.
 * Its lightweight view trace runs only while Developer Mode is active and remains independent of IInteractable.
 */
UCLASS(ClassGroup = (Reality), BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class REALITY_API UDeveloperModeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDeveloperModeComponent();

	/** Maximum distance, in centimeters, used to inspect Reality-editable Actors. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer Mode", meta = (ClampMin = "0.0", Units = "cm"))
	float InspectionDistance = 500.0f;

	/** Collision channel used for the developer target view trace. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer Mode")
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Visibility;

	/** Draws the inspection trace in non-shipping builds. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer Mode|Debug")
	bool bDrawDebugTrace = false;

	/** Shows the legacy engineering text dump instead of relying solely on the player-facing console. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer Mode|Debug")
	bool bShowEngineeringReadout = false;

	/** Broadcast after Developer Mode opens or closes. */
	UPROPERTY(BlueprintAssignable, Category = "Developer Mode|Events")
	FDeveloperModeChangedSignature OnDeveloperModeChanged;

	/** Broadcast after target or editable/Reality display data may have changed. */
	UPROPERTY(BlueprintAssignable, Category = "Developer Mode|Events")
	FDeveloperConsoleRefreshSignature OnDeveloperConsoleRefresh;

	/** Broadcast when a Reality-editable Actor becomes the inspected developer target. */
	UPROPERTY(BlueprintAssignable, Category = "Developer Mode|Focus")
	FDeveloperFocusGainedSignature OnDeveloperFocusGained;

	/** Broadcast when the current Reality-editable Actor is no longer inspected. */
	UPROPERTY(BlueprintAssignable, Category = "Developer Mode|Focus")
	FDeveloperFocusLostSignature OnDeveloperFocusLost;

	/** Activates Developer Mode and its target trace without pausing or changing player controls. */
	UFUNCTION(BlueprintCallable, Category = "Developer Mode")
	void EnterDeveloperMode();

	/** Deactivates Developer Mode, disables its trace, and clears developer focus. */
	UFUNCTION(BlueprintCallable, Category = "Developer Mode")
	void ExitDeveloperMode();

	/** Toggles Developer Mode and returns the new active state. */
	UFUNCTION(BlueprintCallable, Category = "Developer Mode")
	bool ToggleDeveloperMode();

	/** Returns whether Developer Mode and its view trace are active. */
	UFUNCTION(BlueprintPure, Category = "Developer Mode")
	bool IsDeveloperModeActive() const { return bDeveloperModeActive; }

	/** Immediately refreshes developer focus when Developer Mode is active. */
	UFUNCTION(BlueprintCallable, Category = "Developer Mode|Focus")
	void UpdateDeveloperFocus();

	/** Returns the currently inspected Reality-editable Actor, or null. */
	UFUNCTION(BlueprintPure, Category = "Developer Mode|Focus")
	AActor* GetFocusedDeveloperActor() const { return FocusedDeveloperActor.Get(); }

	/** Returns the currently inspected editable component, or null. */
	UFUNCTION(BlueprintPure, Category = "Developer Mode|Focus")
	URealityEditableComponent* GetFocusedEditableComponent() const { return FocusedEditableComponent.Get(); }

	/** Returns the active success confirmation, or None after its brief display window. */
	UFUNCTION(BlueprintPure, Category = "Developer Mode|Presentation")
	EDeveloperOperationFeedback GetOperationFeedback() const { return OperationFeedback; }

	/** Semantic property associated with the current success confirmation. */
	UFUNCTION(BlueprintPure, Category = "Developer Mode|Presentation")
	FGameplayTag GetOperationFeedbackTag() const { return OperationFeedbackTag; }

	/** Monotonic counter used by presentation and tests to distinguish new successful operations from no-ops. */
	UFUNCTION(BlueprintPure, Category = "Developer Mode|Presentation")
	int32 GetOperationFeedbackSequence() const { return OperationFeedbackSequence; }

	/** Applies or restores Cheat.Collision on the focused target using this component's owner as instigator. */
	UFUNCTION(BlueprintCallable, Category = "Developer Mode|Collision")
	bool ToggleFocusedCollisionModification();

	/** Applies a controlled Scale preset to the focused target using this component's owner as instigator. */
	UFUNCTION(BlueprintCallable, Category = "Developer Mode|Scale")
	bool ApplyFocusedScaleModification(ERealityScalePreset Preset);

	/** Restores the focused target's active Scale cycle using this component's owner as instigator. */
	UFUNCTION(BlueprintCallable, Category = "Developer Mode|Scale")
	bool RestoreFocusedScaleModification();

	/** Cycles the focused target through Normal, Low, and Zero gravity. */
	UFUNCTION(BlueprintCallable, Category = "Developer Mode|Gravity")
	bool CycleFocusedGravityModification();

	/** Applies one explicit Gravity preset to the selected target. Intended for console buttons. */
	UFUNCTION(BlueprintCallable, Category = "Developer Mode|Gravity")
	bool ApplyFocusedGravityModification(ERealityGravityPreset Preset);

	/** Restores the focused target's exact captured gravity behavior. */
	UFUNCTION(BlueprintCallable, Category = "Developer Mode|Gravity")
	bool RestoreFocusedGravityModification();

	/** Applies a typed Mass preset to the selected target. Mass intentionally has no fallback hotkey. */
	UFUNCTION(BlueprintCallable, Category = "Developer Mode|Mass")
	bool ApplyFocusedMassModification(ERealityMassPreset Preset);

	/** Restores the selected target's exact captured mass configuration. */
	UFUNCTION(BlueprintCallable, Category = "Developer Mode|Mass")
	bool RestoreFocusedMassModification();

	/** Applies a typed Friction preset to the selected target. Friction intentionally has no fallback hotkey. */
	UFUNCTION(BlueprintCallable, Category = "Developer Mode|Friction")
	bool ApplyFocusedFrictionModification(ERealityFrictionPreset Preset);

	/** Restores the selected target's exact captured Physical Material configuration. */
	UFUNCTION(BlueprintCallable, Category = "Developer Mode|Friction")
	bool RestoreFocusedFrictionModification();

	/** Applies a typed local Time preset to the selected target. Time intentionally has no fallback hotkey. */
	UFUNCTION(BlueprintCallable, Category = "Developer Mode|Time")
	bool ApplyFocusedTimeModification(ERealityTimePreset Preset);

	/** Restores the selected target's exact captured local CustomTimeDilation. */
	UFUNCTION(BlueprintCallable, Category = "Developer Mode|Time")
	bool RestoreFocusedTimeModification();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	/** Sets focus and emits transition events only when the editable target changes. */
	void SetFocusedEditableComponent(URealityEditableComponent* NewEditableComponent);

	/** Finds an editable component under the owner's active viewpoint. */
	URealityEditableComponent* FindEditableFromViewpoint();

	/** Finds the nearest collision-modified target under view when no nearer trace hit occludes it. */
	URealityEditableComponent* FindModifiedTargetUnderView(const FVector& TraceStart, const FVector& TraceEnd, float BlockingHitDistance);

	/** Refreshes the development-only on-screen shell after meaningful state changes. */
	void RefreshDebugReadout() const;

	/** Removes the prototype on-screen shell. */
	void ClearDebugReadout() const;

	/** Notifies presentation listeners and optionally refreshes the legacy engineering overlay. */
	void RefreshDeveloperPresentation();

	/** Clears focus when the focused Actor is destroyed. */
	UFUNCTION()
	void HandleFocusedActorDestroyed(AActor* DestroyedActor);

	/** Refreshes the active overlay when global Reality values change from any event source. */
	UFUNCTION()
	void HandleRealitySuspicionChanged(float OldValue, float NewValue);

	UFUNCTION()
	void HandleRealityStateChanged(ERealityState OldState, ERealityState NewState);

	/** Converts an authoritative successful editable event into presentation-only feedback. */
	UFUNCTION()
	void HandleFocusedRealityCheatEvent(const FRealityCheatEvent& CheatEvent);

	void ClearOperationFeedback();

	UPROPERTY(Transient)
	bool bDeveloperModeActive = false;

	UPROPERTY(Transient)
	TWeakObjectPtr<AActor> FocusedDeveloperActor;

	UPROPERTY(Transient)
	TWeakObjectPtr<URealityEditableComponent> FocusedEditableComponent;

	UPROPERTY(Transient)
	EDeveloperOperationFeedback OperationFeedback = EDeveloperOperationFeedback::None;

	UPROPERTY(Transient)
	FGameplayTag OperationFeedbackTag;

	UPROPERTY(Transient)
	int32 OperationFeedbackSequence = 0;

	FTimerHandle OperationFeedbackTimer;

	/** Targets disabled through this component, retained weakly so each can be reacquired independently for Restore. */
	UPROPERTY(Transient)
	TArray<TWeakObjectPtr<URealityEditableComponent>> CollisionModifiedTargets;
};
