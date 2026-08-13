// Copyright Epic Games, Inc. All Rights Reserved.

#include "Puzzle/Demo/DemoTimeReferenceActor.h"

#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Engine/CollisionProfile.h"
#include "Materials/MaterialInterface.h"
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
	static ConstructorHelpers::FObjectFinder<UStaticMesh> HeroArmMesh(TEXT("/Game/Reality/Environment/Hero/TimeMachinery/Meshes/SM_RLT_Rotor_Arm.SM_RLT_Rotor_Arm"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> HeroTipMesh(TEXT("/Game/Reality/Environment/Hero/TimeMachinery/Meshes/SM_RLT_Rotor_Tip.SM_RLT_Rotor_Tip"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> HeroRotorMaterial(TEXT("/Game/Reality/Environment/Hero/TimeMachinery/Materials/MI_RLT_Hero_RotorPrecision.MI_RLT_Hero_RotorPrecision"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> HeroReferenceMaterial(TEXT("/Game/Reality/Environment/Materials/Instances/MI_RLT_Equipment_WarningAmber.MI_RLT_Equipment_WarningAmber"));
	if (CubeMesh.Succeeded()) RotorMesh->SetStaticMesh(CubeMesh.Object);
	if (HeroArmMesh.Succeeded()) RotorMesh->SetStaticMesh(HeroArmMesh.Object);
	if (HeroRotorMaterial.Succeeded()) RotorMesh->SetMaterial(0, HeroRotorMaterial.Object);

	RotorTipPositive = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Reference Tip Positive"));
	RotorTipPositive->SetupAttachment(RotorMesh);
	RotorTipPositive->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RotorTipPositive->SetRelativeLocation(FVector(0.0f, 50.0f, 0.0f));
	RotorTipPositive->SetAbsolute(false, false, true);
	RotorTipPositive->SetRelativeScale3D(FVector(0.58f, 0.46f, 0.46f));
	if (HeroTipMesh.Succeeded()) RotorTipPositive->SetStaticMesh(HeroTipMesh.Object);
	if (HeroReferenceMaterial.Succeeded()) RotorTipPositive->SetMaterial(0, HeroReferenceMaterial.Object);

	RotorTipNegative = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Reference Tip Negative"));
	RotorTipNegative->SetupAttachment(RotorMesh);
	RotorTipNegative->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RotorTipNegative->SetRelativeLocation(FVector(0.0f, -50.0f, 0.0f));
	RotorTipNegative->SetAbsolute(false, false, true);
	RotorTipNegative->SetRelativeScale3D(FVector(0.58f, 0.46f, 0.46f));
	if (HeroTipMesh.Succeeded()) RotorTipNegative->SetStaticMesh(HeroTipMesh.Object);
	if (HeroReferenceMaterial.Succeeded()) RotorTipNegative->SetMaterial(0, HeroReferenceMaterial.Object);

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
