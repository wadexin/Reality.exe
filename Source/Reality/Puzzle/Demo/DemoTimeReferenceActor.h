// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DemoTimeReferenceActor.generated.h"

class USceneComponent;
class UStaticMeshComponent;
class UTextRenderComponent;
class UAudioComponent;

/** Non-editable rotor that provides a persistent baseline cadence beside the local-Time target. */
UCLASS(Blueprintable)
class REALITY_API ADemoTimeReferenceActor : public AActor
{
	GENERATED_BODY()

public:
	ADemoTimeReferenceActor();
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> RotorMesh;

	/** Collision-free baseline cadence marker carried by the authoritative rotating component. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> RotorTipPositive;

	/** Mirrored collision-free baseline marker carried by the authoritative rotating component. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> RotorTipNegative;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UTextRenderComponent> ReferenceLabel;

	/** Stable spatial cadence baseline; never bound to the editable rotor's events. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UAudioComponent> MachineryAudio;

	UFUNCTION(BlueprintPure, Category = "Demo|Time|Audio")
	float GetPresentationAudioPitch() const { return 1.0f; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Demo|Time")
	float RotationDegreesPerSecond = 90.0f;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Demo|Time")
	float AccumulatedTickSeconds = 0.0f;
};
