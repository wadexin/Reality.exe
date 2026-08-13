// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DemoSystemMenuWidget.generated.h"

class ARealityPlayerController;
class UButton;
class UTextBlock;

/** Minimal in-session system menu providing Resume and confirmed clean Demo restart. */
UCLASS()
class REALITY_API UDemoSystemMenuWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetOwningRealityController(ARealityPlayerController* Controller);
	UFUNCTION(BlueprintPure, Category="Demo|System Menu") bool IsRestartConfirmationVisible() const { return bConfirmingRestart; }
protected:
	virtual bool Initialize() override;
	virtual FReply NativeOnKeyDown(const FGeometry&, const FKeyEvent&) override;
private:
	void BuildLayout();
	void SetRestartConfirmation(bool bConfirming);
	UFUNCTION() void HandleResume();
	UFUNCTION() void HandleRestartRequest();
	UFUNCTION() void HandleRestorePosition();
	UFUNCTION() void HandleRestartConfirm();
	UFUNCTION() void HandleRestartCancel();
	UPROPERTY(Transient) TWeakObjectPtr<ARealityPlayerController> RealityController;
	UPROPERTY(Transient) TObjectPtr<UTextBlock> PromptText;
	UPROPERTY(Transient) TObjectPtr<UButton> ResumeButton;
	UPROPERTY(Transient) TObjectPtr<UButton> RestartButton;
	UPROPERTY(Transient) TObjectPtr<UButton> RestorePositionButton;
	UPROPERTY(Transient) TObjectPtr<UButton> ConfirmButton;
	UPROPERTY(Transient) TObjectPtr<UButton> CancelButton;
	bool bConfirmingRestart = false;
};
