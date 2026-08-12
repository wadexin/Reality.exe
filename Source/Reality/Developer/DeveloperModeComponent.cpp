// Copyright Epic Games, Inc. All Rights Reserved.

#include "Developer/DeveloperModeComponent.h"

#include "Developer/RealityEditableComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/Engine.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"
#include "Reality.h"

namespace DeveloperModeDebug
{
	constexpr uint64 ReadoutKey = 0x5245414C49545944ull;
}

UDeveloperModeComponent::UDeveloperModeComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UDeveloperModeComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ExitDeveloperMode();
	Super::EndPlay(EndPlayReason);
}

void UDeveloperModeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	UpdateDeveloperFocus();
}

void UDeveloperModeComponent::EnterDeveloperMode()
{
	if (bDeveloperModeActive)
	{
		return;
	}

	bDeveloperModeActive = true;
	SetComponentTickEnabled(true);
	UE_LOG(LogReality, Log, TEXT("Developer Mode: Entered for '%s'."), *GetNameSafe(GetOwner()));
	UpdateDeveloperFocus();
	RefreshDebugReadout();
}

void UDeveloperModeComponent::ExitDeveloperMode()
{
	if (!bDeveloperModeActive)
	{
		SetComponentTickEnabled(false);
		SetFocusedEditableComponent(nullptr);
		ClearDebugReadout();
		return;
	}

	bDeveloperModeActive = false;
	SetComponentTickEnabled(false);
	SetFocusedEditableComponent(nullptr);
	ClearDebugReadout();
	UE_LOG(LogReality, Log, TEXT("Developer Mode: Exited for '%s'."), *GetNameSafe(GetOwner()));
}

bool UDeveloperModeComponent::ToggleDeveloperMode()
{
	if (bDeveloperModeActive)
	{
		ExitDeveloperMode();
	}
	else
	{
		EnterDeveloperMode();
	}

	return bDeveloperModeActive;
}

void UDeveloperModeComponent::UpdateDeveloperFocus()
{
	if (!bDeveloperModeActive)
	{
		return;
	}

	SetFocusedEditableComponent(FindEditableFromViewpoint());
}

bool UDeveloperModeComponent::ToggleFocusedCollisionModification()
{
	if (!bDeveloperModeActive)
	{
		return false;
	}

	URealityEditableComponent* EditableComponent = FocusedEditableComponent.Get();
	AActor* InstigatingActor = GetOwner();
	const FGameplayTag CollisionTag = FGameplayTag::RequestGameplayTag(TEXT("Cheat.Collision"));
	if (!IsValid(EditableComponent)
		|| !IsValid(InstigatingActor)
		|| !EditableComponent->SupportsCheat(CollisionTag))
	{
		return false;
	}

	const bool bWasModified = EditableComponent->IsCollisionModified();
	const bool bSucceeded = bWasModified
		? EditableComponent->RestoreCollisionModification(InstigatingActor)
		: EditableComponent->ApplyCollisionModification(InstigatingActor);

	if (bSucceeded)
	{
		if (bWasModified)
		{
			CollisionModifiedTargets.Remove(EditableComponent);
		}
		else
		{
			CollisionModifiedTargets.AddUnique(EditableComponent);
		}
		RefreshDebugReadout();
	}
	return bSucceeded;
}

bool UDeveloperModeComponent::ApplyFocusedScaleModification(const ERealityScalePreset Preset)
{
	if (!bDeveloperModeActive)
	{
		return false;
	}

	URealityEditableComponent* EditableComponent = FocusedEditableComponent.Get();
	AActor* InstigatingActor = GetOwner();
	const FGameplayTag ScaleTag = FGameplayTag::RequestGameplayTag(TEXT("Cheat.Scale"));
	if (!IsValid(EditableComponent)
		|| !IsValid(InstigatingActor)
		|| !EditableComponent->SupportsCheat(ScaleTag))
	{
		return false;
	}

	const bool bSucceeded = EditableComponent->ApplyScaleModification(Preset, InstigatingActor);
	if (bSucceeded)
	{
		RefreshDebugReadout();
	}
	return bSucceeded;
}

