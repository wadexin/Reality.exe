// Copyright Epic Games, Inc. All Rights Reserved.

#include "Developer/DeveloperModeComponent.h"

#include "Developer/RealityEditableComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/Engine.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"
#include "Reality.h"
#include "RealitySystem/RealityManagerSubsystem.h"

namespace DeveloperModeDebug
{
	constexpr uint64 ReadoutKey = 0x5245414C49545944ull;
}

UDeveloperModeComponent::UDeveloperModeComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UDeveloperModeComponent::BeginPlay()
{
	Super::BeginPlay();
	if (UWorld* World = GetWorld())
	{
		if (URealityManagerSubsystem* RealityManager = World->GetSubsystem<URealityManagerSubsystem>())
		{
			RealityManager->OnRealitySuspicionChanged.AddUniqueDynamic(this, &UDeveloperModeComponent::HandleRealitySuspicionChanged);
			RealityManager->OnRealityStateChanged.AddUniqueDynamic(this, &UDeveloperModeComponent::HandleRealityStateChanged);
		}
	}
}

void UDeveloperModeComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld())
	{
		if (URealityManagerSubsystem* RealityManager = World->GetSubsystem<URealityManagerSubsystem>())
		{
			RealityManager->OnRealitySuspicionChanged.RemoveDynamic(this, &UDeveloperModeComponent::HandleRealitySuspicionChanged);
			RealityManager->OnRealityStateChanged.RemoveDynamic(this, &UDeveloperModeComponent::HandleRealityStateChanged);
		}
	}
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
	SetComponentTickEnabled(false);
	UE_LOG(LogReality, Log, TEXT("Developer Mode: Entered for '%s'."), *GetNameSafe(GetOwner()));
	UpdateDeveloperFocus();
	OnDeveloperModeChanged.Broadcast(true);
	RefreshDeveloperPresentation();
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
	OnDeveloperModeChanged.Broadcast(false);
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
		RefreshDeveloperPresentation();
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
		RefreshDeveloperPresentation();
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
		RefreshDeveloperPresentation();
	}
	return bSucceeded;
}

bool UDeveloperModeComponent::CycleFocusedGravityModification()
{
	if (!bDeveloperModeActive)
	{
		return false;
	}

	URealityEditableComponent* EditableComponent = FocusedEditableComponent.Get();
	AActor* InstigatingActor = GetOwner();
	const FGameplayTag GravityTag = FGameplayTag::RequestGameplayTag(TEXT("Cheat.Gravity"));
	if (!IsValid(EditableComponent)
		|| !IsValid(InstigatingActor)
		|| !EditableComponent->SupportsCheat(GravityTag))
	{
		return false;
	}

	ERealityGravityPreset NextPreset = ERealityGravityPreset::Low;
	if (EditableComponent->IsGravityModified())
	{
		switch (EditableComponent->GetCurrentGravityPreset())
		{
		case ERealityGravityPreset::Normal:
			NextPreset = ERealityGravityPreset::Low;
			break;
		case ERealityGravityPreset::Low:
			NextPreset = ERealityGravityPreset::Zero;
			break;
		case ERealityGravityPreset::Zero:
			NextPreset = ERealityGravityPreset::Normal;
			break;
		default:
			return false;
		}
	}

	return ApplyFocusedGravityModification(NextPreset);
}

bool UDeveloperModeComponent::ApplyFocusedGravityModification(const ERealityGravityPreset Preset)
{
	if (!bDeveloperModeActive)
	{
		return false;
	}

	URealityEditableComponent* EditableComponent = FocusedEditableComponent.Get();
	AActor* InstigatingActor = GetOwner();
	const FGameplayTag GravityTag = FGameplayTag::RequestGameplayTag(TEXT("Cheat.Gravity"));
	if (!IsValid(EditableComponent)
		|| !IsValid(InstigatingActor)
		|| !EditableComponent->SupportsCheat(GravityTag))
	{
		return false;
	}

	const bool bSucceeded = EditableComponent->ApplyGravityModification(Preset, InstigatingActor);
	if (bSucceeded)
	{
		RefreshDeveloperPresentation();
	}
	return bSucceeded;
}

