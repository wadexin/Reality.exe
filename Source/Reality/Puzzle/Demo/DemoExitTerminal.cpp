// Copyright Epic Games, Inc. All Rights Reserved.

#include "Puzzle/Demo/DemoExitTerminal.h"

#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Engine/CollisionProfile.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "Reality.h"
#include "UObject/ConstructorHelpers.h"

ADemoExitTerminal::ADemoExitTerminal()
{
	PrimaryActorTick.bCanEverTick = false;
	TerminalMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Exit Terminal"));
	SetRootComponent(TerminalMesh);
	TerminalMesh->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);
	TerminalMesh->SetRelativeScale3D(FVector(0.25f, 0.8f, 1.2f));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		TerminalMesh->SetStaticMesh(CubeMesh.Object);
	}

	TerminalLabel = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Exit Label"));
	TerminalLabel->SetupAttachment(TerminalMesh);
	TerminalLabel->SetRelativeLocation(FVector(52.0f, 0.0f, 0.0f));
	TerminalLabel->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
	TerminalLabel->SetAbsolute(false, false, true);
	TerminalLabel->SetHorizontalAlignment(EHTA_Center);
	TerminalLabel->SetWorldSize(22.0f);
	TerminalLabel->SetTextRenderColor(FColor::Green);
	TerminalLabel->SetText(NSLOCTEXT("RealityDemo", "ExitReady", "REALITY CORE EXIT\nE: COMPLETE DEMO"));
}

bool ADemoExitTerminal::CanInteract_Implementation(AActor* Interactor) const
{
	return !bDemoCompleted && IsValid(Cast<APawn>(Interactor));
}

void ADemoExitTerminal::Interact_Implementation(AActor* Interactor)
{
	CompleteDemo(Interactor);
}

FText ADemoExitTerminal::GetInteractionPrompt_Implementation(AActor* Interactor) const
{
	return bDemoCompleted
		? NSLOCTEXT("RealityDemo", "DemoCompletedPrompt", "Demo Complete")
		: NSLOCTEXT("RealityDemo", "CompleteDemoPrompt", "E: Complete Demo");
}

bool ADemoExitTerminal::CompleteDemo(AActor* CompletingActor)
{
	if (!CanInteract_Implementation(CompletingActor))
	{
		return false;
	}

	bDemoCompleted = true;
	if (const UWorld* World = GetWorld())
	{
		if (const URealityManagerSubsystem* RealityManager = World->GetSubsystem<URealityManagerSubsystem>())
		{
			FinalSuspicion = RealityManager->GetSuspicion();
			FinalRealityState = RealityManager->GetRealityState();
		}
	}

	const FString StateName = StaticEnum<ERealityState>()->GetDisplayNameTextByValue(static_cast<int64>(FinalRealityState)).ToString();
	const FText CompletionText = FText::Format(
		NSLOCTEXT("RealityDemo", "CompletionFormat", "DEMO COMPLETE\nSUSPICION: {0}\nREALITY: {1}"),
		FText::AsNumber(FMath::RoundToInt(FinalSuspicion)),
		FText::FromString(StateName));
	TerminalLabel->SetText(CompletionText);
	TerminalLabel->SetTextRenderColor(FColor::Green);
	UE_LOG(LogReality, Log, TEXT("Demo completed by '%s'. Suspicion=%.1f State=%s."), *GetNameSafe(CompletingActor), FinalSuspicion, *StateName);
#if !UE_BUILD_SHIPPING
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 20.0f, FColor::Green, CompletionText.ToString());
	}
#endif
	return true;
}
