// Copyright Epic Games, Inc. All Rights Reserved.

#include "Puzzle/Demo/DemoSensorFailurePanel.h"

#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Engine/CollisionProfile.h"
#include "NativeGameplayTags.h"
#include "Reality.h"
#include "RealitySystem/RealityContextComponent.h"
#include "UObject/ConstructorHelpers.h"

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Demo_SensorFailure, "Context.SensorFailure");

ADemoSensorFailurePanel::ADemoSensorFailurePanel()
{
	PrimaryActorTick.bCanEverTick = false;
	PanelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sensor Cabinet"));
	SetRootComponent(PanelMesh);
	PanelMesh->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);
	PanelMesh->SetRelativeScale3D(FVector(0.2f, 0.7f, 1.0f));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		PanelMesh->SetStaticMesh(CubeMesh.Object);
	}

	StatusText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Sensor Status"));
	StatusText->SetupAttachment(PanelMesh);
	StatusText->SetRelativeLocation(FVector(52.0f, 0.0f, 0.0f));
	StatusText->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
	StatusText->SetAbsolute(false, false, true);
	StatusText->SetHorizontalAlignment(EHTA_Center);
	StatusText->SetWorldSize(18.0f);

	ContextComponent = CreateDefaultSubobject<URealityContextComponent>(TEXT("Reality Context Component"));
	FGameplayTagContainer ContextTags;
	ContextTags.AddTag(TAG_Demo_SensorFailure);
	ContextComponent->ContextTags = ContextTags;
	ContextComponent->bContextActive = false;
}

void ADemoSensorFailurePanel::BeginPlay()
{
	Super::BeginPlay();
	RefreshStatusFeedback();
}

void ADemoSensorFailurePanel::ConfigureSensorFailure(const float EffectRange, const bool bStartActive)
{
	ContextComponent->EffectRange = FMath::Max(0.0f, EffectRange);
	ContextComponent->SetContextActive(bStartActive);
	RefreshStatusFeedback();
}

bool ADemoSensorFailurePanel::CanInteract_Implementation(AActor* Interactor) const
{
	return IsValid(Interactor) && IsValid(ContextComponent);
}

void ADemoSensorFailurePanel::Interact_Implementation(AActor* Interactor)
{
	if (!CanInteract_Implementation(Interactor))
	{
		return;
	}
	ContextComponent->SetContextActive(!ContextComponent->IsContextActive());
	RefreshStatusFeedback();
	UE_LOG(LogReality, Log, TEXT("Demo sensor array '%s' changed to %s by '%s'."), *GetNameSafe(this),
		ContextComponent->IsContextActive() ? TEXT("FAILURE") : TEXT("ONLINE"), *GetNameSafe(Interactor));
}

FText ADemoSensorFailurePanel::GetInteractionPrompt_Implementation(AActor* Interactor) const
{
	return ContextComponent->IsContextActive()
		? NSLOCTEXT("RealityDemo", "RestoreSensors", "E: Restore Sensor Array")
		: NSLOCTEXT("RealityDemo", "DisableSensors", "E: Run Sensor Failure Test");
}

void ADemoSensorFailurePanel::RefreshStatusFeedback()
{
	const bool bFailure = ContextComponent && ContextComponent->IsContextActive();
	StatusText->SetText(bFailure
		? NSLOCTEXT("RealityDemo", "SensorFailure", "SECURITY SENSOR ARRAY\nFAILURE ACTIVE")
		: NSLOCTEXT("RealityDemo", "SensorOnline", "SECURITY SENSOR ARRAY\nONLINE\nE: DIAGNOSTIC"));
	StatusText->SetTextRenderColor(bFailure ? FColor::Orange : FColor::Green);
}