bool UDeveloperModeComponent::RestoreFocusedGravityModification()
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

	const bool bSucceeded = EditableComponent->RestoreGravityModification(InstigatingActor);
	if (bSucceeded)
	{
		RefreshDeveloperPresentation();
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

	RefreshDeveloperPresentation();
}

void UDeveloperModeComponent::RefreshDeveloperPresentation()
{
	OnDeveloperConsoleRefresh.Broadcast();
	if (bShowEngineeringReadout)
	{
		RefreshDebugReadout();
	}
	else
	{
		ClearDebugReadout();
	}
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

		const FGameplayTag GravityTag = FGameplayTag::RequestGameplayTag(TEXT("Cheat.Gravity"));
		if (EditableComponent->SupportsCheat(GravityTag))
		{
			const UEnum* PresetEnum = StaticEnum<ERealityGravityPreset>();
			const FString PresetLabel = PresetEnum
				? PresetEnum->GetDisplayNameTextByValue(static_cast<int64>(EditableComponent->GetCurrentGravityPreset())).ToString()
				: TEXT("Unknown");
			Readout += FString::Printf(
				TEXT("\nGravity Modified: %s\nGravity Preset: %s\nEligible Physics Components: %d\nLow Gravity Force Active: %s\nGravity: G=Cycle  H=Restore"),
				EditableComponent->IsGravityModified() ? TEXT("True") : TEXT("False"),
				*PresetLabel,
				EditableComponent->GetEligibleGravityComponentCount(),
				EditableComponent->IsComponentTickEnabled() ? TEXT("True") : TEXT("False"));
		}
	}

	if (const UWorld* World = GetWorld())
	{
		if (const URealityManagerSubsystem* RealityManager = World->GetSubsystem<URealityManagerSubsystem>())
		{
			const UEnum* StateEnum = StaticEnum<ERealityState>();
			const FString StateLabel = StateEnum
				? StateEnum->GetDisplayNameTextByValue(static_cast<int64>(RealityManager->GetRealityState())).ToString()
				: TEXT("Unknown");
			Readout += FString::Printf(TEXT("\n\nREALITY\nSuspicion: %.0f / 100\nState: %s"), RealityManager->GetSuspicion(), *StateLabel);
			if (const FRealityProcessedCheatRecord* LastEvent = RealityManager->GetMostRecentEvent())
			{
				Readout += FString::Printf(
					TEXT("\nLast Event: %s %s %+.0f\nBase: %+.0f\nWitness: %+.0f\nContext: -%.0f\nObserved By: %d\nMatched Contexts: %d"),
					*LastEvent->CheatTag.ToString(),
					LastEvent->Operation == ERealityCheatOperation::Apply ? TEXT("Apply") : TEXT("Restore"),
					LastEvent->SuspicionDelta,
					LastEvent->BaseSuspicionDelta,
					LastEvent->WitnessSuspicionDelta,
					LastEvent->ContextSuspicionReduction,
					LastEvent->ObservingWitnessCount,
					LastEvent->MatchedContextCount);
				if (!LastEvent->MatchedContextTags.IsEmpty())
				{
					Readout += FString::Printf(TEXT("\nPlausibility: %s"), *LastEvent->MatchedContextTags.ToStringSimple());
				}
			}
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

void UDeveloperModeComponent::HandleRealitySuspicionChanged(const float OldValue, const float NewValue)
{
	RefreshDeveloperPresentation();
}

void UDeveloperModeComponent::HandleRealityStateChanged(const ERealityState OldState, const ERealityState NewState)
{
	RefreshDeveloperPresentation();
}
