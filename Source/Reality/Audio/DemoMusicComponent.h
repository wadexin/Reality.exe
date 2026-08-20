// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Components/AudioComponent.h"
#include "DemoMusicComponent.generated.h"

/** Local-player, non-spatial Demo music component with delayed full-track replay. */
UCLASS(ClassGroup=(Audio), meta=(BlueprintSpawnableComponent))
class REALITY_API UDemoMusicComponent : public UAudioComponent
{
	GENERATED_BODY()

public:
	UDemoMusicComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Demo|Music", meta=(ClampMin="0.0", ClampMax="1.0"))
	float MusicVolume = 0.30f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Demo|Music", meta=(ClampMin="0.0"))
	float ReplayDelaySeconds = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Demo|Music", meta=(ClampMin="0.0"))
	float FadeInSeconds = 2.0f;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UFUNCTION() void HandleMusicFinished();
	void PlayMusic();
	FTimerHandle ReplayTimer;
};
