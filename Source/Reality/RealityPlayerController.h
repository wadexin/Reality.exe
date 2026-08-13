// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RealityPlayerController.generated.h"

class UInputMappingContext;
class UDeveloperConsoleWidget;
class UDeveloperModeComponent;
class UUserWidget;

/**
 *  Simple first person Player Controller
 *  Manages the input mapping context.
 *  Overrides the Player Camera Manager class.
 */
UCLASS(abstract, config="Game")
class REALITY_API ARealityPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:

	/** Constructor */
	ARealityPlayerController();

	/** Shows the player-facing console for the supplied player-owned Developer Mode component. */
	UFUNCTION(BlueprintCallable, Category = "Developer Console")
	void ShowDeveloperConsole(UDeveloperModeComponent* DeveloperModeComponent);

	/** Hides the console and restores normal first-person input without pausing the world. */
	UFUNCTION(BlueprintCallable, Category = "Developer Console")
	void HideDeveloperConsole();

	UFUNCTION(BlueprintPure, Category = "Developer Console")
	UDeveloperConsoleWidget* GetDeveloperConsoleWidget() const { return DeveloperConsoleWidget; }

protected:

	/** Input Mapping Contexts */
	UPROPERTY(EditAnywhere, Category="Input|Input Mappings")
	TArray<UInputMappingContext*> DefaultMappingContexts;

	/** Input Mapping Contexts */
	UPROPERTY(EditAnywhere, Category="Input|Input Mappings")
	TArray<UInputMappingContext*> MobileExcludedMappingContexts;

	/** Mobile controls widget to spawn */
	UPROPERTY(EditAnywhere, Category="Input|Touch Controls")
	TSubclassOf<UUserWidget> MobileControlsWidgetClass;

	/** Pointer to the mobile controls widget */
	UPROPERTY()
	TObjectPtr<UUserWidget> MobileControlsWidget;

	/** If true, the player will use UMG touch controls even if not playing on mobile platforms */
	UPROPERTY(EditAnywhere, Config, Category = "Input|Touch Controls")
	bool bForceTouchControls = false;

	/** Gameplay initialization */
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

	/** Input mapping context setup */
	virtual void SetupInputComponent() override;

	/** Returns true if the player should use UMG touch controls */
	bool ShouldUseTouchControls() const;

private:
	void BindDeveloperMode(APawn* InPawn);
	void UnbindDeveloperMode();

	UFUNCTION()
	void HandleDeveloperModeChanged(bool bIsActive);

	/** Permanent Widget Blueprint presentation with a native fallback for safe startup/tests. */
	UPROPERTY(EditDefaultsOnly, Category = "Developer Console")
	TSoftClassPtr<UDeveloperConsoleWidget> DeveloperConsoleWidgetClass;

	UPROPERTY(Transient)
	TObjectPtr<UDeveloperConsoleWidget> DeveloperConsoleWidget;

	UPROPERTY(Transient)
	TWeakObjectPtr<UDeveloperModeComponent> BoundDeveloperModeComponent;
};
