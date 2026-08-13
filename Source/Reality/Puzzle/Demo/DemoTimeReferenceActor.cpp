// Copyright Epic Games, Inc. All Rights Reserved.

#include "Puzzle/Demo/DemoTimeReferenceActor.h"

#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Engine/CollisionProfile.h"
#include "UObject/ConstructorHelpers.h"

ADemoTimeReferenceActor::ADemoTimeReferenceActor()
{
	PrimaryActorTick.bCanEverTick = true;
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Root"));
	SetRootComponent(SceneRoot);
	RotorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Reference Rotor"));
	RotorMesh->SetupAttachment(SceneRoot);
	RotorMesh->SetMobility(EComponentMobility::Movable);
	RotorMesh->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);
	RotorMesh->SetRelativeScale3D(FVector(0.25f, 4.5f, 0.25f));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded()) RotorMesh->SetStaticMesh(CubeMesh.Object);

	ReferenceLabel = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Reference Label"));
	ReferenceLabel->SetupAttachment(SceneRoot);
	ReferenceLabel->SetRelativeLocation(FVector(0.0f, 0.0f, 160.0f));
	ReferenceLabel->SetHorizontalAlignment(EHTA_Center);
	ReferenceLabel->SetWorldSize(22.0f);
	ReferenceLabel->SetTextRenderColor(FColor::Silver);
	ReferenceLabel->SetText(NSLOCTEXT("RealityDemo", "ReferenceMachine", "REFERENCE ROTOR\nBASELINE: 1.0x"));
}

void ADemoTimeReferenceActor::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	AccumulatedTickSeconds += DeltaSeconds;
	RotorMesh->AddLocalRotation(FRotator(0.0f, RotationDegreesPerSecond * DeltaSeconds, 0.0f));
}
