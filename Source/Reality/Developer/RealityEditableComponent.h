// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "RealityEditableComponent.generated.h"

/**
 * Marks its owning actor as Reality-editable and declares its supported cheat capabilities and object tags.
 * This component owns classification data only; typed Apply and Restore behavior is added by later tasks.
 */
UCLASS(ClassGroup = (Reality), BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class REALITY_API URealityEditableComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	URealityEditableComponent();

	/** Returns true when CheatTag is valid and configured as an exact supported capability. */
	UFUNCTION(BlueprintPure, Category = "Reality|Editable")
	bool SupportsCheat(FGameplayTag CheatTag) const;

	/** Returns a copy of the configured cheat capabilities for safe C++ and Blueprint inspection. */
	UFUNCTION(BlueprintPure, Category = "Reality|Editable")
	FGameplayTagContainer GetSupportedCheats() const;

	/** Returns true when ObjectTag is valid and configured as an exact semantic object tag. */
	UFUNCTION(BlueprintPure, Category = "Reality|Editable")
	bool HasObjectTag(FGameplayTag ObjectTag) const;

	/** Returns a copy of the configured semantic object tags for safe C++ and Blueprint inspection. */
	UFUNCTION(BlueprintPure, Category = "Reality|Editable")
	FGameplayTagContainer GetObjectTags() const;

	/** Returns a concise description of the owner and its configured tags for development inspection. */
	UFUNCTION(BlueprintPure, Category = "Reality|Editable|Debug")
	FString GetEditableDebugDescription() const;

	/** Writes the current editable configuration to LogReality on explicit request. */
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Reality|Editable|Debug")
	void LogEditableConfiguration() const;

	/** Copies capability configuration without exposing mutable internal storage. Intended for C++ setup and tests. */
	void SetSupportedCheats(const FGameplayTagContainer& InSupportedCheats);

	/** Copies object classification without exposing mutable internal storage. Intended for C++ setup and tests. */
	void SetObjectTags(const FGameplayTagContainer& InObjectTags);

private:
	/** Cheat properties this actor supports. Configure these on the component's Blueprint or instance defaults. */
	UPROPERTY(EditAnywhere, Category = "Reality|Editable", meta = (Categories = "Cheat"))
	FGameplayTagContainer SupportedCheats;

	/** Semantic object classifications used by future Reality rules. */
	UPROPERTY(EditAnywhere, Category = "Reality|Editable", meta = (Categories = "Object"))
	FGameplayTagContainer ObjectTags;
};
