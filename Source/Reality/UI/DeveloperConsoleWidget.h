// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Developer/RealityEditableComponent.h"
#include "DeveloperConsoleWidget.generated.h"

class UButton;
class UDeveloperModeComponent;
class UTextBlock;
class UVerticalBox;

/**
 * Player-facing Developer Console presentation for one player-owned Developer Mode component.
 * It reads authoritative gameplay state and invokes typed component operations; it owns no Reality rules.
 */
UCLASS(Blueprintable)
class REALITY_API UDeveloperConsoleWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Assigns the player-owned gameplay authority displayed and controlled by this console. */
	UFUNCTION(BlueprintCallable, Category = "Developer Console")
	void SetDeveloperModeComponent(UDeveloperModeComponent* NewDeveloperModeComponent);

	/** Rebuilds all displayed values and capability visibility from authoritative gameplay state. */
	UFUNCTION(BlueprintCallable, Category = "Developer Console")
	void RefreshConsole();

	UFUNCTION(BlueprintPure, Category = "Developer Console")
	UDeveloperModeComponent* GetDeveloperModeComponent() const { return DeveloperModeComponent.Get(); }

	/** Returns whether the current selected target supports an exact Cheat capability tag. */
	UFUNCTION(BlueprintPure, Category = "Developer Console")
	bool IsCheatSectionAvailable(FGameplayTag CheatTag) const;

	UFUNCTION(BlueprintPure, Category = "Developer Console")
	bool HasEditableTarget() const;

	UFUNCTION(BlueprintCallable, Category = "Developer Console|Actions")
	bool ExecuteCollisionToggle();

	UFUNCTION(BlueprintCallable, Category = "Developer Console|Actions")
	bool ExecuteScalePreset(ERealityScalePreset Preset);

	UFUNCTION(BlueprintCallable, Category = "Developer Console|Actions")
	bool ExecuteScaleRestore();

	UFUNCTION(BlueprintCallable, Category = "Developer Console|Actions")
	bool ExecuteGravityPreset(ERealityGravityPreset Preset);

	UFUNCTION(BlueprintCallable, Category = "Developer Console|Actions")
	bool ExecuteGravityRestore();

	UFUNCTION(BlueprintCallable, Category = "Developer Console|Actions")
	bool ExecuteMassPreset(ERealityMassPreset Preset);

	UFUNCTION(BlueprintCallable, Category = "Developer Console|Actions")
	bool ExecuteMassRestore();

	UFUNCTION(BlueprintCallable, Category = "Developer Console|Actions")
	bool ExecuteFrictionPreset(ERealityFrictionPreset Preset);

	UFUNCTION(BlueprintCallable, Category = "Developer Console|Actions")
	bool ExecuteFrictionRestore();

	UFUNCTION(BlueprintCallable, Category = "Developer Console|Actions")
	bool ExecuteTimePreset(ERealityTimePreset Preset);

	UFUNCTION(BlueprintCallable, Category = "Developer Console|Actions")
	bool ExecuteTimeRestore();

protected:
	virtual bool Initialize() override;
	virtual void NativeDestruct() override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

private:
	void BuildPrototypeLayout();
	UTextBlock* AddLabel(UVerticalBox* Parent, const FString& Text, int32 Size, const FLinearColor& Color);
	UButton* AddButton(UVerticalBox* Parent, const FString& Text);
	void UnbindDeveloperModeComponent();

	UFUNCTION() void HandleRefreshRequested();
	UFUNCTION() void HandleCollisionClicked();
	UFUNCTION() void HandleScaleQuarterClicked();
	UFUNCTION() void HandleScaleHalfClicked();
	UFUNCTION() void HandleScaleOneClicked();
	UFUNCTION() void HandleScaleDoubleClicked();
	UFUNCTION() void HandleScaleQuadrupleClicked();
	UFUNCTION() void HandleScaleRestoreClicked();
	UFUNCTION() void HandleGravityNormalClicked();
	UFUNCTION() void HandleGravityLowClicked();
	UFUNCTION() void HandleGravityZeroClicked();
	UFUNCTION() void HandleGravityRestoreClicked();
	UFUNCTION() void HandleMassQuarterClicked();
	UFUNCTION() void HandleMassHalfClicked();
	UFUNCTION() void HandleMassOneClicked();
	UFUNCTION() void HandleMassDoubleClicked();
	UFUNCTION() void HandleMassQuadrupleClicked();
	UFUNCTION() void HandleMassRestoreClicked();
	UFUNCTION() void HandleFrictionZeroClicked();
	UFUNCTION() void HandleFrictionLowClicked();
	UFUNCTION() void HandleFrictionNormalClicked();
	UFUNCTION() void HandleFrictionHighClicked();
	UFUNCTION() void HandleFrictionRestoreClicked();
	UFUNCTION() void HandleTimeQuarterClicked();
	UFUNCTION() void HandleTimeHalfClicked();
	UFUNCTION() void HandleTimeOneClicked();
	UFUNCTION() void HandleTimeDoubleClicked();
	UFUNCTION() void HandleTimeQuadrupleClicked();
	UFUNCTION() void HandleTimeRestoreClicked();

	UPROPERTY(Transient)
	TWeakObjectPtr<UDeveloperModeComponent> DeveloperModeComponent;

	UPROPERTY(Transient) TObjectPtr<UTextBlock> TargetText;
	UPROPERTY(Transient) TObjectPtr<UTextBlock> TargetDetailsText;
	UPROPERTY(Transient) TObjectPtr<UVerticalBox> CollisionSection;
	UPROPERTY(Transient) TObjectPtr<UTextBlock> CollisionStateText;
	UPROPERTY(Transient) TObjectPtr<UButton> CollisionButton;
	UPROPERTY(Transient) TObjectPtr<UTextBlock> CollisionButtonText;
	UPROPERTY(Transient) TObjectPtr<UVerticalBox> ScaleSection;
	UPROPERTY(Transient) TObjectPtr<UTextBlock> ScaleStateText;
	UPROPERTY(Transient) TObjectPtr<UButton> ScaleRestoreButton;
	UPROPERTY(Transient) TObjectPtr<UVerticalBox> GravitySection;
	UPROPERTY(Transient) TObjectPtr<UTextBlock> GravityStateText;
	UPROPERTY(Transient) TObjectPtr<UButton> GravityRestoreButton;
	UPROPERTY(Transient) TObjectPtr<UVerticalBox> MassSection;
	UPROPERTY(Transient) TObjectPtr<UTextBlock> MassStateText;
	UPROPERTY(Transient) TObjectPtr<UButton> MassRestoreButton;
	UPROPERTY(Transient) TObjectPtr<UVerticalBox> FrictionSection;
	UPROPERTY(Transient) TObjectPtr<UTextBlock> FrictionStateText;
	UPROPERTY(Transient) TObjectPtr<UButton> FrictionRestoreButton;
	UPROPERTY(Transient) TObjectPtr<UVerticalBox> TimeSection;
	UPROPERTY(Transient) TObjectPtr<UTextBlock> TimeStateText;
	UPROPERTY(Transient) TObjectPtr<UButton> TimeRestoreButton;
	UPROPERTY(Transient) TObjectPtr<UTextBlock> RealityText;
};
