// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RealitySystem/RealityCheatEvent.h"
#include "RealityWitnessTestActor.generated.h"

class USceneComponent;
class URealityWitnessComponent;

/** Minimal native fixture for event-driven Witness registration, viewpoint, and notification tests. */
UCLASS(NotBlueprintable)
class REALITY_API ARealityWitnessTestActor : public AActor
{
	GENERATED_BODY()

public:
	ARealityWitnessTestActor();

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<URealityWitnessComponent> WitnessComponent;

	UPROPERTY(Transient)
	int32 ObservedEventCount = 0;

	UPROPERTY(Transient)
	FRealityCheatEvent LastObservedEvent;

	UFUNCTION()
	void HandleRealityEventObserved(const FRealityCheatEvent& CheatEvent);
};
