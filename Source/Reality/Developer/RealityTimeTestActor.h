// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RealityTimeTestActor.generated.h"

class URealityEditableComponent;
class UStaticMeshComponent;

/** Tick-driven rotor used to demonstrate and validate the supported local-time behavior. */
UCLASS(Blueprintable)
class REALITY_API ARealityTimeTestActor : public AActor
{
	GENERATED_BODY()

public:
	ARealityTimeTestActor();
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reality|Time")
	TObjectPtr<UStaticMeshComponent> RotorMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reality|Time")
	TObjectPtr<URealityEditableComponent> EditableComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reality|Time")
	float RotationDegreesPerSecond = 90.0f;

	/** Delta received through the Actor tick path; exposed for deterministic automation. */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Reality|Time")
	float LastReceivedDeltaSeconds = 0.0f;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Reality|Time")
	float AccumulatedTickSeconds = 0.0f;
};
