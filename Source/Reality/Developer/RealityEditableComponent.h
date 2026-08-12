// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/EngineTypes.h"
#include "GameplayTagContainer.h"
#include "RealitySystem/RealityCheatEvent.h"
#include "RealityEditableComponent.generated.h"

class UPrimitiveComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRealityCheatEventSignature, const FRealityCheatEvent&, CheatEvent);

/** Exact collision state captured for one owner-local primitive during an active modification cycle. */
USTRUCT()
struct FRealityOriginalCollisionState
{
	GENERATED_BODY()

	FRealityOriginalCollisionState() = default;

	FRealityOriginalCollisionState(UPrimitiveComponent* InPrimitiveComponent, const ECollisionEnabled::Type InCollisionEnabled)
		: PrimitiveComponent(InPrimitiveComponent)
		, CollisionEnabled(InCollisionEnabled)
	{
	}

	UPROPERTY()
	TWeakObjectPtr<UPrimitiveComponent> PrimitiveComponent;

	ECollisionEnabled::Type CollisionEnabled = ECollisionEnabled::NoCollision;
};

/**
 * Marks its owning actor as Reality-editable and declares its supported cheat capabilities and object tags.
 * Typed property implementations live here incrementally while keeping their state and operations independent.
 */
UCLASS(ClassGroup = (Reality), BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class REALITY_API URealityEditableComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	URealityEditableComponent();

	/** Broadcast after a successful Reality property Apply or Restore operation. */
	UPROPERTY(BlueprintAssignable, Category = "Reality|Editable|Events")
	FRealityCheatEventSignature OnRealityCheatEvent;

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

	/** Disables collision on valid PrimitiveComponents owned directly by this Actor. */
	UFUNCTION(BlueprintCallable, Category = "Reality|Editable|Collision")
	bool ApplyCollisionModification(AActor* InstigatingActor);

	/** Restores each surviving PrimitiveComponent to the exact state captured by the active collision cycle. */
	UFUNCTION(BlueprintCallable, Category = "Reality|Editable|Collision")
	bool RestoreCollisionModification(AActor* InstigatingActor);

	/** Returns whether this component currently owns an active collision modification cycle. */
	UFUNCTION(BlueprintPure, Category = "Reality|Editable|Collision")
	bool IsCollisionModified() const { return bCollisionModified; }

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

	/** Per-component states captured before the active collision modification. Never exposed for mutation. */
	UPROPERTY(Transient)
	TArray<FRealityOriginalCollisionState> OriginalCollisionStates;

	UPROPERTY(Transient)
	bool bCollisionModified = false;
};
