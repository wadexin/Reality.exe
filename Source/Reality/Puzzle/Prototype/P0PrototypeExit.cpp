// Copyright Epic Games, Inc. All Rights Reserved.

#include "Puzzle/Prototype/P0PrototypeExit.h"

#include "Components/BoxComponent.h"
#include "Components/TextRenderComponent.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "Reality.h"
#include "RealitySystem/RealityManagerSubsystem.h"

AP0PrototypeExit::AP0PrototypeExit()
{
	PrimaryActorTick.bCanEverTick = false;

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Exit Trigger"));
	SetRootComponent(Trigger);
	Trigger->SetBoxExtent(FVector(140.0f, 180.0f, 150.0f));
	Trigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Trigger->SetCollisionResponseToAllChannels(ECR_Ignore);
	Trigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AP0PrototypeExit::HandleTriggerBeginOverlap);

	ExitLabel = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Exit Label"));
	ExitLabel->SetupAttachment(Trigger);
	ExitLabel->SetRelativeLocation(FVector(0.0f, 0.0f, 175.0f));
	ExitLabel->SetHorizontalAlignment(EHTA_Center);
	ExitLabel->SetWorldSize(60.0f);
	ExitLabel->SetTextRenderColor(FColor::Green);
	ExitLabel->SetText(NSLOCTEXT("RealityP0", "ExitLabel", "EXIT"));
}

void AP0PrototypeExit::HandleTriggerBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComponent,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (bPrototypeCompleted || !Cast<APawn>(OtherActor))
	{
		return;
	}

	bPrototypeCompleted = true;
	float Suspicion = 0.0f;
	FString RealityState = TEXT("Unknown");
	if (const UWorld* World = GetWorld())
	{
		if (const URealityManagerSubsystem* RealityManager = World->GetSubsystem<URealityManagerSubsystem>())
		{
			Suspicion = RealityManager->GetSuspicion();
			RealityState = StaticEnum<ERealityState>()->GetDisplayNameTextByValue(static_cast<int64>(RealityManager->GetRealityState())).ToString();
		}
	}

	const TCHAR* Grade = Suspicion < 40.0f ? TEXT("CLEAN") : Suspicion < 80.0f ? TEXT("MESSY") : TEXT("FLAGGED");
	const FString CompletionMessage = FString::Printf(
		TEXT("PROTOTYPE COMPLETE\nSuspicion: %.0f\nReality: %s\nResult: %s"), Suspicion, *RealityState, Grade);
	UE_LOG(LogReality, Log, TEXT("P0 Prototype completed by '%s'. Suspicion=%.1f State=%s Grade=%s."),
		*GetNameSafe(OtherActor), Suspicion, *RealityState, Grade);
#if !UE_BUILD_SHIPPING
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 15.0f, FColor::Green, CompletionMessage);
	}
#endif
}
