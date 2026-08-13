// Copyright Epic Games, Inc. All Rights Reserved.

#include "Puzzle/Demo/DemoTimeMachineryActor.h"

#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Developer/RealityEditableComponent.h"
#include "Engine/CollisionProfile.h"
#include "NativeGameplayTags.h"
#include "UObject/ConstructorHelpers.h"

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Demo_TimeMachinery, "Cheat.Time");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Demo_SecurityObject, "Object.Security");

ADemoTimeMachineryActor::ADemoTimeMachineryActor()
{
	PrimaryActorTick.bCanEverTick = true;
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Root"));
	SetRootComponent(SceneRoot);

	RotorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Rotor"));
	RotorMesh->SetupAttachment(SceneRoot);
	RotorMesh->SetMobility(EComponentMobility::Movable);
	RotorMesh->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);
	RotorMesh->SetRelativeScale3D(FVector(0.25f, 4.5f, 0.25f));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		RotorMesh->SetStaticMesh(CubeMesh.Object);
	}

	MachineryLabel = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Machinery Label"));
	MachineryLabel->SetupAttachment(SceneRoot);
	MachineryLabel->SetRelativeLocation(FVector(0.0f, 0.0f, 160.0f));
	MachineryLabel->SetHorizontalAlignment(EHTA_Center);
	MachineryLabel->SetWorldSize(22.0f);
	MachineryLabel->SetTextRenderColor(FColor::Silver);

	EditableComponent = CreateDefaultSubobject<URealityEditableComponent>(TEXT("Reality Editable Component"));
	FGameplayTagContainer Capabilities;
	Capabilities.AddTag(TAG_Demo_TimeMachinery);
	EditableComponent->SetSupportedCheats(Capabilities);
	FGameplayTagContainer ObjectTags;
	ObjectTags.AddTag(TAG_Demo_SecurityObject);
	EditableComponent->SetObjectTags(ObjectTags);
}

void ADemoTimeMachineryActor::ConfigureTimeMachinery(
	const FText DisplayName,
	const float InRotationDegreesPerSecond)
{
	RotationDegreesPerSecond = InRotationDegreesPerSecond;
	MachineryLabel->SetText(FText::Format(
		NSLOCTEXT("RealityDemo", "EditableMachine", "{0}\nREALITY EDITABLE: TIME"),
		DisplayName));
	MachineryLabel->SetTextRenderColor(FColor(40, 220, 255));
}

void ADemoTimeMachineryActor::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	AccumulatedTickSeconds += DeltaSeconds;
	RotorMesh->AddLocalRotation(FRotator(0.0f, RotationDegreesPerSecond * DeltaSeconds, 0.0f));
}
