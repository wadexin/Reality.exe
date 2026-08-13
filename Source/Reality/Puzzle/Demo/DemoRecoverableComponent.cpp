// Copyright Epic Games, Inc. All Rights Reserved.
#include "Puzzle/Demo/DemoRecoverableComponent.h"

#include "Components/PrimitiveComponent.h"
#include "Developer/RealityEditableComponent.h"
#include "Engine/Engine.h"
#include "Reality.h"

UDemoRecoverableComponent::UDemoRecoverableComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UDemoRecoverableComponent::BeginPlay()
{
	Super::BeginPlay();
	if (AActor* Owner = GetOwner())
	{
		BaselineTransform = Owner->GetActorTransform();
		bHasBaseline = true;
	}
}

bool UDemoRecoverableComponent::IsRecoveryEnabled() const
{
	if (bRecoveryEnabled) return true;
	const AActor* Owner = GetOwner();
	if (!IsValid(Owner)) return false;
	TInlineComponentArray<UPrimitiveComponent*> Primitives(Owner);
	return Primitives.ContainsByPredicate([Owner](const UPrimitiveComponent* Primitive)
	{
		return Primitive && Primitive->GetOwner() == Owner && Primitive->BodyInstance.bSimulatePhysics;
	});
}

bool UDemoRecoverableComponent::RecoverToBaseline()
{
	AActor* Owner = GetOwner();
	if (!IsRecoveryEnabled() || !bHasBaseline || !IsValid(Owner)) return false;
	if (URealityEditableComponent* Editable = Owner->FindComponentByClass<URealityEditableComponent>())
	{
		Editable->RestoreAllModificationsForRecovery();
	}
	Owner->SetActorTransform(BaselineTransform, false, nullptr, ETeleportType::TeleportPhysics);
	TInlineComponentArray<UPrimitiveComponent*> Primitives(Owner);
	for (UPrimitiveComponent* Primitive : Primitives)
	{
		if (Primitive && Primitive->IsSimulatingPhysics())
		{
			Primitive->SetPhysicsLinearVelocity(FVector::ZeroVector);
			Primitive->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
		}
	}
	++RecoveryCount;
#if !UE_BUILD_SHIPPING
	if (GEngine) GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0f, FColor::Cyan, TEXT("OBJECT RECOVERED"));
#endif
	UE_LOG(LogReality, Log, TEXT("Demo recovered critical prop '%s' to its authored baseline."), *GetNameSafe(Owner));
	return true;
}
