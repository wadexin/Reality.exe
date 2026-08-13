// Copyright Epic Games, Inc. All Rights Reserved.

#include "AI/RealityWitnessTestActor.h"

#include "AI/RealityWitnessComponent.h"
#include "Components/SceneComponent.h"

ARealityWitnessTestActor::ARealityWitnessTestActor()
{
	PrimaryActorTick.bCanEverTick = false;
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Root"));
	SetRootComponent(SceneRoot);
	WitnessComponent = CreateDefaultSubobject<URealityWitnessComponent>(TEXT("Reality Witness Component"));
}

void ARealityWitnessTestActor::HandleRealityEventObserved(const FRealityCheatEvent& CheatEvent)
{
	++ObservedEventCount;
	LastObservedEvent = CheatEvent;
}
