// Copyright Epic Games, Inc. All Rights Reserved.

#include "Developer/RealityTimeTestActor.h"

#include "Components/StaticMeshComponent.h"
#include "Developer/RealityEditableComponent.h"
#include "Engine/CollisionProfile.h"
#include "GameplayTagContainer.h"
#include "UObject/ConstructorHelpers.h"

ARealityTimeTestActor::ARealityTimeTestActor()
{
	PrimaryActorTick.bCanEverTick = true;
	RotorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Rotor Mesh"));
	SetRootComponent(RotorMesh);
	RotorMesh->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);
	RotorMesh->SetRelativeScale3D(FVector(2.0f, 0.2f, 0.2f));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded()) RotorMesh->SetStaticMesh(CubeMesh.Object);
	EditableComponent = CreateDefaultSubobject<URealityEditableComponent>(TEXT("Reality Editable Component"));
	FGameplayTagContainer Capabilities;
	Capabilities.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Cheat.Time")));
	EditableComponent->SetSupportedCheats(Capabilities);
}

void ARealityTimeTestActor::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	LastReceivedDeltaSeconds = DeltaSeconds;
	AccumulatedTickSeconds += DeltaSeconds;
	AddActorLocalRotation(FRotator(0.0f, RotationDegreesPerSecond * DeltaSeconds, 0.0f));
}
