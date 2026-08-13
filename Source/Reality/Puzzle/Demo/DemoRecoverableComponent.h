// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DemoRecoverableComponent.generated.h"

/** Captures one critical Demo prop baseline and performs full local recovery only when explicitly triggered. */
UCLASS(ClassGroup=(Reality), meta=(BlueprintSpawnableComponent))
class REALITY_API UDemoRecoverableComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UDemoRecoverableComponent();
	UFUNCTION(BlueprintCallable, Category="Demo|Recovery") bool RecoverToBaseline();
	UFUNCTION(BlueprintPure, Category="Demo|Recovery") int32 GetRecoveryCount() const { return RecoveryCount; }
	void SetRecoveryEnabled(bool bEnabled) { bRecoveryEnabled = bEnabled; }
	UFUNCTION(BlueprintPure, Category="Demo|Recovery")
	bool IsRecoveryEnabled() const;
protected:
	virtual void BeginPlay() override;
private:
	FTransform BaselineTransform;
	bool bHasBaseline = false;
	bool bRecoveryEnabled = false;
	int32 RecoveryCount = 0;
};
