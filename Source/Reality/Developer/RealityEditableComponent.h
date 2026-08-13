// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/EngineTypes.h"
#include "GameplayTagContainer.h"
#include "RealitySystem/RealityCheatEvent.h"
#include "RealityEditableComponent.generated.h"

class UPrimitiveComponent;
class UPhysicalMaterial;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRealityCheatEventSignature, const FRealityCheatEvent&, CheatEvent);

/** Controlled multipliers available to the prototype Scale Reality modification. */
UENUM(BlueprintType)
enum class ERealityScalePreset : uint8
{
	Quarter UMETA(DisplayName = "0.25x"),
	Half UMETA(DisplayName = "0.5x"),
	One UMETA(DisplayName = "1.0x"),
	Double UMETA(DisplayName = "2.0x"),
	Quadruple UMETA(DisplayName = "4.0x")
};

/** Local gravity states supported by the prototype rigid-body Gravity modification. */
UENUM(BlueprintType)
enum class ERealityGravityPreset : uint8
{
	Normal,
	Low,
	Zero
};

/** Controlled baseline-relative multipliers available to the Mass Reality modification. */
UENUM(BlueprintType)
enum class ERealityMassPreset : uint8
{
	Quarter UMETA(DisplayName = "0.25x"),
	Half UMETA(DisplayName = "0.5x"),
	One UMETA(DisplayName = "1.0x"),
	Double UMETA(DisplayName = "2.0x"),
	Quadruple UMETA(DisplayName = "4.0x")
};

/** Controlled contact-friction states available to the Friction Reality modification. */
UENUM(BlueprintType)
enum class ERealityFrictionPreset : uint8
{
	Zero,
	Low,
	Normal,
	High
};

/** Controlled baseline-relative multipliers for Actor-local gameplay Tick time. */
UENUM(BlueprintType)
enum class ERealityTimePreset : uint8
{
	Quarter UMETA(DisplayName = "0.25x"),
	Half UMETA(DisplayName = "0.5x"),
	One UMETA(DisplayName = "1.0x"),
	Double UMETA(DisplayName = "2.0x"),
	Quadruple UMETA(DisplayName = "4.0x")
};

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

/** Exact gravity-enabled state captured for one eligible simulated primitive. */
USTRUCT()
struct FRealityOriginalGravityState
{
	GENERATED_BODY()

	FRealityOriginalGravityState() = default;

	FRealityOriginalGravityState(UPrimitiveComponent* InPrimitiveComponent, const bool bInGravityEnabled)
		: PrimitiveComponent(InPrimitiveComponent)
		, bGravityEnabled(bInGravityEnabled)
	{
	}

	UPROPERTY()
	TWeakObjectPtr<UPrimitiveComponent> PrimitiveComponent;

	bool bGravityEnabled = false;
};

/** Exact authored mass configuration and effective baseline captured for one simulated primitive. */
USTRUCT()
struct FRealityOriginalMassState
{
	GENERATED_BODY()

	UPROPERTY()
	TWeakObjectPtr<UPrimitiveComponent> PrimitiveComponent;

	float BaselineEffectiveMassKg = 0.0f;
	float OriginalMassScale = 1.0f;
	float OriginalMassOverrideKg = 100.0f;
	bool bOriginallyOverrodeMass = false;
};

/** Original component material path and per-cycle effective friction baseline. */
USTRUCT()
struct FRealityOriginalFrictionState
{
	GENERATED_BODY()

	UPROPERTY()
	TWeakObjectPtr<UPrimitiveComponent> PrimitiveComponent;

	/** Exact component override reference; null means the component inherited its legitimate material. */
	UPROPERTY()
	TObjectPtr<UPhysicalMaterial> OriginalPhysMaterialOverride = nullptr;

	/** Effective source material captured at cycle start and duplicated for private runtime presets. */
	UPROPERTY()
	TObjectPtr<UPhysicalMaterial> BaselinePhysMaterial = nullptr;

	/** Unique transient material currently assigned only to this component for a non-Normal preset. */
	UPROPERTY()
	TObjectPtr<UPhysicalMaterial> RuntimePhysMaterial = nullptr;

	float BaselineDynamicFriction = 0.0f;
	float BaselineStaticFriction = 0.0f;
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

	/** Applies a controlled multiplier to the exact Actor scale captured at the start of this Scale cycle. */
	UFUNCTION(BlueprintCallable, Category = "Reality|Editable|Scale")
	bool ApplyScaleModification(ERealityScalePreset Preset, AActor* InstigatingActor);

