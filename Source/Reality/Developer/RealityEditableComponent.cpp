// Copyright Epic Games, Inc. All Rights Reserved.

#include "Developer/RealityEditableComponent.h"

#include "GameFramework/Actor.h"
#include "Reality.h"

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

FString URealityEditableComponent::GetEditableDebugDescription() const
{
	return FString::Printf(
		TEXT("Actor='%s' SupportedCheats=[%s] ObjectTags=[%s]"),
		*GetNameSafe(GetOwner()),
		*SupportedCheats.ToStringSimple(),
		*ObjectTags.ToStringSimple());
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
