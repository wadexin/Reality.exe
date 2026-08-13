// Copyright Epic Games, Inc. All Rights Reserved.
#include "Puzzle/Demo/DemoRecoveryVolume.h"

#include "Components/BoxComponent.h"
#include "Engine/CollisionProfile.h"
#include "Puzzle/Demo/DemoPlayerRecoveryComponent.h"
#include "Puzzle/Demo/DemoRecoverableComponent.h"

ADemoRecoveryVolume::ADemoRecoveryVolume()
{
	PrimaryActorTick.bCanEverTick = false;
	RecoveryBounds = CreateDefaultSubobject<UBoxComponent>(TEXT("Recovery Bounds"));
	SetRootComponent(RecoveryBounds);
	RecoveryBounds->SetCollisionProfileName(TEXT("Trigger"));
	RecoveryBounds->SetGenerateOverlapEvents(true);
	RecoveryBounds->OnComponentBeginOverlap.AddUniqueDynamic(this, &ADemoRecoveryVolume::HandleOverlap);
}

bool ADemoRecoveryVolume::RecoverActor(AActor* Actor)
{
	if (!IsValid(Actor)) return false;
	if (UDemoPlayerRecoveryComponent* PlayerRecovery = Actor->FindComponentByClass<UDemoPlayerRecoveryComponent>()) return PlayerRecovery->RecoverPlayer();
	if (UDemoRecoverableComponent* PropRecovery = Actor->FindComponentByClass<UDemoRecoverableComponent>()) return PropRecovery->RecoverToBaseline();
	return false;
}

void ADemoRecoveryVolume::HandleOverlap(UPrimitiveComponent*, AActor* OtherActor, UPrimitiveComponent*, int32, bool, const FHitResult&)
{
	RecoverActor(OtherActor);
}
