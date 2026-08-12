// Copyright Epic Games, Inc. All Rights Reserved.

#include "Interaction/InteractionTestActor.h"

#include "Engine/CollisionProfile.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Reality.h"

AInteractionTestActor::AInteractionTestActor()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
	Mesh->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		Mesh->SetStaticMesh(CubeMesh.Object);
	}

	InteractionPrompt = NSLOCTEXT("RealityInteraction", "TestInteractionPrompt", "Test Interaction");
}

bool AInteractionTestActor::CanInteract_Implementation(AActor* Interactor) const
{
	return bInteractionAllowed && IsValid(Interactor);
}

void AInteractionTestActor::Interact_Implementation(AActor* Interactor)
{
	if (!CanInteract_Implementation(Interactor))
	{
		return;
	}

	++InteractionCount;
	UE_LOG(LogReality, Log, TEXT("Interaction test: '%s' was triggered by '%s' (Count=%d)."), *GetNameSafe(this), *GetNameSafe(Interactor), InteractionCount);
	BP_OnInteracted(Interactor, InteractionCount);
}

FText AInteractionTestActor::GetInteractionPrompt_Implementation(AActor* Interactor) const
{
	return InteractionPrompt;
}

void AInteractionTestActor::HandleFocusGained(AActor* FocusedActor)
{
	if (FocusedActor == this)
	{
		++FocusGainedCount;
	}
}

void AInteractionTestActor::HandleFocusLost(AActor* UnfocusedActor)
{
	if (UnfocusedActor == this)
	{
		++FocusLostCount;
	}
}
