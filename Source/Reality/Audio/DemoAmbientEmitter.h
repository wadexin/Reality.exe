// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DemoAmbientEmitter.generated.h"

class UAudioComponent;
class USceneComponent;

/** Presentation-only spatial ambience point configured per Demo zone without gameplay dependencies. */
UCLASS(Blueprintable)
class REALITY_API ADemoAmbientEmitter : public AActor
{
	GENERATED_BODY()
public:
	ADemoAmbientEmitter();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TObjectPtr<USceneComponent> SceneRoot;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TObjectPtr<UAudioComponent> AudioComponent;
	UFUNCTION(BlueprintCallable, CallInEditor, Category="Demo|Audio")
	void ConfigureAmbient(FName Profile, float Volume, float InnerRadius, float FalloffDistance);
	UFUNCTION(BlueprintPure, Category="Demo|Audio") FName GetAmbientProfile() const { return AmbientProfile; }
private:
	UPROPERTY(VisibleAnywhere, Category="Demo|Audio") FName AmbientProfile = TEXT("Facility");
};