	/** Restores the exact Actor scale captured before the active Scale cycle. */
	UFUNCTION(BlueprintCallable, Category = "Reality|Editable|Scale")
	bool RestoreScaleModification(AActor* InstigatingActor);

	/** Returns whether an explicit Scale modification cycle is active, including an active 1.0x preset. */
	UFUNCTION(BlueprintPure, Category = "Reality|Editable|Scale")
	bool IsScaleModified() const { return bScaleModified; }

	/** Returns the active Scale preset. The value is 1.0x when no Scale cycle is active. */
	UFUNCTION(BlueprintPure, Category = "Reality|Editable|Scale")
	ERealityScalePreset GetCurrentScalePreset() const { return CurrentScalePreset; }

	/** Returns the captured baseline while Scale is active, otherwise the owner's current legitimate scale. */
	UFUNCTION(BlueprintPure, Category = "Reality|Editable|Scale")
	FVector GetOriginalScale() const;

	/** Applies Normal, Low, or Zero gravity to directly owned simulated primitives. */
	UFUNCTION(BlueprintCallable, Category = "Reality|Editable|Gravity")
	bool ApplyGravityModification(ERealityGravityPreset Preset, AActor* InstigatingActor);

	/** Restores every surviving gravity target to its exact captured gravity-enabled state. */
	UFUNCTION(BlueprintCallable, Category = "Reality|Editable|Gravity")
	bool RestoreGravityModification(AActor* InstigatingActor);

	/** Returns whether an explicit Gravity modification cycle is active, including active Normal. */
	UFUNCTION(BlueprintPure, Category = "Reality|Editable|Gravity")
	bool IsGravityModified() const { return bGravityModified; }

	/** Returns the active Gravity preset. The value is Normal while no cycle is active. */
	UFUNCTION(BlueprintPure, Category = "Reality|Editable|Gravity")
	ERealityGravityPreset GetCurrentGravityPreset() const { return CurrentGravityPreset; }

	/** Counts directly owned PrimitiveComponents that currently simulate physics. */
	UFUNCTION(BlueprintPure, Category = "Reality|Editable|Gravity")
	int32 GetEligibleGravityComponentCount() const;

	/** Applies a controlled multiplier to each simulated primitive's captured effective mass. */
	UFUNCTION(BlueprintCallable, Category = "Reality|Editable|Mass")
	bool ApplyMassModification(ERealityMassPreset Preset, AActor* InstigatingActor);

	/** Restores each surviving primitive's exact authored MassScale and mass-override configuration. */
	UFUNCTION(BlueprintCallable, Category = "Reality|Editable|Mass")
	bool RestoreMassModification(AActor* InstigatingActor);

	/** Returns whether an explicit Mass cycle is active, including active 1.0x. */
	UFUNCTION(BlueprintPure, Category = "Reality|Editable|Mass")
	bool IsMassModified() const { return bMassModified; }

	/** Returns the active Mass preset, or 1.0x while unmodified. */
	UFUNCTION(BlueprintPure, Category = "Reality|Editable|Mass")
	ERealityMassPreset GetCurrentMassPreset() const { return CurrentMassPreset; }

	/** Counts directly owned PrimitiveComponents that currently simulate physics. */
	UFUNCTION(BlueprintPure, Category = "Reality|Editable|Mass")
	int32 GetEligibleMassComponentCount() const;

	/** Returns the sum of captured effective masses for surviving bodies, or current eligible mass while inactive. */
	UFUNCTION(BlueprintPure, Category = "Reality|Editable|Mass")
	float GetBaselineEffectiveMassKg() const;

	/** Returns the aggregate current effective mass of directly owned eligible/captured bodies. */
	UFUNCTION(BlueprintPure, Category = "Reality|Editable|Mass")
	float GetCurrentEffectiveMassKg() const;

	/** Applies a controlled contact-friction preset to directly owned simulated primitives. */
	UFUNCTION(BlueprintCallable, Category = "Reality|Editable|Friction")
	bool ApplyFrictionModification(ERealityFrictionPreset Preset, AActor* InstigatingActor);

	/** Restores each surviving primitive's exact original Physical Material override reference. */
	UFUNCTION(BlueprintCallable, Category = "Reality|Editable|Friction")
	bool RestoreFrictionModification(AActor* InstigatingActor);

	/** Returns whether an explicit Friction cycle is active, including active Normal. */
	UFUNCTION(BlueprintPure, Category = "Reality|Editable|Friction")
	bool IsFrictionModified() const { return bFrictionModified; }

