// Copyright Epic Games, Inc. All Rights Reserved.

#include "Puzzle/Demo/DemoSensorFailurePanel.h"

#include "Components/StaticMeshComponent.h"
#include "Components/AudioComponent.h"
#include "Components/TextRenderComponent.h"
#include "Engine/CollisionProfile.h"
#include "NativeGameplayTags.h"
#include "Reality.h"
#include "Puzzle/Demo/DemoLanguage.h"
#include "RealitySystem/RealityContextComponent.h"
#include "Kismet/GameplayStatics.h"
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

	InfrastructureAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("Sensor Infrastructure Audio"));
	InfrastructureAudio->SetupAttachment(PanelMesh);
	InfrastructureAudio->bAutoActivate = true;
	InfrastructureAudio->bAllowSpatialization = true;
	InfrastructureAudio->bStopWhenOwnerDestroyed = true;
	InfrastructureAudio->bOverrideAttenuation = true;
	InfrastructureAudio->AttenuationOverrides.bAttenuate = true;
	InfrastructureAudio->AttenuationOverrides.bSpatialize = true;
	InfrastructureAudio->AttenuationOverrides.AttenuationShapeExtents = FVector(180.0f);
	InfrastructureAudio->AttenuationOverrides.FalloffDistance = 950.0f;
	static ConstructorHelpers::FObjectFinder<USoundBase> SensorLoop(TEXT("/Game/Audio/Reality/Machinery/S_RLT_UtilityLoop.S_RLT_UtilityLoop"));
	if (SensorLoop.Succeeded()) InfrastructureAudio->SetSound(SensorLoop.Object);
	static ConstructorHelpers::FObjectFinder<USoundBase> SensorToggle(TEXT("/Game/Audio/Reality/Machinery/S_RLT_SensorToggle.S_RLT_SensorToggle"));
	ToggleSound = SensorToggle.Object;
	InfrastructureAudio->SetVolumeMultiplier(0.20f);
}

void ADemoSensorFailurePanel::BeginPlay()
{
	Super::BeginPlay();
	RefreshStatusFeedback();
	RefreshAudioFeedback(false);
}

void ADemoSensorFailurePanel::ConfigureSensorFailure(const float EffectRange, const bool bStartActive)
{
	ContextComponent->EffectRange = FMath::Max(0.0f, EffectRange);
	ContextComponent->SetContextActive(bStartActive);
	RefreshStatusFeedback();
	RefreshAudioFeedback(false);
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
	RefreshAudioFeedback(true);
	UE_LOG(LogReality, Log, TEXT("Demo sensor array '%s' changed to %s by '%s'."), *GetNameSafe(this),
		ContextComponent->IsContextActive() ? TEXT("FAILURE") : TEXT("ONLINE"), *GetNameSafe(Interactor));
}

FText ADemoSensorFailurePanel::GetInteractionPrompt_Implementation(AActor* Interactor) const
{
	return ContextComponent->IsContextActive()
		? RealityDemoLanguage::Text(this, TEXT("E: Restore Sensor Array"), TEXT("E：恢复传感器阵列"))
		: RealityDemoLanguage::Text(this, TEXT("E: Run Sensor Failure Test"), TEXT("E：运行传感器故障测试"));
}

void ADemoSensorFailurePanel::RefreshStatusFeedback()
{
	const bool bFailure = ContextComponent && ContextComponent->IsContextActive();
	StatusText->SetTextRenderColor(bFailure ? FColor::Orange : FColor::Green);
	RealityDemoLanguage::SetWorldText(StatusText, this,
		bFailure ? FText::FromString(TEXT("SECURITY SENSOR ARRAY\nFAILURE ACTIVE")) : FText::FromString(TEXT("SECURITY SENSOR ARRAY\nONLINE\nE: DIAGNOSTIC")),
		bFailure ? FText::FromString(TEXT("安保传感器阵列\n故障已激活")) : FText::FromString(TEXT("安保传感器阵列\n在线\nE：诊断")));
}

void ADemoSensorFailurePanel::RefreshAudioFeedback(const bool bPlayToggleCue)
{
	if (!InfrastructureAudio) return;
	InfrastructureAudio->SetPitchMultiplier(GetInfrastructurePitch());
	InfrastructureAudio->SetVolumeMultiplier(ContextComponent && ContextComponent->IsContextActive() ? 0.14f : 0.20f);
	if (bPlayToggleCue)
	{
		if (ToggleSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, ToggleSound, GetActorLocation(), 0.55f, GetInfrastructurePitch());
		}
	}
}

float ADemoSensorFailurePanel::GetInfrastructurePitch() const
{
	return ContextComponent && ContextComponent->IsContextActive() ? 0.72f : 1.0f;
}
