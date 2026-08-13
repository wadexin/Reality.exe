// Copyright Epic Games, Inc. All Rights Reserved.

#include "Puzzle/Demo/DemoWitnessActor.h"

#include "AI/RealityWitnessComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "NativeGameplayTags.h"
#include "UObject/ConstructorHelpers.h"

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Demo_WitnessHuman, "Witness.Human");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Demo_WitnessCamera, "Witness.Camera");

ADemoWitnessActor::ADemoWitnessActor()
{
	PrimaryActorTick.bCanEverTick = false;
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Root"));
	SetRootComponent(SceneRoot);

	DirectionMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Direction Mesh"));
	DirectionMesh->SetupAttachment(SceneRoot);
	DirectionMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DirectionMesh->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
	DirectionMesh->SetRelativeScale3D(FVector(0.35f, 0.35f, 0.8f));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ConeMesh(TEXT("/Engine/BasicShapes/Cone.Cone"));
	if (ConeMesh.Succeeded())
	{
		DirectionMesh->SetStaticMesh(ConeMesh.Object);
	}

	WitnessLabel = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Witness Label"));
	WitnessLabel->SetupAttachment(SceneRoot);
	WitnessLabel->SetRelativeLocation(FVector(0.0f, 0.0f, 120.0f));
	WitnessLabel->SetHorizontalAlignment(EHTA_Center);
	WitnessLabel->SetWorldSize(24.0f);
	WitnessLabel->SetTextRenderColor(FColor::Red);
	WitnessLabel->SetText(NSLOCTEXT("RealityDemo", "WitnessLabel", "WITNESS\nFACING ->"));

	WitnessComponent = CreateDefaultSubobject<URealityWitnessComponent>(TEXT("Reality Witness Component"));
}

void ADemoWitnessActor::ConfigureDemoWitness(
	const bool bCamera,
	const float ObservationRange,
	const float FieldOfViewDegrees,
	const bool bRequireLineOfSight)
{
	WitnessComponent->WitnessType = bCamera ? TAG_Demo_WitnessCamera : TAG_Demo_WitnessHuman;
	WitnessComponent->ObservationRange = FMath::Max(0.0f, ObservationRange);
	WitnessComponent->FieldOfViewDegrees = FMath::Clamp(FieldOfViewDegrees, 0.0f, 360.0f);
	WitnessComponent->bRequireLineOfSight = bRequireLineOfSight;
	WitnessLabel->SetText(bCamera
		? NSLOCTEXT("RealityDemo", "CameraWitnessLabel", "SECURITY CAMERA\nFACING ->")
		: NSLOCTEXT("RealityDemo", "HumanWitnessLabel", "HUMAN WITNESS\nFACING ->"));
}