bool UDeveloperModeComponent::RestoreFocusedScaleModification()
{
	if (!bDeveloperModeActive)
	{
		return false;
	}

	URealityEditableComponent* EditableComponent = FocusedEditableComponent.Get();
	AActor* InstigatingActor = GetOwner();
	if (!IsValid(EditableComponent) || !IsValid(InstigatingActor))
	{
		return false;
	}

	const bool bSucceeded = EditableComponent->RestoreScaleModification(InstigatingActor);
	if (bSucceeded)
	{
		RefreshDebugReadout();
	}
	return bSucceeded;
}

void UDeveloperModeComponent::SetFocusedEditableComponent(URealityEditableComponent* NewEditableComponent)
{
	AActor* NewActor = IsValid(NewEditableComponent) ? NewEditableComponent->GetOwner() : nullptr;
	if (FocusedEditableComponent.Get() == NewEditableComponent && FocusedDeveloperActor.Get() == NewActor)
	{
		return;
	}

	AActor* PreviousActor = FocusedDeveloperActor.Get();
	if (PreviousActor)
	{
		PreviousActor->OnDestroyed.RemoveDynamic(this, &UDeveloperModeComponent::HandleFocusedActorDestroyed);
	}

	FocusedDeveloperActor = NewActor;
	FocusedEditableComponent = NewEditableComponent;

	if (PreviousActor)
	{
		UE_LOG(LogReality, Verbose, TEXT("Developer Mode: Focus lost from '%s'."), *GetNameSafe(PreviousActor));
		OnDeveloperFocusLost.Broadcast(PreviousActor);
	}

	if (NewActor)
	{
		NewActor->OnDestroyed.AddUniqueDynamic(this, &UDeveloperModeComponent::HandleFocusedActorDestroyed);
		UE_LOG(LogReality, Log, TEXT("Developer Mode: Focus gained. %s"), *NewEditableComponent->GetEditableDebugDescription());
		OnDeveloperFocusGained.Broadcast(NewActor);
	}

	RefreshDebugReadout();
}

URealityEditableComponent* UDeveloperModeComponent::FindEditableFromViewpoint()
{
	AActor* Owner = GetOwner();
	UWorld* World = GetWorld();
	if (!IsValid(Owner) || !IsValid(World) || InspectionDistance <= 0.0f)
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

	const FVector TraceEnd = TraceStart + ViewRotation.Vector() * InspectionDistance;
	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(DeveloperModeTrace), false, Owner);
	FHitResult Hit;
	const bool bHit = World->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, TraceChannel, QueryParams);

#if !UE_BUILD_SHIPPING
	if (bDrawDebugTrace)
	{
		DrawDebugLine(World, TraceStart, TraceEnd, bHit ? FColor::Cyan : FColor::Silver, false, 0.0f, 0, 1.0f);
		if (bHit)
		{
			DrawDebugPoint(World, Hit.ImpactPoint, 8.0f, FColor::Cyan, false, 0.0f);
		}
	}
#endif

	if (URealityEditableComponent* ModifiedTarget = FindModifiedTargetUnderView(TraceStart, TraceEnd, bHit ? Hit.Distance : InspectionDistance))
	{
		return ModifiedTarget;
	}

	AActor* HitActor = bHit ? Hit.GetActor() : nullptr;
	return IsValid(HitActor) ? HitActor->FindComponentByClass<URealityEditableComponent>() : nullptr;
}

