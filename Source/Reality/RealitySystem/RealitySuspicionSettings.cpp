// Copyright Epic Games, Inc. All Rights Reserved.

#include "RealitySystem/RealitySuspicionSettings.h"

#include "NativeGameplayTags.h"

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Reality_Settings_Collision, "Cheat.Collision");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Reality_Settings_Scale, "Cheat.Scale");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Reality_Settings_Gravity, "Cheat.Gravity");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Reality_Settings_Mass, "Cheat.Mass");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Reality_Settings_Friction, "Cheat.Friction");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Reality_Settings_Time, "Cheat.Time");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Reality_Settings_Witness_Human, "Witness.Human");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Reality_Settings_Witness_Camera, "Witness.Camera");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Reality_Settings_Context_Explosion, "Context.Explosion");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Reality_Settings_Context_SensorFailure, "Context.SensorFailure");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Reality_Settings_Context_Fire, "Context.Fire");

URealitySuspicionSettings::URealitySuspicionSettings()
{
	CheatSuspicionRules = {
		FRealityCheatSuspicionRule(TAG_Reality_Settings_Collision, 25.0f),
		FRealityCheatSuspicionRule(TAG_Reality_Settings_Scale, 20.0f),
		FRealityCheatSuspicionRule(TAG_Reality_Settings_Gravity, 15.0f),
		FRealityCheatSuspicionRule(TAG_Reality_Settings_Mass, 10.0f),
		FRealityCheatSuspicionRule(TAG_Reality_Settings_Friction, 10.0f),
		FRealityCheatSuspicionRule(TAG_Reality_Settings_Time, 20.0f)
	};
	WitnessSuspicionRules = {
		FRealityWitnessSuspicionRule(TAG_Reality_Settings_Witness_Human, 10.0f),
		FRealityWitnessSuspicionRule(TAG_Reality_Settings_Witness_Camera, 15.0f)
	};
	ContextPlausibilityRules = {
		FRealityContextPlausibilityRule(TAG_Reality_Settings_Gravity, TAG_Reality_Settings_Context_Explosion, 15.0f),
		FRealityContextPlausibilityRule(TAG_Reality_Settings_Scale, TAG_Reality_Settings_Context_Explosion, 10.0f),
		FRealityContextPlausibilityRule(TAG_Reality_Settings_Collision, TAG_Reality_Settings_Context_SensorFailure, 15.0f),
		FRealityContextPlausibilityRule(TAG_Reality_Settings_Gravity, TAG_Reality_Settings_Context_Fire, 5.0f)
	};
}

bool URealitySuspicionSettings::FindContextReduction(const FGameplayTag CheatTag, const FGameplayTag ContextTag, float& OutSuspicionReduction) const
{
	bool bFoundRule = false;
	float StrongestReduction = 0.0f;
	for (const FRealityContextPlausibilityRule& Rule : ContextPlausibilityRules)
	{
		if (Rule.CheatTag.MatchesTagExact(CheatTag) && Rule.ContextTag.MatchesTagExact(ContextTag))
		{
			bFoundRule = true;
			StrongestReduction = FMath::Max(StrongestReduction, Rule.SuspicionReduction);
		}
	}
	OutSuspicionReduction = FMath::Clamp(StrongestReduction, 0.0f, 100.0f);
	return bFoundRule;
}

bool URealitySuspicionSettings::FindWitnessSuspicion(const FGameplayTag WitnessType, float& OutSuspicionValue) const
{
	const FRealityWitnessSuspicionRule* MatchingRule = WitnessSuspicionRules.FindByPredicate(
		[WitnessType](const FRealityWitnessSuspicionRule& Rule)
		{
			return Rule.WitnessType.IsValid() && Rule.WitnessType.MatchesTagExact(WitnessType);
		});
	if (!MatchingRule)
	{
		return false;
	}

	OutSuspicionValue = FMath::Clamp(MatchingRule->SuspicionValue, 0.0f, 100.0f);
	return true;
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
