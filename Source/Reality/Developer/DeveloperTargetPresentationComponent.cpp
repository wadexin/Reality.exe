// Copyright Epic Games, Inc. All Rights Reserved.

#include "Developer/DeveloperTargetPresentationComponent.h"

#include "Camera/CameraComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "Reality.h"

UDeveloperTargetPresentationComponent::UDeveloperTargetPresentationComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	OutlineMaterial = TSoftObjectPtr<UMaterialInterface>(FSoftObjectPath(TEXT("/Game/Developer/Presentation/M_RLT_DeveloperOutline.M_RLT_DeveloperOutline")));
}

void UDeveloperTargetPresentationComponent::BeginPlay()
{
	Super::BeginPlay();
	PresentationCamera = GetOwner() ? GetOwner()->FindComponentByClass<UCameraComponent>() : nullptr;
}

void UDeveloperTargetPresentationComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	RestoreHighlight();
	DetachPostProcess();
	Super::EndPlay(EndPlayReason);
}

void UDeveloperTargetPresentationComponent::SetPresentationActive(const bool bNewActive)
{
	if (bPresentationActive == bNewActive)
	{
		return;
	}

	bPresentationActive = bNewActive;
	if (bPresentationActive)
	{
		AttachPostProcess();
	}
	else
	{
		RestoreHighlight();
		DetachPostProcess();
	}
}

void UDeveloperTargetPresentationComponent::SetHighlightedActor(AActor* NewTarget)
{
	if (!bPresentationActive)
	{
		NewTarget = nullptr;
	}
	if (HighlightedActor.Get() == NewTarget)
	{
		return;
	}

	RestoreHighlight();
	if (IsValid(NewTarget))
	{
		ApplyHighlight(NewTarget);
	}
}

void UDeveloperTargetPresentationComponent::ApplyHighlight(AActor* Target)
{
	HighlightedActor = Target;
	Target->OnDestroyed.AddUniqueDynamic(this, &UDeveloperTargetPresentationComponent::HandleHighlightedActorDestroyed);

	TInlineComponentArray<UPrimitiveComponent*> Primitives(Target);
	for (UPrimitiveComponent* Primitive : Primitives)
	{
		if (!IsValid(Primitive)
			|| Primitive->GetOwner() != Target
			|| !Primitive->IsRegistered()
			|| !Primitive->IsVisible()
			|| Primitive->bHiddenInGame)
		{
			continue;
		}

		FDeveloperPrimitiveRenderState& SavedState = SavedPrimitiveStates.AddDefaulted_GetRef();
		SavedState.Primitive = Primitive;
		SavedState.bRenderedCustomDepth = Primitive->bRenderCustomDepth;
		Primitive->SetRenderCustomDepth(true);
	}
}

void UDeveloperTargetPresentationComponent::RestoreHighlight()
{
	if (AActor* Target = HighlightedActor.Get())
	{
		Target->OnDestroyed.RemoveDynamic(this, &UDeveloperTargetPresentationComponent::HandleHighlightedActorDestroyed);
	}

	for (const FDeveloperPrimitiveRenderState& SavedState : SavedPrimitiveStates)
	{
		if (UPrimitiveComponent* Primitive = SavedState.Primitive.Get())
		{
			Primitive->SetRenderCustomDepth(SavedState.bRenderedCustomDepth);
		}
	}
	SavedPrimitiveStates.Reset();
	HighlightedActor.Reset();
}

void UDeveloperTargetPresentationComponent::AttachPostProcess()
{
	UCameraComponent* Camera = PresentationCamera.Get();
	UMaterialInterface* Material = OutlineMaterial.LoadSynchronous();
	if (!IsValid(Camera) || !IsValid(Material))
	{
		UE_LOG(LogReality, Verbose, TEXT("Developer presentation: outline material or player camera is unavailable."));
		return;
	}

	if (!OutlineMaterialInstance)
	{
		OutlineMaterialInstance = UMaterialInstanceDynamic::Create(Material, this);
	}
	Camera->AddOrUpdateBlendable(OutlineMaterialInstance, 1.0f);
}

void UDeveloperTargetPresentationComponent::DetachPostProcess()
{
	if (UCameraComponent* Camera = PresentationCamera.Get(); Camera && OutlineMaterialInstance)
	{
		Camera->RemoveBlendable(OutlineMaterialInstance);
	}
	OutlineMaterialInstance = nullptr;
}

void UDeveloperTargetPresentationComponent::HandleHighlightedActorDestroyed(AActor* DestroyedActor)
{
	if (HighlightedActor.Get() == DestroyedActor)
	{
		RestoreHighlight();
	}
}
