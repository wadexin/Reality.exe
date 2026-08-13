// Copyright Epic Games, Inc. All Rights Reserved.

#include "Developer/RealityEditableComponent.h"

#include "Components/PrimitiveComponent.h"
#include "GameFramework/Actor.h"
#include "NativeGameplayTags.h"
#include "PhysicsEngine/BodyInstance.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Reality.h"
#include "RealitySystem/RealityManagerSubsystem.h"

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Reality_Cheat_Collision, "Cheat.Collision");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Reality_Cheat_Scale, "Cheat.Scale");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Reality_Cheat_Gravity, "Cheat.Gravity");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Reality_Cheat_Mass, "Cheat.Mass");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Reality_Cheat_Friction, "Cheat.Friction");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Reality_Cheat_Time, "Cheat.Time");

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

namespace RealityMass
{
	bool TryGetMultiplier(const ERealityMassPreset Preset, float& OutMultiplier)
	{
		switch (Preset)
		{
		case ERealityMassPreset::Quarter: OutMultiplier = 0.25f; return true;
		case ERealityMassPreset::Half: OutMultiplier = 0.5f; return true;
		case ERealityMassPreset::One: OutMultiplier = 1.0f; return true;
		case ERealityMassPreset::Double: OutMultiplier = 2.0f; return true;
		case ERealityMassPreset::Quadruple: OutMultiplier = 4.0f; return true;
		default: return false;
		}
	}

	FString GetPresetLabel(const ERealityMassPreset Preset)
	{
		float Multiplier = 0.0f;
		return TryGetMultiplier(Preset, Multiplier) ? FString::Printf(TEXT("%gx"), Multiplier) : TEXT("Invalid");
	}
}

namespace RealityFriction
{
	bool TryGetMultiplier(const ERealityFrictionPreset Preset, float& OutMultiplier)
	{
		switch (Preset)
		{
		case ERealityFrictionPreset::Zero: OutMultiplier = 0.0f; return true;
		case ERealityFrictionPreset::Low: OutMultiplier = 0.25f; return true;
		case ERealityFrictionPreset::Normal: OutMultiplier = 1.0f; return true;
		case ERealityFrictionPreset::High: OutMultiplier = 4.0f; return true;
		default: return false;
		}
	}

	const TCHAR* GetPresetLabel(const ERealityFrictionPreset Preset)
	{
		switch (Preset)
		{
		case ERealityFrictionPreset::Zero: return TEXT("Zero");
		case ERealityFrictionPreset::Low: return TEXT("Low");
		case ERealityFrictionPreset::Normal: return TEXT("Normal");
		case ERealityFrictionPreset::High: return TEXT("High");
		default: return TEXT("Invalid");
		}
	}
}

namespace RealityTime
{
	bool TryGetMultiplier(const ERealityTimePreset Preset, float& OutMultiplier)
	{
		switch (Preset)
		{
		case ERealityTimePreset::Quarter: OutMultiplier = 0.25f; return true;
		case ERealityTimePreset::Half: OutMultiplier = 0.5f; return true;
		case ERealityTimePreset::One: OutMultiplier = 1.0f; return true;
		case ERealityTimePreset::Double: OutMultiplier = 2.0f; return true;
		case ERealityTimePreset::Quadruple: OutMultiplier = 4.0f; return true;
		default: return false;
		}
	}

