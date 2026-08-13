// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RealitySystem/RealityCheatEvent.h"
#include "DemoTimeMachineryActor.generated.h"

class URealityEditableComponent;
class USceneComponent;
class UStaticMeshComponent;
class UTextRenderComponent;
class UAudioComponent;

/** Lightweight local-Tick rotor used as both a baseline reference and a Reality-editable final gate mechanism. */
UCLASS(Blueprintable)
class REALITY_API ADemoTimeMachineryActor : public AActor
{
	GENERATED_BODY()

public:
	ADemoTimeMachineryActor();
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> RotorMesh;

	/** Collision-free visual cadence marker carried by the authoritative rotating component. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> RotorTipPositive;

	/** Mirrored collision-free cadence marker carried by the authoritative rotating component. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> RotorTipNegative;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UTextRenderComponent> MachineryLabel;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<URealityEditableComponent> EditableComponent;

	/** Spatial motor bed whose pitch explicitly mirrors the authoritative local-Time preset. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UAudioComponent> MachineryAudio;

	UFUNCTION(BlueprintPure, Category = "Demo|Time|Audio")
	float GetPresentationAudioPitch() const;

	UFUNCTION(BlueprintPure, Category = "Demo|Time|Audio")
	int32 GetAudioStateUpdateCount() const { return AudioStateUpdateCount; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Demo|Time")
	float RotationDegreesPerSecond = 90.0f;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Demo|Time")
	float AccumulatedTickSeconds = 0.0f;

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Demo|Time")
	void ConfigureTimeMachinery(FText DisplayName, float InRotationDegreesPerSecond);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UFUNCTION() void HandleRealityCheatEvent(const FRealityCheatEvent& CheatEvent);
	void RefreshTimeAudio();
	int32 AudioStateUpdateCount = 0;
};
