// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RealityContextTestActor.generated.h"

class USceneComponent;
class URealityContextComponent;

/** Minimal native fixture for Context registration, activation, and spatial tests. */
UCLASS(NotBlueprintable)
class REALITY_API ARealityContextTestActor : public AActor
{
	GENERATED_BODY()

public:
	ARealityContextTestActor();

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<URealityContextComponent> ContextComponent;
};
