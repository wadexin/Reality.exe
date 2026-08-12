// Copyright Epic Games, Inc. All Rights Reserved.

#include "RealitySystem/RealitySuspicionSettings.h"

#include "NativeGameplayTags.h"

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Reality_Settings_Collision, "Cheat.Collision");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Reality_Settings_Scale, "Cheat.Scale");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Reality_Settings_Gravity, "Cheat.Gravity");

URealitySuspicionSettings::URealitySuspicionSettings()
{
	CheatSuspicionRules = {
		FRealityCheatSuspicionRule(TAG_Reality_Settings_Collision, 25.0f),
		FRealityCheatSuspicionRule(TAG_Reality_Settings_Scale, 20.0f),
		FRealityCheatSuspicionRule(TAG_Reality_Settings_Gravity, 15.0f)
	};
}

bool URealitySuspicionSettings::FindBaseSuspicion(const FGameplayTag CheatTag, float& OutSuspicionValue) const
{
	const FRealityCheatSuspicionRule* MatchingRule = CheatSuspicionRules.FindByPredicate(
		[CheatTag](const FRealityCheatSuspicionRule& Rule)
		{
			return Rule.CheatTag.IsValid() && Rule.CheatTag.MatchesTagExact(CheatTag);
		});
	if (!MatchingRule)
	{
		return false;
	}

	OutSuspicionValue = FMath::Clamp(MatchingRule->SuspicionValue, 0.0f, 100.0f);
	return true;
}
