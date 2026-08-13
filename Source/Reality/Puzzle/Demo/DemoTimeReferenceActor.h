// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DemoTimeReferenceActor.generated.h"

class USceneComponent;
class UStaticMeshComponent;
class UTextRenderComponent;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Demo|Time")
	float RotationDegreesPerSecond = 90.0f;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Demo|Time")
	float AccumulatedTickSeconds = 0.0f;
};
