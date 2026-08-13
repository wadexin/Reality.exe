// Copyright Epic Games, Inc. All Rights Reserved.


#include "RealityPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "RealityCameraManager.h"
#include "Blueprint/UserWidget.h"
#include "Developer/DeveloperModeComponent.h"
#include "Reality.h"
#include "UI/DeveloperConsoleWidget.h"
#include "Widgets/Input/SVirtualJoystick.h"

ARealityPlayerController::ARealityPlayerController()
{
	// set the player camera manager class
	PlayerCameraManagerClass = ARealityCameraManager::StaticClass();
	DeveloperConsoleWidgetClass = TSoftClassPtr<UDeveloperConsoleWidget>(FSoftObjectPath(TEXT("/Game/UI/Developer/WBP_DeveloperConsole.WBP_DeveloperConsole_C")));
}

void ARealityPlayerController::BeginPlay()
{
	Super::BeginPlay();
	BindDeveloperMode(GetPawn());

	
	// only spawn touch controls on local player controllers
	if (ShouldUseTouchControls() && IsLocalPlayerController())
	{
		// spawn the mobile controls widget
		MobileControlsWidget = CreateWidget<UUserWidget>(this, MobileControlsWidgetClass);

		if (MobileControlsWidget)
		{
			// add the controls to the player screen
			MobileControlsWidget->AddToPlayerScreen(0);

		} else {

			UE_LOG(LogReality, Error, TEXT("Could not spawn mobile controls widget."));

		}

	}
}

void ARealityPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	BindDeveloperMode(InPawn);
}

void ARealityPlayerController::OnUnPossess()
{
	HideDeveloperConsole();
	UnbindDeveloperMode();
	Super::OnUnPossess();
}

void ARealityPlayerController::BindDeveloperMode(APawn* InPawn)
{
	UnbindDeveloperMode();
	if (!IsLocalPlayerController() || !IsValid(InPawn))
	{
		return;
	}

	if (UDeveloperModeComponent* DeveloperComponent = InPawn->FindComponentByClass<UDeveloperModeComponent>())
	{
		BoundDeveloperModeComponent = DeveloperComponent;
		DeveloperComponent->OnDeveloperModeChanged.AddUniqueDynamic(this, &ARealityPlayerController::HandleDeveloperModeChanged);
		if (DeveloperComponent->IsDeveloperModeActive())
		{
			ShowDeveloperConsole(DeveloperComponent);
		}
	}
}

void ARealityPlayerController::UnbindDeveloperMode()
{
	if (UDeveloperModeComponent* DeveloperComponent = BoundDeveloperModeComponent.Get())
	{
		DeveloperComponent->OnDeveloperModeChanged.RemoveDynamic(this, &ARealityPlayerController::HandleDeveloperModeChanged);
	}
	BoundDeveloperModeComponent.Reset();
}

void ARealityPlayerController::HandleDeveloperModeChanged(const bool bIsActive)
{
	if (bIsActive)
	{
		ShowDeveloperConsole(BoundDeveloperModeComponent.Get());
	}
	else
	{
		HideDeveloperConsole();
	}
}

void ARealityPlayerController::ShowDeveloperConsole(UDeveloperModeComponent* DeveloperModeComponent)
{
	if (!IsLocalPlayerController() || !IsValid(DeveloperModeComponent))
	{
		return;
	}

	if (!DeveloperConsoleWidget)
	{
		UClass* WidgetClass = DeveloperConsoleWidgetClass.LoadSynchronous();
		if (!WidgetClass)
		{
			WidgetClass = UDeveloperConsoleWidget::StaticClass();
			UE_LOG(LogReality, Warning, TEXT("Developer Console Blueprint unavailable; using native prototype layout."));
		}
		DeveloperConsoleWidget = CreateWidget<UDeveloperConsoleWidget>(this, WidgetClass);
	}

	if (!DeveloperConsoleWidget)
	{
		UE_LOG(LogReality, Error, TEXT("Developer Console could not be created for '%s'."), *GetNameSafe(this));
		return;
	}

	DeveloperConsoleWidget->SetDeveloperModeComponent(DeveloperModeComponent);
	if (!DeveloperConsoleWidget->IsInViewport())
	{
		DeveloperConsoleWidget->AddToPlayerScreen(20);
	}

	FInputModeGameAndUI InputMode;
	InputMode.SetWidgetToFocus(DeveloperConsoleWidget->TakeWidget());
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetHideCursorDuringCapture(false);
	SetInputMode(InputMode);
	bShowMouseCursor = true;
	DeveloperConsoleWidget->SetKeyboardFocus();
}

void ARealityPlayerController::HideDeveloperConsole()
{
	if (DeveloperConsoleWidget)
	{
		DeveloperConsoleWidget->RemoveFromParent();
	}

	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
	bShowMouseCursor = false;
	SetIgnoreLookInput(false);
}

void ARealityPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// only add IMCs for local player controllers
	if (IsLocalPlayerController())
	{
		// Add Input Mapping Context
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
			}

			// only add these IMCs if we're not using mobile touch input
			if (!ShouldUseTouchControls())
			{
				for (UInputMappingContext* CurrentContext : MobileExcludedMappingContexts)
				{
					Subsystem->AddMappingContext(CurrentContext, 0);
				}
			}
		}
	}
	
}

bool ARealityPlayerController::ShouldUseTouchControls() const
{
	// are we on a mobile platform? Should we force touch?
	return SVirtualJoystick::ShouldDisplayTouchInterface() || bForceTouchControls;
}
