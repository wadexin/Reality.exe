// Copyright Epic Games, Inc. All Rights Reserved.

#include "Puzzle/Demo/DemoEditableActor.h"

#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Developer/RealityEditableComponent.h"
#include "Engine/CollisionProfile.h"
#include "NativeGameplayTags.h"
#include "Reality.h"
#include "UObject/ConstructorHelpers.h"

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Demo_Collision, "Cheat.Collision");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Demo_Scale, "Cheat.Scale");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Demo_Gravity, "Cheat.Gravity");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Demo_Mass, "Cheat.Mass");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Demo_Friction, "Cheat.Friction");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Demo_Time, "Cheat.Time");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Demo_ObjectDoor, "Object.Door");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Demo_ObjectProp, "Object.Prop");

ADemoEditableActor::ADemoEditableActor()
{
	PrimaryActorTick.bCanEverTick = false;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Target Mesh"));
	SetRootComponent(Mesh);
	Mesh->SetMobility(EComponentMobility::Movable);
	Mesh->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		Mesh->SetStaticMesh(CubeMesh.Object);
	}

	EditableMarker = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Editable Marker"));
	EditableMarker->SetupAttachment(Mesh);
	EditableMarker->SetRelativeLocation(FVector(51.0f, 0.0f, 0.0f));
	EditableMarker->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
	EditableMarker->SetAbsolute(false, false, true);
	EditableMarker->SetHorizontalAlignment(EHTA_Center);
	EditableMarker->SetWorldSize(18.0f);
	EditableMarker->SetTextRenderColor(FColor(40, 220, 255));
	EditableMarker->SetText(NSLOCTEXT("RealityDemo", "EditableMarker", "REALITY EDITABLE"));

	EditableComponent = CreateDefaultSubobject<URealityEditableComponent>(TEXT("Reality Editable Component"));
}

void ADemoEditableActor::ConfigureDemoTarget(
	const FText DisplayName,
	const bool bCollision,
	const bool bScale,
	const bool bGravity,
	const bool bMass,
	const bool bFriction,
	const bool bTime,
	const bool bSimulatePhysics,
	const bool bDoor,
	const bool bEnablePushInteraction)
{
	TargetDisplayName = DisplayName;
	bPushInteractionEnabled = bEnablePushInteraction;

	FGameplayTagContainer Capabilities;
	if (bCollision) Capabilities.AddTag(TAG_Demo_Collision);
	if (bScale) Capabilities.AddTag(TAG_Demo_Scale);
	if (bGravity) Capabilities.AddTag(TAG_Demo_Gravity);
	if (bMass) Capabilities.AddTag(TAG_Demo_Mass);
	if (bFriction) Capabilities.AddTag(TAG_Demo_Friction);
	if (bTime) Capabilities.AddTag(TAG_Demo_Time);
	EditableComponent->SetSupportedCheats(Capabilities);

	FGameplayTagContainer ObjectTags;
	ObjectTags.AddTag(bDoor ? TAG_Demo_ObjectDoor : TAG_Demo_ObjectProp);
	EditableComponent->SetObjectTags(ObjectTags);

	Mesh->SetCollisionProfileName(bSimulatePhysics ? UCollisionProfile::PhysicsActor_ProfileName : UCollisionProfile::BlockAll_ProfileName);
	Mesh->SetSimulatePhysics(bSimulatePhysics);
	EditableMarker->SetText(FText::Format(
		NSLOCTEXT("RealityDemo", "EditableTargetFormat", "REALITY EDITABLE\n{0}"),
		DisplayName));
}

bool ADemoEditableActor::CanInteract_Implementation(AActor* Interactor) const
{
	return IsValid(Interactor) && bPushInteractionEnabled && Mesh->IsSimulatingPhysics();
}

void ADemoEditableActor::Interact_Implementation(AActor* Interactor)
{
	if (!CanInteract_Implementation(Interactor))
	{
		return;
	}

	FVector PushDirection = GetActorLocation() - Interactor->GetActorLocation();
	PushDirection.Z = FMath::Max(PushDirection.Z, 30.0f);
	PushDirection = PushDirection.GetSafeNormal();
	Mesh->AddImpulse(PushDirection * PushImpulse);
	UE_LOG(LogReality, Log, TEXT("Demo target '%s' pushed by '%s' with impulse %.0f."), *GetNameSafe(this), *GetNameSafe(Interactor), PushImpulse);
}

FText ADemoEditableActor::GetInteractionPrompt_Implementation(AActor* Interactor) const
{
	return TargetDisplayName.IsEmpty()
		? NSLOCTEXT("RealityDemo", "PushTarget", "Push Test Prop")
		: FText::Format(NSLOCTEXT("RealityDemo", "PushNamedTarget", "Push {0}"), TargetDisplayName);
}
