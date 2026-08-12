// Copyright Epic Games, Inc. All Rights Reserved.

#include "Developer/RealityEditableComponent.h"

#include "Components/PrimitiveComponent.h"
#include "GameFramework/Actor.h"
#include "NativeGameplayTags.h"
#include "Reality.h"
#include "RealitySystem/RealityManagerSubsystem.h"

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Reality_Cheat_Collision, "Cheat.Collision");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Reality_Cheat_Scale, "Cheat.Scale");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Reality_Cheat_Gravity, "Cheat.Gravity");

namespace RealityScale
{
	bool TryGetMultiplier(const ERealityScalePreset Preset, float& OutMultiplier)
	{
		switch (Preset)
		{
		case ERealityScalePreset::Quarter:
			OutMultiplier = 0.25f;
			return true;
		case ERealityScalePreset::Half:
			OutMultiplier = 0.5f;
			return true;
		case ERealityScalePreset::One:
			OutMultiplier = 1.0f;
			return true;
		case ERealityScalePreset::Double:
			OutMultiplier = 2.0f;
			return true;
		case ERealityScalePreset::Quadruple:
			OutMultiplier = 4.0f;
			return true;
		default:
			return false;
		}
	}

	FString GetPresetLabel(const ERealityScalePreset Preset)
	{
		float Multiplier = 0.0f;
		return TryGetMultiplier(Preset, Multiplier)
			? FString::Printf(TEXT("%gx"), Multiplier)
			: TEXT("Invalid");
	}
}

namespace RealityGravity
{
	constexpr float LowGravityMultiplier = 0.25f;

	bool IsValidPreset(const ERealityGravityPreset Preset)
	{
		return Preset == ERealityGravityPreset::Normal
			|| Preset == ERealityGravityPreset::Low
			|| Preset == ERealityGravityPreset::Zero;
	}

	const TCHAR* GetPresetLabel(const ERealityGravityPreset Preset)
	{
		switch (Preset)
		{
		case ERealityGravityPreset::Normal:
			return TEXT("Normal");
		case ERealityGravityPreset::Low:
			return TEXT("Low");
		case ERealityGravityPreset::Zero:
			return TEXT("Zero");
		default:
			return TEXT("Invalid");
		}
	}
}

URealityEditableComponent::URealityEditableComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.TickGroup = TG_PrePhysics;
}

void URealityEditableComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bGravityModified || CurrentGravityPreset != ERealityGravityPreset::Low)
	{
		SetComponentTickEnabled(false);
		return;
	}

	AActor* Owner = GetOwner();
	UWorld* World = GetWorld();
	if (!IsValid(Owner) || !IsValid(World))
	{
		SetComponentTickEnabled(false);
		return;
	}

	const FVector LowGravityAcceleration(0.0f, 0.0f, World->GetGravityZ() * RealityGravity::LowGravityMultiplier);
	for (int32 StateIndex = OriginalGravityStates.Num() - 1; StateIndex >= 0; --StateIndex)
	{
		FRealityOriginalGravityState& OriginalState = OriginalGravityStates[StateIndex];
		UPrimitiveComponent* PrimitiveComponent = OriginalState.PrimitiveComponent.Get();
		if (!IsValid(PrimitiveComponent) || PrimitiveComponent->GetOwner() != Owner)
		{
			OriginalGravityStates.RemoveAtSwap(StateIndex);
			continue;
		}

		if (OriginalState.bGravityEnabled && PrimitiveComponent->IsAnySimulatingPhysics())
		{
			PrimitiveComponent->SetEnableGravity(false);
			PrimitiveComponent->AddForce(LowGravityAcceleration, NAME_None, true);
		}
	}

	if (OriginalGravityStates.IsEmpty())
	{
		bGravityModified = false;
		CurrentGravityPreset = ERealityGravityPreset::Normal;
		SetComponentTickEnabled(false);
	}
}

bool URealityEditableComponent::SupportsCheat(const FGameplayTag CheatTag) const
{
	return CheatTag.IsValid() && SupportedCheats.HasTagExact(CheatTag);
}

