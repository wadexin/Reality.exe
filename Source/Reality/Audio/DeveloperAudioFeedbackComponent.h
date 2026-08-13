// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Developer/DeveloperModeComponent.h"
#include "DeveloperAudioFeedbackComponent.generated.h"

class UAudioComponent;
class UDeveloperModeComponent;
class USoundBase;

/** Event-driven, non-spatial UI audio for Developer Mode and successful Reality operations. */
UCLASS(ClassGroup=(Reality), meta=(BlueprintSpawnableComponent))
class REALITY_API UDeveloperAudioFeedbackComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UDeveloperAudioFeedbackComponent();
	int32 GetSuccessfulFeedbackCount() const { return SuccessfulFeedbackCount; }
	EDeveloperOperationFeedback GetLastFeedback() const { return LastFeedback; }
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
private:
	UFUNCTION() void HandleDeveloperModeChanged(bool bActive);
	UFUNCTION() void HandleOperationFeedback(EDeveloperOperationFeedback Feedback, FGameplayTag CheatTag);
	void PlayCue(USoundBase* Sound, float Pitch = 1.0f);
	UPROPERTY(Transient) TObjectPtr<UAudioComponent> AudioComponent;
	UPROPERTY() TObjectPtr<USoundBase> EntrySound;
	UPROPERTY() TObjectPtr<USoundBase> ExitSound;
	UPROPERTY() TObjectPtr<USoundBase> ApplySound;
	UPROPERTY() TObjectPtr<USoundBase> RestoreSound;
	UPROPERTY(Transient) TWeakObjectPtr<UDeveloperModeComponent> DeveloperMode;
	int32 SuccessfulFeedbackCount = 0;
	EDeveloperOperationFeedback LastFeedback = EDeveloperOperationFeedback::None;
};
