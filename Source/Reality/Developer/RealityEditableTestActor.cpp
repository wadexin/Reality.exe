// Copyright Epic Games, Inc. All Rights Reserved.

#include "Developer/RealityEditableTestActor.h"

#include "Components/BoxComponent.h"
#include "Developer/RealityEditableComponent.h"

ARealityEditableTestActor::ARealityEditableTestActor()
{
	PrimaryActorTick.bCanEverTick = false;

	PrimitiveA = CreateDefaultSubobject<UBoxComponent>(TEXT("Primitive A"));
	SetRootComponent(PrimitiveA);

	PrimitiveB = CreateDefaultSubobject<UBoxComponent>(TEXT("Primitive B"));
	PrimitiveB->SetupAttachment(PrimitiveA);

	EditableComponent = CreateDefaultSubobject<URealityEditableComponent>(TEXT("Reality Editable Component"));
}

void ARealityEditableTestActor::HandleRealityCheatEvent(const FRealityCheatEvent& CheatEvent)
{
	++CheatEventCount;
	LastCheatEvent = CheatEvent;
}

void ARealityEditableTestActor::HandleDeveloperFocusGained(AActor* FocusedActor)
{
	if (FocusedActor == this)
	{
		++DeveloperFocusGainedCount;
	}
}

void ARealityEditableTestActor::HandleDeveloperFocusLost(AActor* LostActor)
{
	if (LostActor == this)
	{
		++DeveloperFocusLostCount;
	}
}

void ARealityEditableTestActor::HandleSuspicionChanged(const float OldValue, const float NewValue)
{
	++SuspicionChangedCount;
	LastOldSuspicion = OldValue;
	LastNewSuspicion = NewValue;
}

void ARealityEditableTestActor::HandleRealityStateChanged(const ERealityState OldState, const ERealityState NewState)
{
	++RealityStateChangedCount;
	LastOldRealityState = OldState;
	LastNewRealityState = NewState;
}
