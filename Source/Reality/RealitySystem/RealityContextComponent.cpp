// Copyright Epic Games, Inc. All Rights Reserved.

#include "RealitySystem/RealityContextComponent.h"

#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "RealitySystem/RealityManagerSubsystem.h"

URealityContextComponent::URealityContextComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void URealityContextComponent::BeginPlay()
{
	Super::BeginPlay();
	if (UWorld* World = GetWorld())
	{
		if (URealityManagerSubsystem* RealityManager = World->GetSubsystem<URealityManagerSubsystem>())
		{
			RealityManager->RegisterContext(this);
		}
	}
}

void URealityContextComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld())
	{
		if (URealityManagerSubsystem* RealityManager = World->GetSubsystem<URealityManagerSubsystem>())
		{
			RealityManager->UnregisterContext(this);
		}
	}
	Super::EndPlay(EndPlayReason);
}

void URealityContextComponent::SetContextActive(const bool bActive)
{
	bContextActive = bActive;
}

bool URealityContextComponent::IsTargetWithinContext(const AActor* TargetActor) const
{
	const AActor* Owner = GetOwner();
	if (!bContextActive || !IsValid(Owner) || !IsValid(TargetActor))
	{
		return false;
	}

	const float SafeRange = FMath::Max(0.0f, EffectRange);
	return FVector::DistSquared(GetContextOrigin(), GetTargetPoint(TargetActor)) <= FMath::Square(SafeRange);
}

FVector URealityContextComponent::GetContextOrigin() const
{
	const AActor* Owner = GetOwner();
	if (!IsValid(Owner))
	{
		return FVector::ZeroVector;
	}

	FVector Origin;
	FVector Extent;
	Owner->GetActorBounds(true, Origin, Extent, false);
	return Origin.ContainsNaN() || Extent.IsNearlyZero() ? Owner->GetActorLocation() : Origin;
}

FVector URealityContextComponent::GetTargetPoint(const AActor* TargetActor) const
{
	if (!IsValid(TargetActor))
	{
		return FVector::ZeroVector;
	}

	FVector Origin;
	FVector Extent;
	TargetActor->GetActorBounds(true, Origin, Extent, false);
	return Origin.ContainsNaN() || Extent.IsNearlyZero() ? TargetActor->GetActorLocation() : Origin;
}