	FString GetPresetLabel(const ERealityTimePreset Preset)
	{
		float Multiplier = 0.0f;
		return TryGetMultiplier(Preset, Multiplier) ? FString::Printf(TEXT("%gx"), Multiplier) : TEXT("Invalid");
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
	ReapplyActiveMassPreset();
	ReapplyActiveFrictionPreset();

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
	ReapplyActiveMassPreset();
	ReapplyActiveFrictionPreset();
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
	ReapplyActiveMassPreset();
	ReapplyActiveFrictionPreset();
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

bool URealityEditableComponent::ApplyMassModification(const ERealityMassPreset Preset, AActor* InstigatingActor)
{
	AActor* Owner = GetOwner();
	if (!IsValid(Owner) || !SupportsCheat(TAG_Reality_Cheat_Mass))
	{
		UE_LOG(LogReality, Warning, TEXT("Reality Mass: Apply rejected for '%s': invalid owner or unsupported Cheat.Mass."), *GetNameSafe(Owner));
		return false;
	}

	float Multiplier = 0.0f;
	if (!RealityMass::TryGetMultiplier(Preset, Multiplier))
	{
		UE_LOG(LogReality, Warning, TEXT("Reality Mass: Apply rejected for '%s' because preset value %d is invalid."), *GetNameSafe(Owner), static_cast<uint8>(Preset));
		return false;
	}
	if (bMassModified && CurrentMassPreset == Preset)
	{
		return false;
	}
	if (!bMassModified)
	{
		if (Preset == ERealityMassPreset::One)
		{
			return false;
		}

		TInlineComponentArray<UPrimitiveComponent*> PrimitiveComponents;
		Owner->GetComponents(PrimitiveComponents, false);
		OriginalMassStates.Reset(PrimitiveComponents.Num());
		for (UPrimitiveComponent* Primitive : PrimitiveComponents)
		{
			FBodyInstance* BodyInstance = IsValid(Primitive) ? Primitive->GetBodyInstance() : nullptr;
			if (Primitive && Primitive->GetOwner() == Owner && Primitive->IsAnySimulatingPhysics() && BodyInstance)
			{
				const float EffectiveMass = Primitive->GetMass();
				if (FMath::IsFinite(EffectiveMass) && EffectiveMass > 0.0f)
				{
					FRealityOriginalMassState& State = OriginalMassStates.AddDefaulted_GetRef();
					State.PrimitiveComponent = Primitive;
					State.BaselineEffectiveMassKg = EffectiveMass;
					State.OriginalMassScale = BodyInstance->MassScale;
					State.OriginalMassOverrideKg = BodyInstance->GetMassOverride();
					State.bOriginallyOverrodeMass = BodyInstance->bOverrideMass;
				}
			}
		}
		if (OriginalMassStates.IsEmpty())
		{
			UE_LOG(LogReality, Warning, TEXT("Reality Mass: Apply failed for '%s' because it has no directly owned simulated PrimitiveComponents with valid mass."), *GetNameSafe(Owner));
			return false;
		}
		bMassModified = true;
	}

	CurrentMassPreset = Preset;
	ReapplyActiveMassPreset();
	if (!bMassModified)
	{
		return false;
	}

	EmitRealityCheatEvent(FRealityCheatEvent(Owner, TAG_Reality_Cheat_Mass, InstigatingActor, ERealityCheatOperation::Apply));
	UE_LOG(LogReality, Log, TEXT("Reality Mass: Set '%s' to %s across %d body component(s). Baseline=%.3fkg Current=%.3fkg."),
		*GetNameSafe(Owner), *RealityMass::GetPresetLabel(Preset), OriginalMassStates.Num(), GetBaselineEffectiveMassKg(), GetCurrentEffectiveMassKg());
	return true;
}

bool URealityEditableComponent::RestoreMassModification(AActor* InstigatingActor)
{
	AActor* Owner = GetOwner();
	if (!IsValid(Owner) || !bMassModified)
	{
		return false;
	}

	int32 RestoredCount = 0;
	for (const FRealityOriginalMassState& State : OriginalMassStates)
	{
		UPrimitiveComponent* Primitive = State.PrimitiveComponent.Get();
		if (IsValid(Primitive) && Primitive->GetOwner() == Owner)
		{
			Primitive->SetMassScale(NAME_None, State.OriginalMassScale);
			Primitive->SetMassOverrideInKg(NAME_None, State.OriginalMassOverrideKg, State.bOriginallyOverrodeMass);
			++RestoredCount;
		}
	}

	OriginalMassStates.Reset();
	bMassModified = false;
	CurrentMassPreset = ERealityMassPreset::One;
	if (RestoredCount == 0)
	{
		UE_LOG(LogReality, Warning, TEXT("Reality Mass: Restore for '%s' found no surviving components; state cleared without an event."), *GetNameSafe(Owner));
		return false;
	}

	EmitRealityCheatEvent(FRealityCheatEvent(Owner, TAG_Reality_Cheat_Mass, InstigatingActor, ERealityCheatOperation::Restore));
	UE_LOG(LogReality, Log, TEXT("Reality Mass: Restored exact authored mass configuration for '%s' on %d component(s)."), *GetNameSafe(Owner), RestoredCount);
	return true;
}

void URealityEditableComponent::ReapplyActiveMassPreset()
{
	if (!bMassModified)
	{
		return;
	}
	float Multiplier = 0.0f;
	if (!RealityMass::TryGetMultiplier(CurrentMassPreset, Multiplier))
	{
		return;
	}
	AActor* Owner = GetOwner();
	for (int32 Index = OriginalMassStates.Num() - 1; Index >= 0; --Index)
	{
		UPrimitiveComponent* Primitive = OriginalMassStates[Index].PrimitiveComponent.Get();
		if (!IsValid(Primitive) || Primitive->GetOwner() != Owner)
		{
			OriginalMassStates.RemoveAtSwap(Index);
			continue;
		}
		Primitive->SetMassOverrideInKg(NAME_None, FMath::Max(0.001f, OriginalMassStates[Index].BaselineEffectiveMassKg * Multiplier), true);
	}
	if (OriginalMassStates.IsEmpty())
	{
		bMassModified = false;
		CurrentMassPreset = ERealityMassPreset::One;
	}
}

int32 URealityEditableComponent::GetEligibleMassComponentCount() const
{
	const AActor* Owner = GetOwner();
	if (!IsValid(Owner)) return 0;
	TInlineComponentArray<UPrimitiveComponent*> Components;
	Owner->GetComponents(Components, false);
	int32 EligibleCount = 0;
	for (const UPrimitiveComponent* Primitive : Components)
	{
		if (IsValid(Primitive) && Primitive->GetOwner() == Owner && Primitive->IsAnySimulatingPhysics() && Primitive->GetBodyInstance())
		{
			++EligibleCount;
		}
	}
	return EligibleCount;
}

float URealityEditableComponent::GetBaselineEffectiveMassKg() const
{
	if (!bMassModified) return GetCurrentEffectiveMassKg();
	float Total = 0.0f;
	for (const FRealityOriginalMassState& State : OriginalMassStates)
	{
		if (State.PrimitiveComponent.IsValid()) Total += State.BaselineEffectiveMassKg;
	}
	return Total;
}

float URealityEditableComponent::GetCurrentEffectiveMassKg() const
{
	const AActor* Owner = GetOwner();
	if (!IsValid(Owner)) return 0.0f;
	float Total = 0.0f;
	if (bMassModified)
	{
		float Multiplier = 0.0f;
		if (!RealityMass::TryGetMultiplier(CurrentMassPreset, Multiplier)) return 0.0f;
		for (const FRealityOriginalMassState& State : OriginalMassStates)
		{
			if (State.PrimitiveComponent.IsValid()) Total += State.BaselineEffectiveMassKg * Multiplier;
		}
		return Total;
	}
	TInlineComponentArray<UPrimitiveComponent*> Components;
	Owner->GetComponents(Components, false);
	for (const UPrimitiveComponent* Primitive : Components)
	{
		if (IsValid(Primitive) && Primitive->GetOwner() == Owner && Primitive->IsAnySimulatingPhysics() && Primitive->GetBodyInstance()) Total += Primitive->GetMass();
	}
	return Total;
}

bool URealityEditableComponent::ApplyFrictionModification(const ERealityFrictionPreset Preset, AActor* InstigatingActor)
{
	AActor* Owner = GetOwner();
	if (!IsValid(Owner) || !SupportsCheat(TAG_Reality_Cheat_Friction))
	{
		UE_LOG(LogReality, Warning, TEXT("Reality Friction: Apply rejected for '%s': invalid owner or unsupported Cheat.Friction."), *GetNameSafe(Owner));
		return false;
	}

	float Multiplier = 0.0f;
	if (!RealityFriction::TryGetMultiplier(Preset, Multiplier))
	{
		UE_LOG(LogReality, Warning, TEXT("Reality Friction: Apply rejected for '%s' because preset value %d is invalid."), *GetNameSafe(Owner), static_cast<uint8>(Preset));
		return false;
	}
	if (bFrictionModified && CurrentFrictionPreset == Preset)
	{
		return false;
	}

	if (!bFrictionModified)
	{
		if (Preset == ERealityFrictionPreset::Normal)
		{
			return false;
		}

		TInlineComponentArray<UPrimitiveComponent*> PrimitiveComponents;
		Owner->GetComponents(PrimitiveComponents, false);
		OriginalFrictionStates.Reset(PrimitiveComponents.Num());
		for (UPrimitiveComponent* Primitive : PrimitiveComponents)
		{
			FBodyInstance* BodyInstance = IsValid(Primitive) ? Primitive->GetBodyInstance() : nullptr;
			if (Primitive && Primitive->GetOwner() == Owner && Primitive->IsAnySimulatingPhysics() && BodyInstance)
			{
				UPhysicalMaterial* EffectiveMaterial = BodyInstance->GetSimplePhysicalMaterial();
				if (IsValid(EffectiveMaterial))
				{
					FRealityOriginalFrictionState& State = OriginalFrictionStates.AddDefaulted_GetRef();
					State.PrimitiveComponent = Primitive;
					State.OriginalPhysMaterialOverride = BodyInstance->GetPhysMaterialOverride();
					State.BaselinePhysMaterial = EffectiveMaterial;
					State.BaselineDynamicFriction = FMath::Max(0.0f, EffectiveMaterial->Friction);
					State.BaselineStaticFriction = FMath::Max(0.0f, EffectiveMaterial->StaticFriction);
				}
			}
		}
		if (OriginalFrictionStates.IsEmpty())
		{
			UE_LOG(LogReality, Warning, TEXT("Reality Friction: Apply failed for '%s' because it has no directly owned simulated PrimitiveComponents with a valid Physical Material."), *GetNameSafe(Owner));
			return false;
		}
		bFrictionModified = true;
	}

	CurrentFrictionPreset = Preset;
	ReapplyActiveFrictionPreset();
	if (!bFrictionModified)
	{
		return false;
	}

	EmitRealityCheatEvent(FRealityCheatEvent(Owner, TAG_Reality_Cheat_Friction, InstigatingActor, ERealityCheatOperation::Apply));
	UE_LOG(LogReality, Log, TEXT("Reality Friction: Set '%s' to %s across %d component(s). Baseline=%.3f Current=%.3f."),
		*GetNameSafe(Owner), RealityFriction::GetPresetLabel(Preset), OriginalFrictionStates.Num(), GetBaselineFriction(), GetCurrentFriction());
	return true;
}

bool URealityEditableComponent::RestoreFrictionModification(AActor* InstigatingActor)
{
	AActor* Owner = GetOwner();
	if (!IsValid(Owner) || !bFrictionModified)
	{
		return false;
	}

	int32 RestoredCount = 0;
	for (FRealityOriginalFrictionState& State : OriginalFrictionStates)
	{
		UPrimitiveComponent* Primitive = State.PrimitiveComponent.Get();
		if (IsValid(Primitive) && Primitive->GetOwner() == Owner)
		{
			Primitive->SetPhysMaterialOverride(State.OriginalPhysMaterialOverride);
			State.RuntimePhysMaterial = nullptr;
			++RestoredCount;
		}
	}

	OriginalFrictionStates.Reset();
	bFrictionModified = false;
	CurrentFrictionPreset = ERealityFrictionPreset::Normal;
	ReapplyActiveMassPreset();
	if (RestoredCount == 0)
	{
		UE_LOG(LogReality, Warning, TEXT("Reality Friction: Restore for '%s' found no surviving components; state cleared without an event."), *GetNameSafe(Owner));
		return false;
	}

	EmitRealityCheatEvent(FRealityCheatEvent(Owner, TAG_Reality_Cheat_Friction, InstigatingActor, ERealityCheatOperation::Restore));
	UE_LOG(LogReality, Log, TEXT("Reality Friction: Restored exact Physical Material override configuration for '%s' on %d component(s)."), *GetNameSafe(Owner), RestoredCount);
	return true;
}

void URealityEditableComponent::ReapplyActiveFrictionPreset()
{
	if (!bFrictionModified)
	{
		return;
	}

	float Multiplier = 0.0f;
	if (!RealityFriction::TryGetMultiplier(CurrentFrictionPreset, Multiplier))
	{
		return;
	}
	AActor* Owner = GetOwner();
	for (int32 Index = OriginalFrictionStates.Num() - 1; Index >= 0; --Index)
	{
		FRealityOriginalFrictionState& State = OriginalFrictionStates[Index];
		UPrimitiveComponent* Primitive = State.PrimitiveComponent.Get();
		if (!IsValid(Primitive) || Primitive->GetOwner() != Owner)
		{
			OriginalFrictionStates.RemoveAtSwap(Index);
			continue;
		}

		if (CurrentFrictionPreset == ERealityFrictionPreset::Normal)
		{
			Primitive->SetPhysMaterialOverride(State.OriginalPhysMaterialOverride);
			State.RuntimePhysMaterial = nullptr;
			continue;
		}

		UPhysicalMaterial* SourceMaterial = State.BaselinePhysMaterial;
		if (!IsValid(SourceMaterial))
		{
			OriginalFrictionStates.RemoveAtSwap(Index);
			continue;
		}

		UPhysicalMaterial* RuntimeMaterial = DuplicateObject<UPhysicalMaterial>(SourceMaterial, this);
		if (!IsValid(RuntimeMaterial))
		{
			OriginalFrictionStates.RemoveAtSwap(Index);
			continue;
		}
		RuntimeMaterial->SetFlags(RF_Transient);
		RuntimeMaterial->Friction = State.BaselineDynamicFriction * Multiplier;
		RuntimeMaterial->StaticFriction = State.BaselineStaticFriction * Multiplier;
		State.RuntimePhysMaterial = RuntimeMaterial;
		Primitive->SetPhysMaterialOverride(RuntimeMaterial);
	}

	if (OriginalFrictionStates.IsEmpty())
	{
		bFrictionModified = false;
		CurrentFrictionPreset = ERealityFrictionPreset::Normal;
	}
	ReapplyActiveMassPreset();
}

int32 URealityEditableComponent::GetEligibleFrictionComponentCount() const
{
	const AActor* Owner = GetOwner();
	if (!IsValid(Owner)) return 0;
	TInlineComponentArray<UPrimitiveComponent*> Components;
	Owner->GetComponents(Components, false);
	int32 Count = 0;
	for (const UPrimitiveComponent* Primitive : Components)
	{
		const FBodyInstance* BodyInstance = IsValid(Primitive) ? Primitive->GetBodyInstance() : nullptr;
		if (Primitive && Primitive->GetOwner() == Owner && Primitive->IsAnySimulatingPhysics() && BodyInstance && BodyInstance->GetSimplePhysicalMaterial()) ++Count;
	}
	return Count;
}

float URealityEditableComponent::GetBaselineFriction() const
{
	if (!bFrictionModified)
	{
		const AActor* Owner = GetOwner();
		if (!IsValid(Owner)) return 0.0f;
		TInlineComponentArray<UPrimitiveComponent*> Components;
		Owner->GetComponents(Components, false);
		float Total = 0.0f;
		int32 Count = 0;
		for (const UPrimitiveComponent* Primitive : Components)
		{
			const FBodyInstance* Body = IsValid(Primitive) ? Primitive->GetBodyInstance() : nullptr;
			if (Primitive && Primitive->GetOwner() == Owner && Primitive->IsAnySimulatingPhysics() && Body)
			{
				if (const UPhysicalMaterial* Material = Body->GetSimplePhysicalMaterial()) { Total += Material->Friction; ++Count; }
			}
		}
		return Count > 0 ? Total / Count : 0.0f;
	}
	float Total = 0.0f;
	int32 Count = 0;
	for (const FRealityOriginalFrictionState& State : OriginalFrictionStates)
	{
		if (State.PrimitiveComponent.IsValid()) { Total += State.BaselineDynamicFriction; ++Count; }
	}
	return Count > 0 ? Total / Count : 0.0f;
}

float URealityEditableComponent::GetCurrentFriction() const
{
	float Multiplier = 1.0f;
	if (bFrictionModified && RealityFriction::TryGetMultiplier(CurrentFrictionPreset, Multiplier)) return GetBaselineFriction() * Multiplier;
	return GetBaselineFriction();
}

bool URealityEditableComponent::ApplyTimeModification(const ERealityTimePreset Preset, AActor* InstigatingActor)
{
	AActor* Owner = GetOwner();
	if (!IsValid(Owner) || !SupportsCheat(TAG_Reality_Cheat_Time))
	{
		UE_LOG(LogReality, Warning, TEXT("Reality Time: Apply rejected for '%s': invalid owner or unsupported Cheat.Time."), *GetNameSafe(Owner));
		return false;
	}

	float Multiplier = 0.0f;
	if (!RealityTime::TryGetMultiplier(Preset, Multiplier))
	{
		UE_LOG(LogReality, Warning, TEXT("Reality Time: Apply rejected for '%s' because preset value %d is invalid."), *GetNameSafe(Owner), static_cast<uint8>(Preset));
		return false;
	}
	if (bTimeModified && CurrentTimePreset == Preset)
	{
		return false;
	}

	const float Baseline = bTimeModified ? OriginalTimeDilation : Owner->CustomTimeDilation;
	if (!FMath::IsFinite(Baseline) || Baseline < 0.0f)
	{
		UE_LOG(LogReality, Warning, TEXT("Reality Time: Apply rejected for '%s' because baseline CustomTimeDilation %.6f is invalid."), *GetNameSafe(Owner), Baseline);
		return false;
	}
	if (!bTimeModified && Preset == ERealityTimePreset::One)
	{
		return false;
	}

	const float NewDilation = Baseline * Multiplier;
	if (!FMath::IsFinite(NewDilation) || NewDilation < 0.0f)
	{
		UE_LOG(LogReality, Warning, TEXT("Reality Time: Apply rejected for '%s' because preset produced invalid dilation %.6f."), *GetNameSafe(Owner), NewDilation);
		return false;
	}
	if (!bTimeModified)
	{
		OriginalTimeDilation = Baseline;
		bTimeModified = true;
	}

	Owner->CustomTimeDilation = NewDilation;
	CurrentTimePreset = Preset;
	EmitRealityCheatEvent(FRealityCheatEvent(Owner, TAG_Reality_Cheat_Time, InstigatingActor, ERealityCheatOperation::Apply));
	UE_LOG(LogReality, Log, TEXT("Reality Time: Set '%s' to %s. BaselineLocal=%.3f CurrentLocal=%.3f. Actor/component Tick only; Chaos and world timers are unaffected."),
		*GetNameSafe(Owner), *RealityTime::GetPresetLabel(Preset), OriginalTimeDilation, Owner->CustomTimeDilation);
	return true;
}

bool URealityEditableComponent::RestoreTimeModification(AActor* InstigatingActor)
{
	AActor* Owner = GetOwner();
	if (!IsValid(Owner) || !bTimeModified)
	{
		return false;
	}
	if (!FMath::IsFinite(OriginalTimeDilation) || OriginalTimeDilation < 0.0f)
	{
		UE_LOG(LogReality, Error, TEXT("Reality Time: Restore failed for '%s' because captured dilation %.6f is invalid; cycle remains active."), *GetNameSafe(Owner), OriginalTimeDilation);
		return false;
	}

	Owner->CustomTimeDilation = OriginalTimeDilation;
	bTimeModified = false;
	CurrentTimePreset = ERealityTimePreset::One;
	EmitRealityCheatEvent(FRealityCheatEvent(Owner, TAG_Reality_Cheat_Time, InstigatingActor, ERealityCheatOperation::Restore));
	UE_LOG(LogReality, Log, TEXT("Reality Time: Restored '%s' to exact local dilation %.3f."), *GetNameSafe(Owner), Owner->CustomTimeDilation);
	return true;
}

float URealityEditableComponent::GetOriginalTimeDilation() const
{
	if (bTimeModified) return OriginalTimeDilation;
	const AActor* Owner = GetOwner();
	return IsValid(Owner) && FMath::IsFinite(Owner->CustomTimeDilation) ? Owner->CustomTimeDilation : 1.0f;
}

float URealityEditableComponent::GetCurrentEffectiveTimeDilation() const
{
	const AActor* Owner = GetOwner();
	return IsValid(Owner) && FMath::IsFinite(Owner->CustomTimeDilation) ? Owner->CustomTimeDilation : 1.0f;
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
		TEXT("Actor='%s' SupportedCheats=[%s] ObjectTags=[%s] CollisionModified=%s ScaleModified=%s ScalePreset=%s OriginalScale=%s CurrentScale=%s GravityModified=%s GravityPreset=%s EligibleGravityComponents=%d MassModified=%s MassPreset=%s BaselineMass=%.3fkg CurrentMass=%.3fkg EligibleMassComponents=%d FrictionModified=%s FrictionPreset=%s BaselineFriction=%.3f CurrentFriction=%.3f EligibleFrictionComponents=%d TimeModified=%s TimePreset=%s BaselineLocalTime=%.3f CurrentLocalTime=%.3f"),
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
		GetEligibleGravityComponentCount(),
		bMassModified ? TEXT("true") : TEXT("false"),
		*RealityMass::GetPresetLabel(CurrentMassPreset),
		GetBaselineEffectiveMassKg(),
		GetCurrentEffectiveMassKg(),
		GetEligibleMassComponentCount(),
		bFrictionModified ? TEXT("true") : TEXT("false"),
		RealityFriction::GetPresetLabel(CurrentFrictionPreset),
		GetBaselineFriction(),
		GetCurrentFriction(),
		GetEligibleFrictionComponentCount(),
		bTimeModified ? TEXT("true") : TEXT("false"),
		*RealityTime::GetPresetLabel(CurrentTimePreset),
		GetOriginalTimeDilation(),
		GetCurrentEffectiveTimeDilation());
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
