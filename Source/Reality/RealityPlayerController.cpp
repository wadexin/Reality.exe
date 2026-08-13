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
#include "UI/DemoSystemMenuWidget.h"
#include "InputCoreTypes.h"
#include "Puzzle/Demo/DemoPlayerRecoveryComponent.h"
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

void ARealityPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	HideSystemMenu();
	HideDeveloperConsole();
	UnbindDeveloperMode();
	Super::EndPlay(EndPlayReason);
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
	if (IsSystemMenuOpen())
	{
		DeveloperModeComponent->ExitDeveloperMode();
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

void ARealityPlayerController::ShowSystemMenu()
{
	if (!IsLocalPlayerController() || !IsDemoWorld() || IsSystemMenuOpen()) return;
	if (UDeveloperModeComponent* Developer = BoundDeveloperModeComponent.Get()) Developer->ExitDeveloperMode();
	SystemMenuWidget = CreateWidget<UDemoSystemMenuWidget>(this, UDemoSystemMenuWidget::StaticClass());
	if (!SystemMenuWidget) return;
	SystemMenuWidget->SetOwningRealityController(this);
	SystemMenuWidget->AddToPlayerScreen(100);
	SetPause(true);
	FInputModeGameAndUI Mode; Mode.SetWidgetToFocus(SystemMenuWidget->TakeWidget()); Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock); Mode.SetHideCursorDuringCapture(false);
	SetInputMode(Mode); bShowMouseCursor = true; SystemMenuWidget->SetKeyboardFocus();
}

void ARealityPlayerController::HideSystemMenu()
{
	if (SystemMenuWidget) { SystemMenuWidget->RemoveFromParent(); SystemMenuWidget = nullptr; }
	SetPause(false);
	FInputModeGameOnly Mode; SetInputMode(Mode); bShowMouseCursor = false; SetIgnoreLookInput(false);
}

void ARealityPlayerController::ToggleSystemMenu() { IsSystemMenuOpen() ? HideSystemMenu() : ShowSystemMenu(); }
bool ARealityPlayerController::IsSystemMenuOpen() const { return SystemMenuWidget && SystemMenuWidget->IsInViewport(); }
bool ARealityPlayerController::IsDemoWorld() const { return GetWorld() && GetWorld()->GetMapName().Contains(TEXT("Lvl_Demo_Graybox")); }

void ARealityPlayerController::ConfirmRestartDemo()
{
	if (UDeveloperModeComponent* Developer = BoundDeveloperModeComponent.Get()) Developer->ExitDeveloperMode();
	HideSystemMenu();
	RestartLevel();
}

bool ARealityPlayerController::RecoverPlayerPosition()
{
	APawn* ControlledPawn = GetPawn();
	UDemoPlayerRecoveryComponent* Recovery = ControlledPawn ? ControlledPawn->FindComponentByClass<UDemoPlayerRecoveryComponent>() : nullptr;
	const bool bRecovered = Recovery && Recovery->RecoverPlayer();
	if (bRecovered) HideSystemMenu();
	return bRecovered;
}

void ARealityPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// only add IMCs for local player controllers
	if (IsLocalPlayerController())
	{
		FInputKeyBinding& SystemMenuBinding = InputComponent->BindKey(EKeys::Escape, IE_Pressed, this, &ARealityPlayerController::ToggleSystemMenu);
		SystemMenuBinding.bExecuteWhenPaused = true;
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
