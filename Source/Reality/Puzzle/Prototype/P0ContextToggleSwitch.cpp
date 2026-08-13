// Copyright Epic Games, Inc. All Rights Reserved.

#include "Puzzle/Prototype/P0ContextToggleSwitch.h"

#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Engine/Engine.h"
#include "Engine/CollisionProfile.h"
#include "Reality.h"
#include "RealitySystem/RealityContextComponent.h"
#include "UObject/ConstructorHelpers.h"

AP0ContextToggleSwitch::AP0ContextToggleSwitch()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Switch Mesh"));
	SetRootComponent(Mesh);
	Mesh->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		Mesh->SetStaticMesh(CubeMesh.Object);
	}
	Mesh->SetRelativeScale3D(FVector(0.15f, 0.45f, 0.65f));

	StatusText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Status Text"));
	StatusText->SetupAttachment(Mesh);
	StatusText->SetRelativeLocation(FVector(55.0f, 0.0f, 0.0f));
	StatusText->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
	StatusText->SetHorizontalAlignment(EHTA_Center);
	StatusText->SetWorldSize(18.0f);
	StatusText->SetTextRenderColor(FColor::Silver);
}

void AP0ContextToggleSwitch::BeginPlay()
{
	Super::BeginPlay();
	RefreshStatusFeedback();
}

void AP0ContextToggleSwitch::SetContextSourceActor(AActor* NewContextSourceActor)
{
	ContextSourceActor = NewContextSourceActor;
	RefreshStatusFeedback();
}

URealityContextComponent* AP0ContextToggleSwitch::GetTargetContextComponent() const
{
	return IsValid(ContextSourceActor)
		? ContextSourceActor->FindComponentByClass<URealityContextComponent>()
		: nullptr;
}

bool AP0ContextToggleSwitch::CanInteract_Implementation(AActor* Interactor) const
{
	return IsValid(Interactor) && IsValid(GetTargetContextComponent());
}

void AP0ContextToggleSwitch::Interact_Implementation(AActor* Interactor)
{
	if (!CanInteract_Implementation(Interactor))
	{
		return;
	}

	URealityContextComponent* ContextComponent = GetTargetContextComponent();
	ContextComponent->SetContextActive(!ContextComponent->IsContextActive());
	RefreshStatusFeedback();

	const TCHAR* StateLabel = ContextComponent->IsContextActive() ? TEXT("ACTIVE") : TEXT("INACTIVE");
	UE_LOG(LogReality, Log, TEXT("P0 Context Switch '%s': '%s' is now %s."), *GetNameSafe(this), *GetNameSafe(ContextSourceActor), StateLabel);
#if !UE_BUILD_SHIPPING
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 3.0f, ContextComponent->IsContextActive() ? FColor::Green : FColor::Silver,
			FString::Printf(TEXT("CONTEXT %s: %s"), StateLabel, *ContextComponent->GetContextTags().ToStringSimple()));
	}
#endif
}

FText AP0ContextToggleSwitch::GetInteractionPrompt_Implementation(AActor* Interactor) const
{
	const URealityContextComponent* ContextComponent = GetTargetContextComponent();
	return ContextComponent && ContextComponent->IsContextActive()
		? NSLOCTEXT("RealityP0", "DeactivateContext", "Deactivate Context")
		: NSLOCTEXT("RealityP0", "ActivateContext", "Activate Context");
}

void AP0ContextToggleSwitch::RefreshStatusFeedback()
{
	if (!StatusText)
	{
		return;
	}

	const URealityContextComponent* ContextComponent = GetTargetContextComponent();
	const bool bActive = ContextComponent && ContextComponent->IsContextActive();
	StatusText->SetText(bActive
		? NSLOCTEXT("RealityP0", "ContextActive", "CONTEXT ACTIVE")
		: NSLOCTEXT("RealityP0", "ContextInactive", "CONTEXT INACTIVE"));
	StatusText->SetTextRenderColor(bActive ? FColor::Green : FColor::Silver);
}
