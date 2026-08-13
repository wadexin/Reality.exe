// Copyright Epic Games, Inc. All Rights Reserved.
#include "Puzzle/Demo/DemoCheckpointVolume.h"

#include "Components/BoxComponent.h"
#include "Engine/CollisionProfile.h"
#include "Puzzle/Demo/DemoPlayerRecoveryComponent.h"

ADemoCheckpointVolume::ADemoCheckpointVolume()
{
	PrimaryActorTick.bCanEverTick = false;
	CheckpointBounds = CreateDefaultSubobject<UBoxComponent>(TEXT("Checkpoint Bounds"));
	SetRootComponent(CheckpointBounds);
	CheckpointBounds->SetCollisionProfileName(TEXT("Trigger"));
	CheckpointBounds->SetGenerateOverlapEvents(true);
	CheckpointBounds->OnComponentBeginOverlap.AddUniqueDynamic(this, &ADemoCheckpointVolume::HandleOverlap);
}

bool ADemoCheckpointVolume::ActivateCheckpoint(AActor* PlayerActor)
{
	if (!IsValid(PlayerActor)) return false;
	if (UDemoPlayerRecoveryComponent* Recovery = PlayerActor->FindComponentByClass<UDemoPlayerRecoveryComponent>())
	{
		FTransform SafeTransform = GetActorTransform();
		SafeTransform.SetScale3D(PlayerActor->GetActorScale3D());
		return Recovery->RegisterCheckpoint(CheckpointIndex, SafeTransform);
	}
	return false;
}

void ADemoCheckpointVolume::HandleOverlap(UPrimitiveComponent*, AActor* OtherActor, UPrimitiveComponent*, int32, bool, const FHitResult&)
{
	ActivateCheckpoint(OtherActor);
}
