// Copyright Epic Games, Inc. All Rights Reserved.

#include "RealityCharacter.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Developer/DeveloperModeComponent.h"
#include "Developer/DeveloperTargetPresentationComponent.h"
#include "Audio/DeveloperAudioFeedbackComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"
#include "InputCoreTypes.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interaction/InteractionComponent.h"
#include "Reality.h"

ARealityCharacter::ARealityCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
	
	// Create the first person mesh that will be viewed only by this character's owner
	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("First Person Mesh"));

	FirstPersonMesh->SetupAttachment(GetMesh());
	FirstPersonMesh->SetOnlyOwnerSee(true);
	FirstPersonMesh->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;
	FirstPersonMesh->SetCollisionProfileName(FName("NoCollision"));

	// Create the Camera Component	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("First Person Camera"));
	FirstPersonCameraComponent->SetupAttachment(FirstPersonMesh, FName("head"));
	FirstPersonCameraComponent->SetRelativeLocationAndRotation(FVector(-2.8f, 5.89f, 0.0f), FRotator(0.0f, 90.0f, -90.0f));
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	FirstPersonCameraComponent->bEnableFirstPersonFieldOfView = true;
	FirstPersonCameraComponent->bEnableFirstPersonScale = true;
	FirstPersonCameraComponent->FirstPersonFieldOfView = 70.0f;
	FirstPersonCameraComponent->FirstPersonScale = 0.6f;

	// Create the reusable interaction component.
	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("Interaction Component"));
	InteractionAction = CreateDefaultSubobject<UInputAction>(TEXT("Interaction Action"));
	InteractionMappingContext = CreateDefaultSubobject<UInputMappingContext>(TEXT("Interaction Mapping Context"));
	if (!InteractionMappingContext->HasMappingForInputAction(InteractionAction))
	{
		InteractionMappingContext->MapKey(InteractionAction, EKeys::E);
	}

	// Create the minimal Developer Mode shell and its isolated prototype controls.
	DeveloperModeComponent = CreateDefaultSubobject<UDeveloperModeComponent>(TEXT("Developer Mode Component"));
	DeveloperTargetPresentationComponent = CreateDefaultSubobject<UDeveloperTargetPresentationComponent>(TEXT("Developer Target Presentation Component"));
	DeveloperAudioFeedbackComponent = CreateDefaultSubobject<UDeveloperAudioFeedbackComponent>(TEXT("Developer Audio Feedback Component"));
	DeveloperModeAction = CreateDefaultSubobject<UInputAction>(TEXT("Developer Mode Action"));
	DeveloperCollisionAction = CreateDefaultSubobject<UInputAction>(TEXT("Developer Collision Action"));
	DeveloperScaleActions.Reserve(5);
	DeveloperScaleActions.Add(CreateDefaultSubobject<UInputAction>(TEXT("Developer Scale Quarter Action")));
	DeveloperScaleActions.Add(CreateDefaultSubobject<UInputAction>(TEXT("Developer Scale Half Action")));
	DeveloperScaleActions.Add(CreateDefaultSubobject<UInputAction>(TEXT("Developer Scale One Action")));
	DeveloperScaleActions.Add(CreateDefaultSubobject<UInputAction>(TEXT("Developer Scale Double Action")));
	DeveloperScaleActions.Add(CreateDefaultSubobject<UInputAction>(TEXT("Developer Scale Quadruple Action")));
	DeveloperScaleRestoreAction = CreateDefaultSubobject<UInputAction>(TEXT("Developer Scale Restore Action"));
	DeveloperGravityCycleAction = CreateDefaultSubobject<UInputAction>(TEXT("Developer Gravity Cycle Action"));
	DeveloperGravityRestoreAction = CreateDefaultSubobject<UInputAction>(TEXT("Developer Gravity Restore Action"));
	DeveloperMappingContext = CreateDefaultSubobject<UInputMappingContext>(TEXT("Developer Mapping Context"));
	if (!DeveloperMappingContext->HasMappingForInputAction(DeveloperModeAction))
	{
		DeveloperMappingContext->MapKey(DeveloperModeAction, EKeys::F6);
	}
	if (!DeveloperMappingContext->HasMappingForInputAction(DeveloperCollisionAction))
	{
		DeveloperMappingContext->MapKey(DeveloperCollisionAction, EKeys::R);
	}
	const FKey ScaleKeys[] = {EKeys::One, EKeys::Two, EKeys::Three, EKeys::Four, EKeys::Five};
	for (int32 ScaleActionIndex = 0; ScaleActionIndex < DeveloperScaleActions.Num(); ++ScaleActionIndex)
	{
		if (!DeveloperMappingContext->HasMappingForInputAction(DeveloperScaleActions[ScaleActionIndex]))
		{
			DeveloperMappingContext->MapKey(DeveloperScaleActions[ScaleActionIndex], ScaleKeys[ScaleActionIndex]);
		}
	}
	if (!DeveloperMappingContext->HasMappingForInputAction(DeveloperScaleRestoreAction))
	{
		DeveloperMappingContext->MapKey(DeveloperScaleRestoreAction, EKeys::T);
	}
	if (!DeveloperMappingContext->HasMappingForInputAction(DeveloperGravityCycleAction))
	{
		DeveloperMappingContext->MapKey(DeveloperGravityCycleAction, EKeys::G);
	}
	if (!DeveloperMappingContext->HasMappingForInputAction(DeveloperGravityRestoreAction))
	{
		DeveloperMappingContext->MapKey(DeveloperGravityRestoreAction, EKeys::H);
	}

	// configure the character comps
	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::WorldSpaceRepresentation;

	GetCapsuleComponent()->SetCapsuleSize(34.0f, 96.0f);

	// Configure character movement
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	GetCharacterMovement()->AirControl = 0.5f;
}