	/** Returns the active Friction preset, or Normal while unmodified. */
	UFUNCTION(BlueprintPure, Category = "Reality|Editable|Friction")
	ERealityFrictionPreset GetCurrentFrictionPreset() const { return CurrentFrictionPreset; }

	/** Counts directly owned primitives eligible for Friction under current Collision state. */
	UFUNCTION(BlueprintPure, Category = "Reality|Editable|Friction")
	int32 GetEligibleFrictionComponentCount() const;

	/** Returns the average captured dynamic-friction baseline, or current eligible average while inactive. */
	UFUNCTION(BlueprintPure, Category = "Reality|Editable|Friction")
	float GetBaselineFriction() const;

	/** Returns the deterministic average dynamic friction represented by the active preset/current state. */
	UFUNCTION(BlueprintPure, Category = "Reality|Editable|Friction")
	float GetCurrentFriction() const;

	/** Applies a controlled multiplier to the owner's captured CustomTimeDilation. */
	UFUNCTION(BlueprintCallable, Category = "Reality|Editable|Time")
	bool ApplyTimeModification(ERealityTimePreset Preset, AActor* InstigatingActor);

	/** Restores the exact legitimate CustomTimeDilation captured for the active cycle. */
	UFUNCTION(BlueprintCallable, Category = "Reality|Editable|Time")
	bool RestoreTimeModification(AActor* InstigatingActor);

	/** Returns whether an explicit Time cycle is active, including active 1.0x. */
	UFUNCTION(BlueprintPure, Category = "Reality|Editable|Time")
	bool IsTimeModified() const { return bTimeModified; }

	/** Returns the active Time preset, or 1.0x while unmodified. */
	UFUNCTION(BlueprintPure, Category = "Reality|Editable|Time")
	ERealityTimePreset GetCurrentTimePreset() const { return CurrentTimePreset; }

	/** Returns the captured baseline while active, otherwise the owner's current local dilation. */
	UFUNCTION(BlueprintPure, Category = "Reality|Editable|Time")
	float GetOriginalTimeDilation() const;

	/** Returns the owner's current CustomTimeDilation without global world dilation. */
	UFUNCTION(BlueprintPure, Category = "Reality|Editable|Time")
	float GetCurrentEffectiveTimeDilation() const;

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

protected:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	/** Broadcasts one successful property event and submits that same event to the world Reality Manager. */
	void EmitRealityCheatEvent(const FRealityCheatEvent& CheatEvent);

	/** Reasserts active temporary mass overrides after Chaos geometry mass recalculation. Emits no event. */
	void ReapplyActiveMassPreset();

	/** Reassigns active component-local runtime materials after body recreation without emitting an event. */
	void ReapplyActiveFrictionPreset();

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

	/** Exact legitimate Actor scale captured once for the active Scale cycle. */
	UPROPERTY(Transient)
	FVector OriginalScale = FVector::OneVector;

	UPROPERTY(Transient)
	ERealityScalePreset CurrentScalePreset = ERealityScalePreset::One;

	UPROPERTY(Transient)
	bool bScaleModified = false;

	/** Per-component legitimate gravity state captured once for the active Gravity cycle. */
	UPROPERTY(Transient)
	TArray<FRealityOriginalGravityState> OriginalGravityStates;

	UPROPERTY(Transient)
	ERealityGravityPreset CurrentGravityPreset = ERealityGravityPreset::Normal;

	UPROPERTY(Transient)
	bool bGravityModified = false;

	/** Per-component authored mass configuration and effective baseline captured once per Mass cycle. */
	UPROPERTY(Transient)
	TArray<FRealityOriginalMassState> OriginalMassStates;

	UPROPERTY(Transient)
	ERealityMassPreset CurrentMassPreset = ERealityMassPreset::One;

	UPROPERTY(Transient)
	bool bMassModified = false;

	/** Per-component original material paths and unique runtime materials for one Friction cycle. */
	UPROPERTY(Transient)
	TArray<FRealityOriginalFrictionState> OriginalFrictionStates;

	UPROPERTY(Transient)
	ERealityFrictionPreset CurrentFrictionPreset = ERealityFrictionPreset::Normal;

	UPROPERTY(Transient)
	bool bFrictionModified = false;

	/** Exact legitimate Actor-local dilation captured once per Time cycle. */
	UPROPERTY(Transient)
	float OriginalTimeDilation = 1.0f;

	UPROPERTY(Transient)
	ERealityTimePreset CurrentTimePreset = ERealityTimePreset::One;

	UPROPERTY(Transient)
	bool bTimeModified = false;
};
