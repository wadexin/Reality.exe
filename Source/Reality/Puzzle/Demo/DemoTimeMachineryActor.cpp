// Copyright Epic Games, Inc. All Rights Reserved.

#include "Puzzle/Demo/DemoTimeMachineryActor.h"

#include "Components/SceneComponent.h"
#include "Components/AudioComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Developer/RealityEditableComponent.h"
#include "Engine/CollisionProfile.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInterface.h"
#include "NativeGameplayTags.h"
#include "Puzzle/Demo/DemoPlayerRecoveryComponent.h"
#include "TimerManager.h"
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
	RotorMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	RotorMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	RotorMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	RotorMesh->SetRelativeScale3D(FVector(0.25f, 1.2f, 0.25f));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> HeroArmMesh(TEXT("/Game/Reality/Environment/Hero/TimeMachinery/Meshes/SM_RLT_Rotor_Arm.SM_RLT_Rotor_Arm"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> HeroTipMesh(TEXT("/Game/Reality/Environment/Hero/TimeMachinery/Meshes/SM_RLT_Rotor_Tip.SM_RLT_Rotor_Tip"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> HeroRotorMaterial(TEXT("/Game/Reality/Environment/Hero/TimeMachinery/Materials/MI_RLT_Hero_RotorPrecision.MI_RLT_Hero_RotorPrecision"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> HeroCadenceMaterial(TEXT("/Game/Reality/Environment/Hero/TimeMachinery/Materials/MI_RLT_Hero_RotorCadence.MI_RLT_Hero_RotorCadence"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> ScannerGlassMaterial(TEXT("/Game/Reality/Environment/Materials/Masters/M_Glass.M_Glass"));
	if (CubeMesh.Succeeded())
	{
		RotorMesh->SetStaticMesh(CubeMesh.Object);
	}
	if (HeroArmMesh.Succeeded())
	{
		RotorMesh->SetStaticMesh(HeroArmMesh.Object);
	}
	if (HeroRotorMaterial.Succeeded())
	{
		RotorMesh->SetMaterial(0, HeroRotorMaterial.Object);
	}

	TargetingProxy = CreateDefaultSubobject<UBoxComponent>(TEXT("Developer Targeting Proxy"));
	TargetingProxy->SetupAttachment(SceneRoot);
	TargetingProxy->SetBoxExtent(FVector(110.0f, 160.0f, 140.0f));
	TargetingProxy->SetRelativeLocation(FVector(0.0f, 0.0f, 20.0f));
	TargetingProxy->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TargetingProxy->SetCollisionResponseToAllChannels(ECR_Ignore);
	TargetingProxy->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	TargetingProxy->SetGenerateOverlapEvents(false);

	auto CreateScannerPlane = [this](const TCHAR* Name, const float X)
	{
		UStaticMeshComponent* Plane = CreateDefaultSubobject<UStaticMeshComponent>(Name);
		Plane->SetupAttachment(SceneRoot);
		Plane->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Plane->SetRelativeLocation(FVector(X, 0.0f, 30.0f));
		Plane->SetRelativeScale3D(FVector(0.06f, 6.3f, 3.0f));
		if (CubeMesh.Succeeded()) Plane->SetStaticMesh(CubeMesh.Object);
		if (ScannerGlassMaterial.Succeeded()) Plane->SetMaterial(0, ScannerGlassMaterial.Object);
		return Plane;
	};
	PrimaryScannerPlane = CreateScannerPlane(TEXT("Primary Scanner Light Curtain"), -150.0f);
	SecondaryScannerPlane = CreateScannerPlane(TEXT("Secondary Scanner Light Curtain"), 150.0f);

	auto CreateScannerDetection = [this](const TCHAR* Name, const float X)
	{
		UBoxComponent* Detection = CreateDefaultSubobject<UBoxComponent>(Name);
		Detection->SetupAttachment(SceneRoot);
		Detection->SetBoxExtent(FVector(3.0f, 315.0f, 150.0f));
		Detection->SetRelativeLocation(FVector(X, 0.0f, 30.0f));
		Detection->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Detection->SetCollisionResponseToAllChannels(ECR_Ignore);
		Detection->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
		Detection->SetGenerateOverlapEvents(true);
		Detection->OnComponentBeginOverlap.AddUniqueDynamic(this, &ADemoTimeMachineryActor::HandleScannerOverlap);
		return Detection;
	};
	PrimaryScannerDetection = CreateScannerDetection(TEXT("Primary Scanner Detection"), -150.0f);
	SecondaryScannerDetection = CreateScannerDetection(TEXT("Secondary Scanner Detection"), 150.0f);

	RotorTipPositive = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Rotor Tip Positive"));
	RotorTipPositive->SetupAttachment(RotorMesh);
	RotorTipPositive->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RotorTipPositive->SetRelativeLocation(FVector(0.0f, 50.0f, 0.0f));
	RotorTipPositive->SetAbsolute(false, false, true);
	RotorTipPositive->SetRelativeScale3D(FVector(0.58f, 0.46f, 0.46f));
	if (HeroTipMesh.Succeeded())
	{
		RotorTipPositive->SetStaticMesh(HeroTipMesh.Object);
	}
	if (HeroCadenceMaterial.Succeeded())
	{
		RotorTipPositive->SetMaterial(0, HeroCadenceMaterial.Object);
	}

	RotorTipNegative = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Rotor Tip Negative"));
	RotorTipNegative->SetupAttachment(RotorMesh);
	RotorTipNegative->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RotorTipNegative->SetRelativeLocation(FVector(0.0f, -50.0f, 0.0f));
	RotorTipNegative->SetAbsolute(false, false, true);
	RotorTipNegative->SetRelativeScale3D(FVector(0.58f, 0.46f, 0.46f));
	if (HeroTipMesh.Succeeded())
	{
		RotorTipNegative->SetStaticMesh(HeroTipMesh.Object);
	}
	if (HeroCadenceMaterial.Succeeded())
	{
		RotorTipNegative->SetMaterial(0, HeroCadenceMaterial.Object);
	}

	MachineryLabel = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Machinery Label"));
	MachineryLabel->SetupAttachment(SceneRoot);
	MachineryLabel->SetRelativeLocation(FVector(0.0f, 0.0f, 160.0f));
	MachineryLabel->SetHorizontalAlignment(EHTA_Center);
	MachineryLabel->SetWorldSize(22.0f);
	MachineryLabel->SetTextRenderColor(FColor::Silver);

	EditableComponent = CreateDefaultSubobject<URealityEditableComponent>(TEXT("Reality Editable Component"));
	EditableComponent->SetPlayerFacingName(NSLOCTEXT("RealityDemo", "SecurityRotorName", "Security Rotor"));
	FGameplayTagContainer Capabilities;
	Capabilities.AddTag(TAG_Demo_TimeMachinery);
	EditableComponent->SetSupportedCheats(Capabilities);
	FGameplayTagContainer ObjectTags;
	ObjectTags.AddTag(TAG_Demo_SecurityObject);
	EditableComponent->SetObjectTags(ObjectTags);

	MachineryAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("Time Machinery Audio"));
	MachineryAudio->SetupAttachment(SceneRoot);
	MachineryAudio->bAutoActivate = true;
	MachineryAudio->bAllowSpatialization = true;
	MachineryAudio->bStopWhenOwnerDestroyed = true;
	MachineryAudio->bOverrideAttenuation = true;
	MachineryAudio->AttenuationOverrides.bAttenuate = true;
	MachineryAudio->AttenuationOverrides.bSpatialize = true;
	MachineryAudio->AttenuationOverrides.AttenuationShapeExtents = FVector(250.0f);
	MachineryAudio->AttenuationOverrides.FalloffDistance = 1400.0f;
	static ConstructorHelpers::FObjectFinder<USoundBase> RotorSound(TEXT("/Game/Audio/Reality/Machinery/S_RLT_RotorLoop.S_RLT_RotorLoop"));
	static ConstructorHelpers::FObjectFinder<USoundBase> ViolationSound(TEXT("/Game/Audio/Reality/Machinery/S_RLT_SensorToggle.S_RLT_SensorToggle"));
	if (RotorSound.Succeeded()) MachineryAudio->SetSound(RotorSound.Object);
	if (ViolationSound.Succeeded()) ScannerViolationSound = ViolationSound.Object;
	MachineryAudio->SetVolumeMultiplier(0.34f);
}

void ADemoTimeMachineryActor::BeginPlay()
{
	Super::BeginPlay();
	EditableComponent->OnRealityCheatEvent.AddUniqueDynamic(this, &ADemoTimeMachineryActor::HandleRealityCheatEvent);
	RefreshTimeAudio();
	RefreshScannerState();
}

void ADemoTimeMachineryActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	EditableComponent->OnRealityCheatEvent.RemoveDynamic(this, &ADemoTimeMachineryActor::HandleRealityCheatEvent);
	GetWorldTimerManager().ClearTimer(RejectionTimer);
	if (MachineryAudio) MachineryAudio->Stop();
	Super::EndPlay(EndPlayReason);
}

void ADemoTimeMachineryActor::HandleRealityCheatEvent(const FRealityCheatEvent& CheatEvent)
{
	if (CheatEvent.TargetActor == this && CheatEvent.CheatTag.MatchesTagExact(TAG_Demo_TimeMachinery)) RefreshTimeAudio();
}

void ADemoTimeMachineryActor::RefreshTimeAudio()
{
	if (MachineryAudio) MachineryAudio->SetPitchMultiplier(GetPresentationAudioPitch());
	++AudioStateUpdateCount;
}

float ADemoTimeMachineryActor::GetPresentationAudioPitch() const
{
	if (!EditableComponent || !EditableComponent->IsTimeModified()) return 1.0f;
	switch (EditableComponent->GetCurrentTimePreset())
	{
	case ERealityTimePreset::Quarter: return 0.50f;
	case ERealityTimePreset::Half: return 0.70f;
	case ERealityTimePreset::One: return 1.0f;
	case ERealityTimePreset::Double: return 1.40f;
	case ERealityTimePreset::Quadruple: return 2.0f;
	default: return 1.0f;
	}
}

void ADemoTimeMachineryActor::ConfigureTimeMachinery(
	const FText DisplayName,
	const float InRotationDegreesPerSecond)
{
	RotationDegreesPerSecond = InRotationDegreesPerSecond;
	EditableComponent->SetPlayerFacingName(DisplayName);
	MachineryLabel->SetText(FText::Format(
		NSLOCTEXT("RealityDemo", "EditableMachine", "{0}\nLOCAL-TIME SCANNER DRIVE\nSLOW SCAN TO EXTEND TRANSIT WINDOW"),
		DisplayName));
	MachineryLabel->SetTextRenderColor(FColor(40, 220, 255));
}

void ADemoTimeMachineryActor::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	AccumulatedTickSeconds += DeltaSeconds;
	RotorMesh->AddLocalRotation(FRotator(0.0f, RotationDegreesPerSecond * DeltaSeconds, 0.0f));
	RefreshScannerState();
}

void ADemoTimeMachineryActor::RefreshScannerState()
{
	constexpr float ActiveDuty = 0.72f;
	constexpr float SecondaryPhaseOffset = 90.0f / 360.0f;
	const float RotorDegrees = FMath::Fmod(FMath::Fmod(RotorMesh->GetRelativeRotation().Yaw, 360.0f) + 360.0f, 360.0f);
	const float PrimaryPhase = RotorDegrees / 360.0f;
	const float SecondaryPhase = FMath::Fmod(PrimaryPhase + SecondaryPhaseOffset, 1.0f);
	bPrimaryScannerActive = PrimaryPhase < ActiveDuty;
	bSecondaryScannerActive = SecondaryPhase < ActiveDuty;
	SetScannerActive(PrimaryScannerDetection, PrimaryScannerPlane, bPrimaryScannerActive);
	SetScannerActive(SecondaryScannerDetection, SecondaryScannerPlane, bSecondaryScannerActive);
}

void ADemoTimeMachineryActor::SetScannerActive(UBoxComponent* Detection, UStaticMeshComponent* Visual, const bool bActive)
{
	const ECollisionEnabled::Type DesiredCollision = bActive ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision;
	if (Detection && Detection->GetCollisionEnabled() != DesiredCollision) Detection->SetCollisionEnabled(DesiredCollision);
	if (Visual && Visual->IsVisible() != bActive) Visual->SetVisibility(bActive, true);
}

void ADemoTimeMachineryActor::HandleScannerOverlap(UPrimitiveComponent*, AActor* OtherActor, UPrimitiveComponent*, int32, bool, const FHitResult&)
{
	RejectPlayer(OtherActor);
}

bool ADemoTimeMachineryActor::RejectPlayer(AActor* PlayerActor)
{
	UDemoPlayerRecoveryComponent* Recovery = IsValid(PlayerActor) ? PlayerActor->FindComponentByClass<UDemoPlayerRecoveryComponent>() : nullptr;
	APawn* PlayerPawn = Cast<APawn>(PlayerActor);
	APlayerController* Controller = PlayerPawn ? Cast<APlayerController>(PlayerPawn->GetController()) : nullptr;
	if (bRejectingPlayer || !Recovery || !Controller) return false;

	bRejectingPlayer = true;
	Controller->ClientMessage(TEXT("ACCESS VIOLATION\nUNAUTHORIZED TRANSIT DETECTED\nRETURNING TO CONTROLLED SIDE"), NAME_None, 3.0f);
	if (ScannerViolationSound) UGameplayStatics::PlaySound2D(this, ScannerViolationSound, 0.55f);
	if (Controller->PlayerCameraManager) Controller->PlayerCameraManager->StartCameraFade(0.0f, 1.0f, 0.12f, FLinearColor::Black, false, true);

	const TWeakObjectPtr<AActor> WeakPlayer(PlayerActor);
	GetWorldTimerManager().SetTimer(RejectionTimer, FTimerDelegate::CreateWeakLambda(this, [this, WeakPlayer]()
	{
		if (AActor* Player = WeakPlayer.Get())
		{
			if (UDemoPlayerRecoveryComponent* PlayerRecovery = Player->FindComponentByClass<UDemoPlayerRecoveryComponent>()) PlayerRecovery->RecoverPlayer();
			APawn* Pawn = Cast<APawn>(Player);
			if (APlayerController* PlayerController = Pawn ? Cast<APlayerController>(Pawn->GetController()) : nullptr)
			{
				if (PlayerController->PlayerCameraManager) PlayerController->PlayerCameraManager->StartCameraFade(1.0f, 0.0f, 0.22f, FLinearColor::Black, false, false);
			}
		}
		bRejectingPlayer = false;
	}), 0.13f, false);
	return true;
}
