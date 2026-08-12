// Copyright Epic Games, Inc. All Rights Reserved.

#include "Developer/RealityEditableComponent.h"

#include "Components/PrimitiveComponent.h"
#include "GameFramework/Actor.h"
#include "NativeGameplayTags.h"
#include "Reality.h"

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Reality_Cheat_Collision, "Cheat.Collision");

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

FString URealityEditableComponent::GetEditableDebugDescription() const
{
	return FString::Printf(
		TEXT("Actor='%s' SupportedCheats=[%s] ObjectTags=[%s] CollisionModified=%s"),
		*GetNameSafe(GetOwner()),
		*SupportedCheats.ToStringSimple(),
		*ObjectTags.ToStringSimple(),
		bCollisionModified ? TEXT("true") : TEXT("false"));
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
