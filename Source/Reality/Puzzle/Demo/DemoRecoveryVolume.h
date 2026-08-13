// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DemoRecoveryVolume.generated.h"

class UBoxComponent;

/** Authored invalid-space volume that recovers only the overlapping player or explicitly recoverable prop. */
UCLASS(Blueprintable)
class REALITY_API ADemoRecoveryVolume : public AActor
{
	GENERATED_BODY()
public:
	ADemoRecoveryVolume();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TObjectPtr<UBoxComponent> RecoveryBounds;
	UFUNCTION(BlueprintCallable, Category="Demo|Recovery") bool RecoverActor(AActor* Actor);
private:
	UFUNCTION() void HandleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
