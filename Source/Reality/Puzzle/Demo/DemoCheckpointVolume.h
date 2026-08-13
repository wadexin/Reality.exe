// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DemoCheckpointVolume.generated.h"

class UBoxComponent;

/** Forward-only authored Demo progression anchor that updates the player's in-memory safe transform. */
UCLASS(Blueprintable)
class REALITY_API ADemoCheckpointVolume : public AActor
{
	GENERATED_BODY()
public:
	ADemoCheckpointVolume();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TObjectPtr<UBoxComponent> CheckpointBounds;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Demo|Checkpoint", meta=(ClampMin="1")) int32 CheckpointIndex = 1;
	UFUNCTION(BlueprintCallable, Category="Demo|Checkpoint") bool ActivateCheckpoint(AActor* PlayerActor);
private:
	UFUNCTION() void HandleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
