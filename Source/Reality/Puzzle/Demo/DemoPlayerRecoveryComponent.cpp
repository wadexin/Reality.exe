// Copyright Epic Games, Inc. All Rights Reserved.
#include "Puzzle/Demo/DemoPlayerRecoveryComponent.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/Engine.h"
#include "Reality.h"

UDemoPlayerRecoveryComponent::UDemoPlayerRecoveryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UDemoPlayerRecoveryComponent::BeginPlay()
{
	Super::BeginPlay();
	if (AActor* Owner = GetOwner())
	{
		RecoveryTransform = Owner->GetActorTransform();
		bHasRecoveryTransform = true;
	}
}

bool UDemoPlayerRecoveryComponent::RegisterCheckpoint(const int32 InCheckpointIndex, const FTransform& SafeTransform)
{
	if (InCheckpointIndex <= CheckpointIndex || !SafeTransform.IsValid()) return false;
	CheckpointIndex = InCheckpointIndex;
	RecoveryTransform = SafeTransform;
	bHasRecoveryTransform = true;
	UE_LOG(LogReality, Log, TEXT("Demo checkpoint %d registered for '%s'."), CheckpointIndex, *GetNameSafe(GetOwner()));
#if !UE_BUILD_SHIPPING
	if (GEngine) GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.5f, FColor::Cyan, TEXT("RECOVERY POINT UPDATED"));
#endif
	return true;
}

bool UDemoPlayerRecoveryComponent::RecoverPlayer()
{
	AActor* Owner = GetOwner();
	if (!IsValid(Owner) || !bHasRecoveryTransform) return false;
	if (ACharacter* Character = Cast<ACharacter>(Owner))
	{
		if (UCharacterMovementComponent* Movement = Character->GetCharacterMovement()) Movement->StopMovementImmediately();
	}
	Owner->SetActorTransform(RecoveryTransform, false, nullptr, ETeleportType::TeleportPhysics);
	++RecoveryCount;
#if !UE_BUILD_SHIPPING
	if (GEngine) GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0f, FColor::Cyan, TEXT("POSITION RESTORED"));
#endif
	return true;
}