FGameplayTagContainer URealityEditableComponent::GetSupportedCheats() const
{
	return SupportedCheats;
}

bool URealityEditableComponent::HasObjectTag(const FGameplayTag ObjectTag) const
{
	return ObjectTag.IsValid() && ObjectTags.HasTagExact(ObjectTag);
}

FGameplayTagContainer URealityEditableComponent::GetObjectTags() const
{
	return ObjectTags;
}

bool URealityEditableComponent::ApplyCollisionModification(AActor* InstigatingActor)
{
	AActor* Owner = GetOwner();
	if (!IsValid(Owner))
	{
		UE_LOG(LogReality, Warning, TEXT("Reality Collision: Apply failed because component '%s' has no valid Actor owner."), *GetNameSafe(this));
		return false;
	}

	if (!SupportsCheat(TAG_Reality_Cheat_Collision))
	{
		UE_LOG(LogReality, Warning, TEXT("Reality Collision: Apply rejected for '%s' because Cheat.Collision is unsupported."), *GetNameSafe(Owner));
		return false;
	}

	if (bCollisionModified)
	{
		UE_LOG(LogReality, Verbose, TEXT("Reality Collision: Apply ignored for '%s' because collision is already modified."), *GetNameSafe(Owner));
		return false;
	}

	TInlineComponentArray<UPrimitiveComponent*> PrimitiveComponents;
	Owner->GetComponents(PrimitiveComponents, false);
	OriginalCollisionStates.Reset(PrimitiveComponents.Num());
	for (UPrimitiveComponent* PrimitiveComponent : PrimitiveComponents)
	{
		if (IsValid(PrimitiveComponent)
			&& PrimitiveComponent->GetOwner() == Owner
			&& PrimitiveComponent->GetCollisionEnabled() != ECollisionEnabled::NoCollision)
		{
			OriginalCollisionStates.Emplace(PrimitiveComponent, PrimitiveComponent->GetCollisionEnabled());
		}
	}

	if (OriginalCollisionStates.IsEmpty())
	{
		UE_LOG(LogReality, Warning, TEXT("Reality Collision: Apply failed for '%s' because it has no owner-local PrimitiveComponents with collision enabled."), *GetNameSafe(Owner));
		return false;
	}

	for (const FRealityOriginalCollisionState& OriginalState : OriginalCollisionStates)
	{
		if (UPrimitiveComponent* PrimitiveComponent = OriginalState.PrimitiveComponent.Get())
		{
			PrimitiveComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}

	bCollisionModified = true;
	const FRealityCheatEvent CheatEvent(Owner, TAG_Reality_Cheat_Collision, InstigatingActor, ERealityCheatOperation::Apply);
	EmitRealityCheatEvent(CheatEvent);
	UE_LOG(LogReality, Log, TEXT("Reality Collision: Disabled collision for '%s' on %d PrimitiveComponent(s)."), *GetNameSafe(Owner), OriginalCollisionStates.Num());
	return true;
}

bool URealityEditableComponent::RestoreCollisionModification(AActor* InstigatingActor)
{
	AActor* Owner = GetOwner();
	if (!IsValid(Owner))
	{
		UE_LOG(LogReality, Warning, TEXT("Reality Collision: Restore failed because component '%s' has no valid Actor owner."), *GetNameSafe(this));
		return false;
	}

	if (!bCollisionModified)
	{
		UE_LOG(LogReality, Verbose, TEXT("Reality Collision: Restore ignored for '%s' because collision is not modified."), *GetNameSafe(Owner));
		return false;
	}

	int32 RestoredComponentCount = 0;
	for (const FRealityOriginalCollisionState& OriginalState : OriginalCollisionStates)
	{
		UPrimitiveComponent* PrimitiveComponent = OriginalState.PrimitiveComponent.Get();
		if (IsValid(PrimitiveComponent) && PrimitiveComponent->GetOwner() == Owner)
		{
			PrimitiveComponent->SetCollisionEnabled(OriginalState.CollisionEnabled);
			++RestoredComponentCount;
		}
	}

	OriginalCollisionStates.Reset();
	bCollisionModified = false;
	if (RestoredComponentCount == 0)
	{
		UE_LOG(LogReality, Warning, TEXT("Reality Collision: Restore for '%s' found no surviving PrimitiveComponents; saved state was cleared without emitting an event."), *GetNameSafe(Owner));
		return false;
	}

	const FRealityCheatEvent CheatEvent(Owner, TAG_Reality_Cheat_Collision, InstigatingActor, ERealityCheatOperation::Restore);
	EmitRealityCheatEvent(CheatEvent);
	UE_LOG(LogReality, Log, TEXT("Reality Collision: Restored collision for '%s' on %d PrimitiveComponent(s)."), *GetNameSafe(Owner), RestoredComponentCount);
	return true;
}

bool URealityEditableComponent::ApplyScaleModification(const ERealityScalePreset Preset, AActor* InstigatingActor)
{
	AActor* Owner = GetOwner();
	if (!IsValid(Owner))
	{
		UE_LOG(LogReality, Warning, TEXT("Reality Scale: Apply failed because component '%s' has no valid Actor owner."), *GetNameSafe(this));
		return false;
	}

	if (!SupportsCheat(TAG_Reality_Cheat_Scale))
	{
		UE_LOG(LogReality, Warning, TEXT("Reality Scale: Apply rejected for '%s' because Cheat.Scale is unsupported."), *GetNameSafe(Owner));
		return false;
	}

	float Multiplier = 0.0f;
	if (!RealityScale::TryGetMultiplier(Preset, Multiplier))
	{
		UE_LOG(LogReality, Warning, TEXT("Reality Scale: Apply rejected for '%s' because preset value %d is invalid."), *GetNameSafe(Owner), static_cast<uint8>(Preset));
		return false;
	}

	const FVector BaselineScale = bScaleModified ? OriginalScale : Owner->GetActorScale3D();
	if (BaselineScale.ContainsNaN())
	{
		UE_LOG(LogReality, Warning, TEXT("Reality Scale: Apply rejected for '%s' because its baseline scale is not finite."), *GetNameSafe(Owner));
		return false;
	}

	const FVector NewScale = BaselineScale * Multiplier;
	if (NewScale.ContainsNaN())
	{
		UE_LOG(LogReality, Warning, TEXT("Reality Scale: Apply rejected for '%s' because preset %s produced a non-finite scale."), *GetNameSafe(Owner), *RealityScale::GetPresetLabel(Preset));
		return false;
	}

	const FVector CurrentScale = Owner->GetActorScale3D();
	if (CurrentScale.Equals(NewScale))
	{
		UE_LOG(LogReality, Verbose, TEXT("Reality Scale: Apply ignored for '%s' because preset %s causes no scale change."), *GetNameSafe(Owner), *RealityScale::GetPresetLabel(Preset));
		return false;
	}

	if (!bScaleModified)
	{
		OriginalScale = BaselineScale;
		bScaleModified = true;
	}

	Owner->SetActorScale3D(NewScale);
	CurrentScalePreset = Preset;
	const FRealityCheatEvent CheatEvent(Owner, TAG_Reality_Cheat_Scale, InstigatingActor, ERealityCheatOperation::Apply);
	EmitRealityCheatEvent(CheatEvent);
	UE_LOG(
		LogReality,
		Log,
		TEXT("Reality Scale: Set '%s' to %s from baseline %s. Current scale=%s."),
		*GetNameSafe(Owner),
		*RealityScale::GetPresetLabel(Preset),
		*OriginalScale.ToString(),
		*NewScale.ToString());
	return true;
}

bool URealityEditableComponent::RestoreScaleModification(AActor* InstigatingActor)
{
	AActor* Owner = GetOwner();
	if (!IsValid(Owner))
	{
		UE_LOG(LogReality, Warning, TEXT("Reality Scale: Restore failed because component '%s' has no valid Actor owner."), *GetNameSafe(this));
		return false;
	}

	if (!bScaleModified)
	{
		UE_LOG(LogReality, Verbose, TEXT("Reality Scale: Restore ignored for '%s' because scale is not modified."), *GetNameSafe(Owner));
		return false;
	}

	if (OriginalScale.ContainsNaN())
	{
		UE_LOG(LogReality, Error, TEXT("Reality Scale: Restore failed for '%s' because its captured scale is not finite; the active cycle remains intact."), *GetNameSafe(Owner));
		return false;
	}

	Owner->SetActorScale3D(OriginalScale);
	const FVector RestoredScale = OriginalScale;
	bScaleModified = false;
	CurrentScalePreset = ERealityScalePreset::One;
	OriginalScale = RestoredScale;

	const FRealityCheatEvent CheatEvent(Owner, TAG_Reality_Cheat_Scale, InstigatingActor, ERealityCheatOperation::Restore);
	EmitRealityCheatEvent(CheatEvent);
	UE_LOG(LogReality, Log, TEXT("Reality Scale: Restored '%s' to exact scale %s."), *GetNameSafe(Owner), *RestoredScale.ToString());
	return true;
}

FVector URealityEditableComponent::GetOriginalScale() const
{
	if (bScaleModified)
	{
		return OriginalScale;
	}

	const AActor* Owner = GetOwner();
	const FVector CurrentScale = IsValid(Owner) ? Owner->GetActorScale3D() : FVector::OneVector;
	return !CurrentScale.ContainsNaN() ? CurrentScale : FVector::OneVector;
}

bool URealityEditableComponent::ApplyGravityModification(const ERealityGravityPreset Preset, AActor* InstigatingActor)
{
	AActor* Owner = GetOwner();
	if (!IsValid(Owner))
	{
		UE_LOG(LogReality, Warning, TEXT("Reality Gravity: Apply failed because component '%s' has no valid Actor owner."), *GetNameSafe(this));
		return false;
	}

	if (!SupportsCheat(TAG_Reality_Cheat_Gravity))
	{
		UE_LOG(LogReality, Warning, TEXT("Reality Gravity: Apply rejected for '%s' because Cheat.Gravity is unsupported."), *GetNameSafe(Owner));
		return false;
	}

	if (!RealityGravity::IsValidPreset(Preset))
	{
		UE_LOG(LogReality, Warning, TEXT("Reality Gravity: Apply rejected for '%s' because preset value %d is invalid."), *GetNameSafe(Owner), static_cast<uint8>(Preset));
		return false;
	}

	if (bGravityModified && CurrentGravityPreset == Preset)
	{
		UE_LOG(LogReality, Verbose, TEXT("Reality Gravity: Apply ignored for '%s' because preset %s is already active."), *GetNameSafe(Owner), RealityGravity::GetPresetLabel(Preset));
		return false;
	}

	if (!bGravityModified)
	{
		if (Preset == ERealityGravityPreset::Normal)
		{
			UE_LOG(LogReality, Verbose, TEXT("Reality Gravity: Normal ignored for '%s' because no Gravity cycle is active."), *GetNameSafe(Owner));
			return false;
		}

		TInlineComponentArray<UPrimitiveComponent*> PrimitiveComponents;
		Owner->GetComponents(PrimitiveComponents, false);
		OriginalGravityStates.Reset(PrimitiveComponents.Num());
		bool bAnyBaselineGravityEnabled = false;
		for (UPrimitiveComponent* PrimitiveComponent : PrimitiveComponents)
		{
			if (IsValid(PrimitiveComponent)
				&& PrimitiveComponent->GetOwner() == Owner
				&& PrimitiveComponent->IsAnySimulatingPhysics())
			{
				const bool bGravityEnabled = PrimitiveComponent->IsGravityEnabled();
				OriginalGravityStates.Emplace(PrimitiveComponent, bGravityEnabled);
				bAnyBaselineGravityEnabled |= bGravityEnabled;
			}
		}

		if (OriginalGravityStates.IsEmpty())
		{
			UE_LOG(LogReality, Warning, TEXT("Reality Gravity: Apply failed for '%s' because it has no directly owned simulated PrimitiveComponents."), *GetNameSafe(Owner));
			return false;
		}

		if (!bAnyBaselineGravityEnabled)
		{
			OriginalGravityStates.Reset();
			UE_LOG(LogReality, Verbose, TEXT("Reality Gravity: Apply ignored for '%s' because all eligible components already have gravity disabled."), *GetNameSafe(Owner));
			return false;
		}

		bGravityModified = true;
	}

	int32 AppliedComponentCount = 0;
	for (int32 StateIndex = OriginalGravityStates.Num() - 1; StateIndex >= 0; --StateIndex)
	{
		FRealityOriginalGravityState& OriginalState = OriginalGravityStates[StateIndex];
		UPrimitiveComponent* PrimitiveComponent = OriginalState.PrimitiveComponent.Get();
		if (!IsValid(PrimitiveComponent) || PrimitiveComponent->GetOwner() != Owner)
		{
			OriginalGravityStates.RemoveAtSwap(StateIndex);
			continue;
		}

		const bool bEnableGravity = Preset == ERealityGravityPreset::Normal
			? OriginalState.bGravityEnabled
			: false;
		PrimitiveComponent->SetEnableGravity(bEnableGravity);
		++AppliedComponentCount;
	}

	if (AppliedComponentCount == 0)
	{
		OriginalGravityStates.Reset();
		bGravityModified = false;
		CurrentGravityPreset = ERealityGravityPreset::Normal;
		SetComponentTickEnabled(false);
		UE_LOG(LogReality, Warning, TEXT("Reality Gravity: Apply failed for '%s' because all captured components became invalid."), *GetNameSafe(Owner));
		return false;
	}

	CurrentGravityPreset = Preset;
	SetComponentTickEnabled(Preset == ERealityGravityPreset::Low);
	const FRealityCheatEvent CheatEvent(Owner, TAG_Reality_Cheat_Gravity, InstigatingActor, ERealityCheatOperation::Apply);
	EmitRealityCheatEvent(CheatEvent);
	UE_LOG(LogReality, Log, TEXT("Reality Gravity: Set '%s' to %s on %d PrimitiveComponent(s). LowForceActive=%s."), *GetNameSafe(Owner), RealityGravity::GetPresetLabel(Preset), AppliedComponentCount, Preset == ERealityGravityPreset::Low ? TEXT("true") : TEXT("false"));
	return true;
}

bool URealityEditableComponent::RestoreGravityModification(AActor* InstigatingActor)
{
	AActor* Owner = GetOwner();
	if (!IsValid(Owner))
	{
		UE_LOG(LogReality, Warning, TEXT("Reality Gravity: Restore failed because component '%s' has no valid Actor owner."), *GetNameSafe(this));
		return false;
	}

	if (!bGravityModified)
	{
		UE_LOG(LogReality, Verbose, TEXT("Reality Gravity: Restore ignored for '%s' because gravity is not modified."), *GetNameSafe(Owner));
		return false;
	}

	SetComponentTickEnabled(false);
	int32 RestoredComponentCount = 0;
	for (const FRealityOriginalGravityState& OriginalState : OriginalGravityStates)
	{
		UPrimitiveComponent* PrimitiveComponent = OriginalState.PrimitiveComponent.Get();
		if (IsValid(PrimitiveComponent) && PrimitiveComponent->GetOwner() == Owner)
		{
			PrimitiveComponent->SetEnableGravity(OriginalState.bGravityEnabled);
			++RestoredComponentCount;
		}
	}

	OriginalGravityStates.Reset();
	bGravityModified = false;
	CurrentGravityPreset = ERealityGravityPreset::Normal;
	if (RestoredComponentCount == 0)
	{
		UE_LOG(LogReality, Warning, TEXT("Reality Gravity: Restore for '%s' found no surviving PrimitiveComponents; saved state was cleared without emitting an event."), *GetNameSafe(Owner));
		return false;
	}

	const FRealityCheatEvent CheatEvent(Owner, TAG_Reality_Cheat_Gravity, InstigatingActor, ERealityCheatOperation::Restore);
	EmitRealityCheatEvent(CheatEvent);
	UE_LOG(LogReality, Log, TEXT("Reality Gravity: Restored exact gravity state for '%s' on %d PrimitiveComponent(s)."), *GetNameSafe(Owner), RestoredComponentCount);
	return true;
}

int32 URealityEditableComponent::GetEligibleGravityComponentCount() const
{
	const AActor* Owner = GetOwner();
	if (!IsValid(Owner))
	{
		return 0;
	}

	TInlineComponentArray<UPrimitiveComponent*> PrimitiveComponents;
	Owner->GetComponents(PrimitiveComponents, false);
	int32 EligibleComponentCount = 0;
	for (const UPrimitiveComponent* PrimitiveComponent : PrimitiveComponents)
	{
		if (IsValid(PrimitiveComponent)
			&& PrimitiveComponent->GetOwner() == Owner
			&& PrimitiveComponent->IsAnySimulatingPhysics())
		{
			++EligibleComponentCount;
		}
	}
	return EligibleComponentCount;
}

void URealityEditableComponent::EmitRealityCheatEvent(const FRealityCheatEvent& CheatEvent)
{
	if (UWorld* World = GetWorld())
	{
		if (URealityManagerSubsystem* RealityManager = World->GetSubsystem<URealityManagerSubsystem>())
		{
			RealityManager->ProcessCheatEvent(CheatEvent);
		}
		else
		{
			UE_LOG(LogReality, Error, TEXT("Reality Editable: Successful event for '%s' could not reach a Reality Manager in world '%s'."), *GetNameSafe(GetOwner()), *GetNameSafe(World));
		}
	}
	else
	{
		UE_LOG(LogReality, Error, TEXT("Reality Editable: Successful event for '%s' has no valid world for Reality Manager routing."), *GetNameSafe(GetOwner()));
	}
	OnRealityCheatEvent.Broadcast(CheatEvent);
}

FString URealityEditableComponent::GetEditableDebugDescription() const
{
	return FString::Printf(
		TEXT("Actor='%s' SupportedCheats=[%s] ObjectTags=[%s] CollisionModified=%s ScaleModified=%s ScalePreset=%s OriginalScale=%s CurrentScale=%s GravityModified=%s GravityPreset=%s EligibleGravityComponents=%d"),
		*GetNameSafe(GetOwner()),
		*SupportedCheats.ToStringSimple(),
		*ObjectTags.ToStringSimple(),
		bCollisionModified ? TEXT("true") : TEXT("false"),
		bScaleModified ? TEXT("true") : TEXT("false"),
		*RealityScale::GetPresetLabel(CurrentScalePreset),
		*GetOriginalScale().ToString(),
		IsValid(GetOwner()) ? *GetOwner()->GetActorScale3D().ToString() : TEXT("Invalid"),
		bGravityModified ? TEXT("true") : TEXT("false"),
		RealityGravity::GetPresetLabel(CurrentGravityPreset),
		GetEligibleGravityComponentCount());
}

void URealityEditableComponent::LogEditableConfiguration() const
{
#if !UE_BUILD_SHIPPING
	if (!IsValid(GetOwner()))
	{
		UE_LOG(LogReality, Warning, TEXT("Reality Editable: Component '%s' has no valid Actor owner."), *GetNameSafe(this));
		return;
	}

	UE_LOG(LogReality, Log, TEXT("Reality Editable: %s"), *GetEditableDebugDescription());
#endif
}

void URealityEditableComponent::SetSupportedCheats(const FGameplayTagContainer& InSupportedCheats)
{
	SupportedCheats = InSupportedCheats;
}

void URealityEditableComponent::SetObjectTags(const FGameplayTagContainer& InObjectTags)
{
	ObjectTags = InObjectTags;
}
