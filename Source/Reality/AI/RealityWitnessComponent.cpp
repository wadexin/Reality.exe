// Copyright Epic Games, Inc. All Rights Reserved.

#include "AI/RealityWitnessComponent.h"

#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "RealitySystem/RealityManagerSubsystem.h"

URealityWitnessComponent::URealityWitnessComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void URealityWitnessComponent::BeginPlay()
{
	Super::BeginPlay();
	if (UWorld* World = GetWorld())
	{
		if (URealityManagerSubsystem* RealityManager = World->GetSubsystem<URealityManagerSubsystem>())
		{
			RealityManager->RegisterWitness(this);
		}
	}
}

void URealityWitnessComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld())
	{
		if (URealityManagerSubsystem* RealityManager = World->GetSubsystem<URealityManagerSubsystem>())
		{
			RealityManager->UnregisterWitness(this);
		}
	}
	Super::EndPlay(EndPlayReason);
}

bool URealityWitnessComponent::GetObservationViewpoint(FVector& OutOrigin, FVector& OutForward) const
{
	const AActor* Owner = GetOwner();
	if (!IsValid(Owner))
	{
		return false;
	}

	FRotator ViewRotation;
	Owner->GetActorEyesViewPoint(OutOrigin, ViewRotation);
	OutForward = ViewRotation.Vector().GetSafeNormal();
	if (OutForward.IsNearlyZero())
	{
		OutOrigin = Owner->GetActorLocation();
		OutForward = Owner->GetActorForwardVector().GetSafeNormal();
	}
	return !OutForward.IsNearlyZero();
}

bool URealityWitnessComponent::CanObserveRealityEvent(const FRealityCheatEvent& CheatEvent) const
{
	const AActor* Owner = GetOwner();
	AActor* TargetActor = CheatEvent.TargetActor.Get();
	UWorld* World = GetWorld();
	if (!bWitnessEnabled || !IsValid(Owner) || !IsValid(TargetActor) || !IsValid(World) || TargetActor == Owner)
	{
		return false;
	}

	FVector ObservationOrigin;
	FVector ObservationForward;
	if (!GetObservationViewpoint(ObservationOrigin, ObservationForward))
	{
		return false;
	}

	const FVector TargetOrigin = GetObservationTargetPoint(TargetActor);

	const FVector ToTarget = TargetOrigin - ObservationOrigin;
	const float DistanceSquared = ToTarget.SizeSquared();
	const float SafeRange = FMath::Max(0.0f, ObservationRange);
	if (DistanceSquared > FMath::Square(SafeRange) || DistanceSquared <= UE_KINDA_SMALL_NUMBER)
	{
		return false;
	}

	const float SafeFovDegrees = FMath::Clamp(FieldOfViewDegrees, 0.0f, 360.0f);
	const float HalfFovRadians = FMath::DegreesToRadians(SafeFovDegrees * 0.5f);
	if (SafeFovDegrees < 360.0f && FVector::DotProduct(ObservationForward, ToTarget.GetSafeNormal()) < FMath::Cos(HalfFovRadians))
	{
		return false;
	}

	if (!bRequireLineOfSight)
	{
		return true;
	}

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(RealityWitnessLineOfSight), false, Owner);
	const float TargetDistance = FMath::Sqrt(DistanceSquared);
	FHitResult BlockingHit;
	if (!World->LineTraceSingleByChannel(BlockingHit, ObservationOrigin, TargetOrigin, ECC_Visibility, QueryParams))
	{
		return true;
	}

	if (BlockingHit.GetActor() == TargetActor)
	{
		return true;
	}

	// Collision-off targets need not be hit. Any blocking hit at or beyond the deterministic target point is still clear.
	return BlockingHit.Distance + UE_KINDA_SMALL_NUMBER >= TargetDistance;
}

FVector URealityWitnessComponent::GetObservationTargetPoint(const AActor* TargetActor) const
{
	if (!IsValid(TargetActor))
	{
		return FVector::ZeroVector;
	}

	FVector TargetOrigin;
	FVector TargetExtent;
	TargetActor->GetActorBounds(true, TargetOrigin, TargetExtent, false);
	return TargetOrigin.ContainsNaN() || TargetExtent.IsNearlyZero()
		? TargetActor->GetActorLocation()
		: TargetOrigin;
}

void URealityWitnessComponent::NotifyRealityEventObserved(const FRealityCheatEvent& CheatEvent)
{
	if (bWitnessEnabled)
	{
		OnRealityEventObserved.Broadcast(CheatEvent);
	}
}
