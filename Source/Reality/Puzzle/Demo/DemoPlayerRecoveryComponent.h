// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DemoPlayerRecoveryComponent.generated.h"

/** Session-only safe-position history for the local Demo player. It never reconstructs world or Reality state. */
UCLASS(ClassGroup=(Reality), meta=(BlueprintSpawnableComponent))
class REALITY_API UDemoPlayerRecoveryComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UDemoPlayerRecoveryComponent();

	/** Accepts an authored forward-only checkpoint and records its safe player transform. */
	UFUNCTION(BlueprintCallable, Category="Demo|Recovery")
	bool RegisterCheckpoint(int32 CheckpointIndex, const FTransform& SafeTransform);

	/** Teleports the owner to the latest safe transform without changing Suspicion or world state. */
	UFUNCTION(BlueprintCallable, Category="Demo|Recovery")
	bool RecoverPlayer();

	UFUNCTION(BlueprintPure, Category="Demo|Recovery") int32 GetCheckpointIndex() const { return CheckpointIndex; }
	UFUNCTION(BlueprintPure, Category="Demo|Recovery") FTransform GetRecoveryTransform() const { return RecoveryTransform; }
	UFUNCTION(BlueprintPure, Category="Demo|Recovery") int32 GetRecoveryCount() const { return RecoveryCount; }

protected:
	virtual void BeginPlay() override;

private:
	FTransform RecoveryTransform;
	int32 CheckpointIndex = 0;
	int32 RecoveryCount = 0;
	bool bHasRecoveryTransform = false;
};
