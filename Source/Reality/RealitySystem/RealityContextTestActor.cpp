// Copyright Epic Games, Inc. All Rights Reserved.

#include "RealitySystem/RealityContextTestActor.h"

#include "Components/SceneComponent.h"
#include "RealitySystem/RealityContextComponent.h"

ARealityContextTestActor::ARealityContextTestActor()
{
	PrimaryActorTick.bCanEverTick = false;
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Root"));
	SetRootComponent(SceneRoot);
	ContextComponent = CreateDefaultSubobject<URealityContextComponent>(TEXT("Reality Context Component"));
}
