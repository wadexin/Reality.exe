// Copyright Epic Games, Inc. All Rights Reserved.
#include "Audio/DeveloperAudioFeedbackComponent.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"
#include "UObject/ConstructorHelpers.h"

UDeveloperAudioFeedbackComponent::UDeveloperAudioFeedbackComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	static ConstructorHelpers::FObjectFinder<USoundBase> Entry(TEXT("/Game/Audio/Reality/UI/S_RLT_DeveloperOpen.S_RLT_DeveloperOpen"));
	static ConstructorHelpers::FObjectFinder<USoundBase> Exit(TEXT("/Game/Audio/Reality/UI/S_RLT_DeveloperClose.S_RLT_DeveloperClose"));
	static ConstructorHelpers::FObjectFinder<USoundBase> Apply(TEXT("/Game/Audio/Reality/UI/S_RLT_RealityApply.S_RLT_RealityApply"));
	static ConstructorHelpers::FObjectFinder<USoundBase> Restore(TEXT("/Game/Audio/Reality/UI/S_RLT_RealityRestore.S_RLT_RealityRestore"));
	EntrySound = Entry.Object; ExitSound = Exit.Object; ApplySound = Apply.Object; RestoreSound = Restore.Object;
}

void UDeveloperAudioFeedbackComponent::BeginPlay()
{
	Super::BeginPlay();
	AudioComponent = NewObject<UAudioComponent>(GetOwner(), TEXT("Developer Feedback Audio"));
	AudioComponent->bAutoActivate = false;
	AudioComponent->bIsUISound = true;
	AudioComponent->bAllowSpatialization = false;
	AudioComponent->bStopWhenOwnerDestroyed = true;
	AudioComponent->RegisterComponent();
	DeveloperMode = GetOwner()->FindComponentByClass<UDeveloperModeComponent>();
	if (UDeveloperModeComponent* Mode = DeveloperMode.Get())
	{
		Mode->OnDeveloperModeChanged.AddUniqueDynamic(this, &UDeveloperAudioFeedbackComponent::HandleDeveloperModeChanged);
		Mode->OnDeveloperOperationFeedback.AddUniqueDynamic(this, &UDeveloperAudioFeedbackComponent::HandleOperationFeedback);
	}
}

void UDeveloperAudioFeedbackComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UDeveloperModeComponent* Mode = DeveloperMode.Get())
	{
		Mode->OnDeveloperModeChanged.RemoveDynamic(this, &UDeveloperAudioFeedbackComponent::HandleDeveloperModeChanged);
		Mode->OnDeveloperOperationFeedback.RemoveDynamic(this, &UDeveloperAudioFeedbackComponent::HandleOperationFeedback);
	}
	if (AudioComponent)
	{
		AudioComponent->Stop();
		AudioComponent->DestroyComponent();
		AudioComponent = nullptr;
	}
	Super::EndPlay(EndPlayReason);
}

void UDeveloperAudioFeedbackComponent::HandleDeveloperModeChanged(const bool bActive)
{
	PlayCue(bActive ? EntrySound : ExitSound);
}

void UDeveloperAudioFeedbackComponent::HandleOperationFeedback(const EDeveloperOperationFeedback Feedback, const FGameplayTag CheatTag)
{
	if (Feedback == EDeveloperOperationFeedback::None) return;
	++SuccessfulFeedbackCount;
	LastFeedback = Feedback;
	float Pitch = 1.0f;
	if (CheatTag.MatchesTagExact(FGameplayTag::RequestGameplayTag(TEXT("Cheat.Scale")))) Pitch = 1.06f;
	else if (CheatTag.MatchesTagExact(FGameplayTag::RequestGameplayTag(TEXT("Cheat.Gravity")))) Pitch = 0.94f;
	else if (CheatTag.MatchesTagExact(FGameplayTag::RequestGameplayTag(TEXT("Cheat.Mass")))) Pitch = 0.88f;
	else if (CheatTag.MatchesTagExact(FGameplayTag::RequestGameplayTag(TEXT("Cheat.Friction")))) Pitch = 0.97f;
	else if (CheatTag.MatchesTagExact(FGameplayTag::RequestGameplayTag(TEXT("Cheat.Time")))) Pitch = 1.12f;
	PlayCue(Feedback == EDeveloperOperationFeedback::Applied ? ApplySound : RestoreSound, Pitch);
}

void UDeveloperAudioFeedbackComponent::PlayCue(USoundBase* Sound, const float Pitch)
{
	if (!AudioComponent || !Sound) return;
	AudioComponent->Stop();
	AudioComponent->SetSound(Sound);
	AudioComponent->SetPitchMultiplier(Pitch);
	AudioComponent->SetVolumeMultiplier(0.72f);
	AudioComponent->Play();
}