void ARealityCharacter::PawnClientRestart()
{
	Super::PawnClientRestart();

	if (!InteractionMappingContext || !DeveloperMappingContext)
	{
		UE_LOG(LogReality, Error, TEXT("'%s' is missing an Interaction or Developer Mapping Context."), *GetNameSafe(this));
		return;
	}

	if (const APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				if (!Subsystem->HasMappingContext(InteractionMappingContext))
				{
					Subsystem->AddMappingContext(InteractionMappingContext, 0);
				}
				if (!Subsystem->HasMappingContext(DeveloperMappingContext))
				{
					Subsystem->AddMappingContext(DeveloperMappingContext, 0);
				}
			}
		}
	}
}

void ARealityCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ARealityCharacter::DoJumpStart);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ARealityCharacter::DoJumpEnd);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ARealityCharacter::MoveInput);

		// Looking/Aiming
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ARealityCharacter::LookInput);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &ARealityCharacter::LookInput);

		// Interacting
		if (InteractionAction)
		{
			EnhancedInputComponent->BindAction(InteractionAction, ETriggerEvent::Started, this, &ARealityCharacter::DoInteract);
		}
		else
		{
			UE_LOG(LogReality, Warning, TEXT("'%s' has no Interaction Input Action configured."), *GetNameSafe(this));
		}

		// Minimal Developer Mode prototype controls.
		if (DeveloperModeAction
			&& DeveloperCollisionAction
			&& DeveloperScaleActions.Num() == 5
			&& DeveloperScaleRestoreAction
			&& DeveloperGravityCycleAction
			&& DeveloperGravityRestoreAction)
		{
			EnhancedInputComponent->BindAction(DeveloperModeAction, ETriggerEvent::Started, this, &ARealityCharacter::DoToggleDeveloperMode);
			EnhancedInputComponent->BindAction(DeveloperCollisionAction, ETriggerEvent::Started, this, &ARealityCharacter::DoToggleDeveloperCollision);
			EnhancedInputComponent->BindAction(DeveloperScaleActions[0], ETriggerEvent::Started, this, &ARealityCharacter::DoApplyDeveloperScaleQuarter);
			EnhancedInputComponent->BindAction(DeveloperScaleActions[1], ETriggerEvent::Started, this, &ARealityCharacter::DoApplyDeveloperScaleHalf);
			EnhancedInputComponent->BindAction(DeveloperScaleActions[2], ETriggerEvent::Started, this, &ARealityCharacter::DoApplyDeveloperScaleOne);
			EnhancedInputComponent->BindAction(DeveloperScaleActions[3], ETriggerEvent::Started, this, &ARealityCharacter::DoApplyDeveloperScaleDouble);
			EnhancedInputComponent->BindAction(DeveloperScaleActions[4], ETriggerEvent::Started, this, &ARealityCharacter::DoApplyDeveloperScaleQuadruple);
			EnhancedInputComponent->BindAction(DeveloperScaleRestoreAction, ETriggerEvent::Started, this, &ARealityCharacter::DoRestoreDeveloperScale);
			EnhancedInputComponent->BindAction(DeveloperGravityCycleAction, ETriggerEvent::Started, this, &ARealityCharacter::DoCycleDeveloperGravity);
			EnhancedInputComponent->BindAction(DeveloperGravityRestoreAction, ETriggerEvent::Started, this, &ARealityCharacter::DoRestoreDeveloperGravity);
		}
		else
		{
			UE_LOG(LogReality, Warning, TEXT("'%s' is missing a Developer Mode prototype Input Action."), *GetNameSafe(this));
		}
	}
	else
	{
		UE_LOG(LogReality, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}


void ARealityCharacter::MoveInput(const FInputActionValue& Value)
{
	// get the Vector2D move axis
	FVector2D MovementVector = Value.Get<FVector2D>();

	// pass the axis values to the move input
	DoMove(MovementVector.X, MovementVector.Y);

}

void ARealityCharacter::LookInput(const FInputActionValue& Value)
{
	// get the Vector2D look axis
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// pass the axis values to the aim input
	DoAim(LookAxisVector.X, LookAxisVector.Y);

}

void ARealityCharacter::DoAim(float Yaw, float Pitch)
{
	if (GetController())
	{
		// pass the rotation inputs
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void ARealityCharacter::DoMove(float Right, float Forward)
{
	if (GetController())
	{
		// pass the move inputs
		AddMovementInput(GetActorRightVector(), Right);
		AddMovementInput(GetActorForwardVector(), Forward);
	}
}

void ARealityCharacter::DoJumpStart()
{
	// pass Jump to the character
	Jump();
}

void ARealityCharacter::DoJumpEnd()
{
	// pass StopJumping to the character
	StopJumping();
}

void ARealityCharacter::DoInteract()
{
	if (InteractionComponent)
	{
		InteractionComponent->InteractWithFocusedActor();
	}
}

void ARealityCharacter::DoToggleDeveloperMode()
{
	if (DeveloperModeComponent)
	{
		DeveloperModeComponent->ToggleDeveloperMode();
	}
}

void ARealityCharacter::DoToggleDeveloperCollision()
{
	if (DeveloperModeComponent)
	{
		DeveloperModeComponent->ToggleFocusedCollisionModification();
	}
}

void ARealityCharacter::DoApplyDeveloperScaleQuarter()
{
	if (DeveloperModeComponent)
	{
		DeveloperModeComponent->ApplyFocusedScaleModification(ERealityScalePreset::Quarter);
	}
}

void ARealityCharacter::DoApplyDeveloperScaleHalf()
{
	if (DeveloperModeComponent)
	{
		DeveloperModeComponent->ApplyFocusedScaleModification(ERealityScalePreset::Half);
	}
}

void ARealityCharacter::DoApplyDeveloperScaleOne()
{
	if (DeveloperModeComponent)
	{
		DeveloperModeComponent->ApplyFocusedScaleModification(ERealityScalePreset::One);
	}
}

void ARealityCharacter::DoApplyDeveloperScaleDouble()
{
	if (DeveloperModeComponent)
	{
		DeveloperModeComponent->ApplyFocusedScaleModification(ERealityScalePreset::Double);
	}
}

void ARealityCharacter::DoApplyDeveloperScaleQuadruple()
{
	if (DeveloperModeComponent)
	{
		DeveloperModeComponent->ApplyFocusedScaleModification(ERealityScalePreset::Quadruple);
	}
}

void ARealityCharacter::DoRestoreDeveloperScale()
{
	if (DeveloperModeComponent)
	{
		DeveloperModeComponent->RestoreFocusedScaleModification();
	}
}

void ARealityCharacter::DoCycleDeveloperGravity()
{
	if (DeveloperModeComponent)
	{
		DeveloperModeComponent->CycleFocusedGravityModification();
	}
}

void ARealityCharacter::DoRestoreDeveloperGravity()
{
	if (DeveloperModeComponent)
	{
		DeveloperModeComponent->RestoreFocusedGravityModification();
	}
}
