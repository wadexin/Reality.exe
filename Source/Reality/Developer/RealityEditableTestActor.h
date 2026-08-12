// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RealitySystem/RealityCheatEvent.h"
#include "RealityEditableTestActor.generated.h"

class UBoxComponent;
class URealityEditableComponent;

/** Minimal native fixture used to validate multi-component Reality collision behavior and event emission. */
UCLASS(NotBlueprintable)
class REALITY_API ARealityEditableTestActor : public AActor
{
	GENERATED_BODY()

public:
	ARealityEditableTestActor();

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> PrimitiveA;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> PrimitiveB;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<URealityEditableComponent> EditableComponent;

	UPROPERTY(Transient)
	int32 CheatEventCount = 0;

	UPROPERTY(Transient)
	FRealityCheatEvent LastCheatEvent;

	UPROPERTY(Transient)
	int32 DeveloperFocusGainedCount = 0;

	UPROPERTY(Transient)
	int32 DeveloperFocusLostCount = 0;

	/** Records emitted events for focused automation validation. */
	UFUNCTION()
	void HandleRealityCheatEvent(const FRealityCheatEvent& CheatEvent);

	UFUNCTION()
	void HandleDeveloperFocusGained(AActor* FocusedActor);

	UFUNCTION()
	void HandleDeveloperFocusLost(AActor* LostActor);
};
