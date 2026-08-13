// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DeveloperTargetPresentationComponent.generated.h"

class AActor;
class UCameraComponent;
class UMaterialInstanceDynamic;
class UMaterialInterface;
class UPrimitiveComponent;

/** Saved render state for one component temporarily used by Developer Mode highlighting. */
USTRUCT()
struct FDeveloperPrimitiveRenderState
{
	GENERATED_BODY()

	UPROPERTY(Transient)
	TWeakObjectPtr<UPrimitiveComponent> Primitive;

	UPROPERTY(Transient)
	bool bRenderedCustomDepth = false;
};

/**
 * Owns the transient visual presentation for the player's frozen Developer Mode target.
 * It never changes Reality rules: it only marks direct-owned visible primitives and restores their exact prior state.
 */
UCLASS(ClassGroup = (Reality), meta = (BlueprintSpawnableComponent))
class REALITY_API UDeveloperTargetPresentationComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDeveloperTargetPresentationComponent();

	/** Enables or disables the presentation layer without selecting a target. */
	void SetPresentationActive(bool bNewActive);

	/** Replaces the highlighted target, restoring every touched component on the previous target first. */
	void SetHighlightedActor(AActor* NewTarget);

	/** Returns the actor currently represented by the outline. */
	AActor* GetHighlightedActor() const { return HighlightedActor.Get(); }

	/** Number of direct-owned primitives whose prior render state is currently retained. */
	int32 GetSavedPrimitiveCount() const { return SavedPrimitiveStates.Num(); }

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	void ApplyHighlight(AActor* Target);
	void RestoreHighlight();
	void AttachPostProcess();
	void DetachPostProcess();

	UFUNCTION()
	void HandleHighlightedActorDestroyed(AActor* DestroyedActor);

	/** Project-owned post-process material that converts CustomDepth edges into a thin cyan outline. */
	UPROPERTY(EditDefaultsOnly, Category = "Developer Presentation")
	TSoftObjectPtr<UMaterialInterface> OutlineMaterial;

	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> OutlineMaterialInstance;

	UPROPERTY(Transient)
	TWeakObjectPtr<UCameraComponent> PresentationCamera;

	UPROPERTY(Transient)
	TWeakObjectPtr<AActor> HighlightedActor;

	UPROPERTY(Transient)
	TArray<FDeveloperPrimitiveRenderState> SavedPrimitiveStates;

	bool bPresentationActive = false;
};
