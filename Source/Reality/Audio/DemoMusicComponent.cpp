// Copyright Epic Games, Inc. All Rights Reserved.
#include "Audio/DemoMusicComponent.h"

#include "RealityPlayerController.h"
#include "Sound/SoundBase.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"

UDemoMusicComponent::UDemoMusicComponent()
{
	bAutoActivate = false;
	bAllowSpatialization = false;
	bOverrideAttenuation = false;
	bIsUISound = true;
	bStopWhenOwnerDestroyed = true;

	static ConstructorHelpers::FObjectFinder<USoundBase> Music(
		TEXT("/Game/Audio/Reality/Music/S_RLT_BehindTheParameters.S_RLT_BehindTheParameters"));
	if (Music.Succeeded()) SetSound(Music.Object);
}

void UDemoMusicComponent::BeginPlay()
{
	Super::BeginPlay();
	const ARealityPlayerController* Controller = Cast<ARealityPlayerController>(GetOwner());
	if (!Controller || !Controller->IsLocalPlayerController() || !Controller->IsDemoWorld()) return;
	OnAudioFinished.AddUniqueDynamic(this, &UDemoMusicComponent::HandleMusicFinished);
	PlayMusic();
}

void UDemoMusicComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld()) World->GetTimerManager().ClearTimer(ReplayTimer);
	OnAudioFinished.RemoveDynamic(this, &UDemoMusicComponent::HandleMusicFinished);
	Stop();
	Super::EndPlay(EndPlayReason);
}

void UDemoMusicComponent::HandleMusicFinished()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(ReplayTimer, this, &UDemoMusicComponent::PlayMusic, ReplayDelaySeconds, false);
	}
}

void UDemoMusicComponent::PlayMusic()
{
	if (Sound) FadeIn(FadeInSeconds, MusicVolume);
}
