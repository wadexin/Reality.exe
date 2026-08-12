// Copyright Epic Games, Inc. All Rights Reserved.

#include "Interaction/InteractionComponent.h"

#include "DrawDebugHelpers.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"
#include "Interaction/Interactable.h"
#include "Reality.h"

UInteractionComponent::UInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	if (const APawn* OwnerPawn = Cast<APawn>(GetOwner()); OwnerPawn && !OwnerPawn->IsLocallyControlled())
	{
		bFocusTracingEnabled = false;
	}

	SetComponentTickEnabled(bFocusTracingEnabled);
	if (bFocusTracingEnabled)
	{
		UpdateFocus();
	}
}

void UInteractionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	SetFocusedActor(nullptr);
	Super::EndPlay(EndPlayReason);
}

void UInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateFocus();
}

bool UInteractionComponent::InteractWithFocusedActor()
{
	AActor* Target = FocusedActor.Get();
	AActor* Interactor = GetOwner();
	if (!IsValid(Target) || !IsValid(Interactor) || !Target->Implements<UInteractable>())
	{
		SetFocusedActor(nullptr);
		return false;
	}

	if (!IInteractable::Execute_CanInteract(Target, Interactor))
	{
		SetFocusedActor(nullptr);
		return false;
	}

	UE_LOG(LogReality, Log, TEXT("Interaction: '%s' interacting with '%s'."), *GetNameSafe(Interactor), *GetNameSafe(Target));
	IInteractable::Execute_Interact(Target, Interactor);
	return true;
}

void UInteractionComponent::SetFocusTracingEnabled(const bool bEnabled)
{
	bFocusTracingEnabled = bEnabled;
	SetComponentTickEnabled(bFocusTracingEnabled);

	if (bFocusTracingEnabled)
	{
		UpdateFocus();
	}
	else
	{
		SetFocusedActor(nullptr);
	}
}

void UInteractionComponent::UpdateFocus()
{
	SetFocusedActor(FindInteractableFromViewpoint());
}

FText UInteractionComponent::GetFocusedInteractionPrompt() const
{
	AActor* Target = FocusedActor.Get();
	AActor* Interactor = GetOwner();
	if (!IsValid(Target) || !IsValid(Interactor) || !Target->Implements<UInteractable>())
	{
		return FText::GetEmpty();
	}

	if (!IInteractable::Execute_CanInteract(Target, Interactor))
	{
		return FText::GetEmpty();
	}

	return IInteractable::Execute_GetInteractionPrompt(Target, Interactor);
}

void UInteractionComponent::SetFocusedActor(AActor* NewFocusedActor)
{
	if (FocusedActor.Get() == NewFocusedActor)
	{
		return;
	}

	AActor* PreviousActor = FocusedActor.Get();
	if (PreviousActor)
	{
		PreviousActor->OnDestroyed.RemoveDynamic(this, &UInteractionComponent::HandleFocusedActorDestroyed);
	}

	FocusedActor = NewFocusedActor;

	if (PreviousActor)
	{
		UE_LOG(LogReality, Verbose, TEXT("Interaction: Focus lost from '%s'."), *GetNameSafe(PreviousActor));
		OnFocusLost.Broadcast(PreviousActor);
	}

	if (AActor* CurrentActor = FocusedActor.Get())
	{
		CurrentActor->OnDestroyed.AddUniqueDynamic(this, &UInteractionComponent::HandleFocusedActorDestroyed);
		UE_LOG(LogReality, Verbose, TEXT("Interaction: Focus gained on '%s'."), *GetNameSafe(CurrentActor));
		OnFocusGained.Broadcast(CurrentActor);
	}
}

void UInteractionComponent::HandleFocusedActorDestroyed(AActor* DestroyedActor)
{
	if (FocusedActor.Get() == DestroyedActor)
	{
		SetFocusedActor(nullptr);
	}
}

AActor* UInteractionComponent::FindInteractableFromViewpoint() const
{
	AActor* Owner = GetOwner();
	UWorld* World = GetWorld();
	if (!IsValid(Owner) || !IsValid(World) || InteractionDistance <= 0.0f)
	{
		return nullptr;
	}

	FVector TraceStart;
	FRotator ViewRotation;
	if (const APawn* OwnerPawn = Cast<APawn>(Owner); OwnerPawn && OwnerPawn->GetController())
	{
		OwnerPawn->GetController()->GetPlayerViewPoint(TraceStart, ViewRotation);
	}
	else
	{
		Owner->GetActorEyesViewPoint(TraceStart, ViewRotation);
	}

	const FVector TraceEnd = TraceStart + ViewRotation.Vector() * InteractionDistance;
	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(InteractionTrace), false, Owner);
	FHitResult Hit;
	const bool bHit = World->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, TraceChannel, QueryParams);

#if !UE_BUILD_SHIPPING
	if (bDrawDebugTrace)
	{
		const FColor TraceColor = bHit ? FColor::Green : FColor::Red;
		DrawDebugLine(World, TraceStart, TraceEnd, TraceColor, false, 0.0f, 0, 1.0f);
		if (bHit)
		{
			DrawDebugPoint(World, Hit.ImpactPoint, 8.0f, TraceColor, false, 0.0f);
		}
	}
#endif

	AActor* HitActor = bHit ? Hit.GetActor() : nullptr;
	if (!IsValid(HitActor) || !HitActor->Implements<UInteractable>())
	{
		return nullptr;
	}

	return IInteractable::Execute_CanInteract(HitActor, Owner) ? HitActor : nullptr;
}
