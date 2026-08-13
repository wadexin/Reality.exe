// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DemoWitnessActor.generated.h"

class URealityWitnessComponent;
class USceneComponent;
class UStaticMeshComponent;
class UTextRenderComponent;

/** Stationary, visibly oriented Human/Camera Witness for authored Demo observation zones. */
UCLASS(Blueprintable)
class REALITY_API ADemoWitnessActor : public AActor
{
	GENERATED_BODY()

public:
	ADemoWitnessActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> DirectionMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UTextRenderComponent> WitnessLabel;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<URealityWitnessComponent> WitnessComponent;

	/** Configures semantic type and the exact event-driven observation volume for this placed Witness. */
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Demo|Witness")
	void ConfigureDemoWitness(bool bCamera, float ObservationRange, float FieldOfViewDegrees, bool bRequireLineOfSight = true);
};
