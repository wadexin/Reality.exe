// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "P0PrototypeExit.generated.h"

class UBoxComponent;
class UTextRenderComponent;

/** One-shot P0 completion trigger that reports final Reality pressure without changing levels or saves. */
UCLASS(Blueprintable)
class REALITY_API AP0PrototypeExit : public AActor
{
	GENERATED_BODY()

public:
	AP0PrototypeExit();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> Trigger;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UTextRenderComponent> ExitLabel;

	UFUNCTION(BlueprintPure, Category = "Prototype|Exit")
	bool HasCompletedPrototype() const { return bPrototypeCompleted; }

protected:
	UFUNCTION()
	void HandleTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	UPROPERTY(Transient)
	bool bPrototypeCompleted = false;
};