URealityEditableComponent* UDeveloperModeComponent::FindModifiedTargetUnderView(
	const FVector& TraceStart,
	const FVector& TraceEnd,
	const float BlockingHitDistance)
{
	URealityEditableComponent* NearestTarget = nullptr;
	float NearestDistance = BlockingHitDistance + UE_KINDA_SMALL_NUMBER;
	for (int32 TargetIndex = CollisionModifiedTargets.Num() - 1; TargetIndex >= 0; --TargetIndex)
	{
		URealityEditableComponent* EditableComponent = CollisionModifiedTargets[TargetIndex].Get();
		AActor* ModifiedActor = IsValid(EditableComponent) ? EditableComponent->GetOwner() : nullptr;
		if (!IsValid(EditableComponent) || !IsValid(ModifiedActor) || !EditableComponent->IsCollisionModified())
		{
			CollisionModifiedTargets.RemoveAtSwap(TargetIndex);
			continue;
		}

		const FBox TargetBounds = ModifiedActor->GetComponentsBoundingBox(true, false);
		FVector BoundsHitLocation;
		FVector BoundsHitNormal;
		float BoundsHitTime = 0.0f;
		if (!TargetBounds.IsValid
			|| !FMath::LineExtentBoxIntersection(TargetBounds, TraceStart, TraceEnd, FVector::ZeroVector, BoundsHitLocation, BoundsHitNormal, BoundsHitTime))
		{
			continue;
		}

		const float BoundsHitDistance = FVector::Distance(TraceStart, BoundsHitLocation);
		if (BoundsHitDistance <= NearestDistance)
		{
			NearestDistance = BoundsHitDistance;
			NearestTarget = EditableComponent;
		}
	}

	return NearestTarget;
}

void UDeveloperModeComponent::RefreshDebugReadout() const
{
#if !UE_BUILD_SHIPPING
	if (!bDeveloperModeActive || !GEngine)
	{
		return;
	}

	FString Readout = TEXT("DEVELOPER MODE\nTarget: None\nCollision Toggle: R");
	if (const URealityEditableComponent* EditableComponent = FocusedEditableComponent.Get())
	{
		Readout = FString::Printf(
			TEXT("DEVELOPER MODE\nActor: %s\nObject Tags: %s\nSupported Cheats: %s\nCollision Modified: %s\nCollision Toggle: R"),
			*GetNameSafe(EditableComponent->GetOwner()),
			*EditableComponent->GetObjectTags().ToStringSimple(),
			*EditableComponent->GetSupportedCheats().ToStringSimple(),
			EditableComponent->IsCollisionModified() ? TEXT("True") : TEXT("False"));

		const FGameplayTag ScaleTag = FGameplayTag::RequestGameplayTag(TEXT("Cheat.Scale"));
		if (EditableComponent->SupportsCheat(ScaleTag))
		{
			const UEnum* PresetEnum = StaticEnum<ERealityScalePreset>();
			const FString PresetLabel = PresetEnum
				? PresetEnum->GetDisplayNameTextByValue(static_cast<int64>(EditableComponent->GetCurrentScalePreset())).ToString()
				: TEXT("Unknown");
			const FVector OriginalScale = EditableComponent->GetOriginalScale();
			const FVector CurrentScale = EditableComponent->GetOwner()->GetActorScale3D();
			Readout += FString::Printf(
				TEXT("\nScale Modified: %s\nCurrent Scale Preset: %s\nOriginal Scale: %.3f / %.3f / %.3f\nCurrent Actor Scale: %.3f / %.3f / %.3f\nScale: 1=0.25x  2=0.5x  3=1.0x  4=2.0x  5=4.0x\nRestore Scale: T"),
				EditableComponent->IsScaleModified() ? TEXT("True") : TEXT("False"),
				*PresetLabel,
				OriginalScale.X,
				OriginalScale.Y,
				OriginalScale.Z,
				CurrentScale.X,
				CurrentScale.Y,
				CurrentScale.Z);
		}
	}

	GEngine->AddOnScreenDebugMessage(DeveloperModeDebug::ReadoutKey, 86400.0f, FColor::Cyan, Readout, true);
#endif
}

void UDeveloperModeComponent::ClearDebugReadout() const
{
#if !UE_BUILD_SHIPPING
	if (GEngine)
	{
		GEngine->RemoveOnScreenDebugMessage(DeveloperModeDebug::ReadoutKey);
	}
#endif
}

void UDeveloperModeComponent::HandleFocusedActorDestroyed(AActor* DestroyedActor)
{
	if (FocusedDeveloperActor.Get() == DestroyedActor)
	{
		SetFocusedEditableComponent(nullptr);
	}
}
