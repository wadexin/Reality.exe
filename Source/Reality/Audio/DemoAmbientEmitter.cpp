// Copyright Epic Games, Inc. All Rights Reserved.
#include "Audio/DemoAmbientEmitter.h"
#include "Components/AudioComponent.h"
#include "Components/SceneComponent.h"
#include "Sound/SoundBase.h"

ADemoAmbientEmitter::ADemoAmbientEmitter()
{
	PrimaryActorTick.bCanEverTick = false;
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Root"));
	SetRootComponent(SceneRoot);
	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("Ambient Audio"));
	AudioComponent->SetupAttachment(SceneRoot);
	AudioComponent->bAutoActivate = true;
	AudioComponent->bAllowSpatialization = true;
	AudioComponent->bStopWhenOwnerDestroyed = true;
	AudioComponent->bOverrideAttenuation = true;
}

void ADemoAmbientEmitter::ConfigureAmbient(const FName Profile, const float Volume, const float InnerRadius, const float FalloffDistance)
{
	AmbientProfile = Profile;
	const TCHAR* Path = Profile == TEXT("Industrial")
		? TEXT("/Game/Audio/Reality/Ambience/S_RLT_IndustrialLoop.S_RLT_IndustrialLoop")
		: Profile == TEXT("Utility")
			? TEXT("/Game/Audio/Reality/Ambience/S_RLT_UtilityAmbienceLoop.S_RLT_UtilityAmbienceLoop")
			: TEXT("/Game/Audio/Reality/Ambience/S_RLT_FacilityLoop.S_RLT_FacilityLoop");
	AudioComponent->SetSound(LoadObject<USoundBase>(nullptr, Path));
	AudioComponent->SetVolumeMultiplier(FMath::Clamp(Volume, 0.0f, 1.0f));
	AudioComponent->AttenuationOverrides.bAttenuate = true;
	AudioComponent->AttenuationOverrides.bSpatialize = true;
	AudioComponent->AttenuationOverrides.AttenuationShapeExtents = FVector(FMath::Max(0.0f, InnerRadius));
	AudioComponent->AttenuationOverrides.FalloffDistance = FMath::Max(0.0f, FalloffDistance);
	if (HasActorBegunPlay() && !AudioComponent->IsPlaying()) AudioComponent->Play();
}
