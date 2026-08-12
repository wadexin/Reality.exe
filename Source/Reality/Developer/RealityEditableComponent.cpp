// Copyright Epic Games, Inc. All Rights Reserved.

#include "Developer/RealityEditableComponent.h"

#include "Components/PrimitiveComponent.h"
#include "GameFramework/Actor.h"
#include "NativeGameplayTags.h"
#include "Reality.h"

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Reality_Cheat_Collision, "Cheat.Collision");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Reality_Cheat_Scale, "Cheat.Scale");

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

URealityEditableComponent::URealityEditableComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
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
	OnRealityCheatEvent.Broadcast(CheatEvent);
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
	OnRealityCheatEvent.Broadcast(CheatEvent);
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
	OnRealityCheatEvent.Broadcast(CheatEvent);
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
	OnRealityCheatEvent.Broadcast(CheatEvent);
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

FString URealityEditableComponent::GetEditableDebugDescription() const
{
	return FString::Printf(
		TEXT("Actor='%s' SupportedCheats=[%s] ObjectTags=[%s] CollisionModified=%s ScaleModified=%s ScalePreset=%s OriginalScale=%s CurrentScale=%s"),
		*GetNameSafe(GetOwner()),
		*SupportedCheats.ToStringSimple(),
		*ObjectTags.ToStringSimple(),
		bCollisionModified ? TEXT("true") : TEXT("false"),
		bScaleModified ? TEXT("true") : TEXT("false"),
		*RealityScale::GetPresetLabel(CurrentScalePreset),
		*GetOriginalScale().ToString(),
		IsValid(GetOwner()) ? *GetOwner()->GetActorScale3D().ToString() : TEXT("Invalid"));
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
