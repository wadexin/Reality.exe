// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DemoChinesePresentationActor.generated.h"

/** Applies Chinese copy and a CJK-capable font to the dedicated Chinese Demo map at startup. */
UCLASS(NotBlueprintable)
class REALITY_API ADemoChinesePresentationActor : public AActor
{
	GENERATED_BODY()

public:
	ADemoChinesePresentationActor();

protected:
	virtual void BeginPlay() override;

private:
	void ApplyPresentation();
};
