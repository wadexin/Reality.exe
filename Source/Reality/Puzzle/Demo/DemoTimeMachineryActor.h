// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RealitySystem/RealityCheatEvent.h"
#include "DemoTimeMachineryActor.generated.h"

class URealityEditableComponent;
class USceneComponent;
class UPrimitiveComponent;
class UBoxComponent;
class UStaticMeshComponent;
class UTextRenderComponent;
class UAudioComponent;
class USoundBase;

/** Lightweight local-Tick rotor used as both a baseline reference and a Reality-editable final gate mechanism. */
UCLASS(Blueprintable)
class REALITY_API ADemoTimeMachineryActor : public AActor
{
	GENERATED_BODY()

public:
	ADemoTimeMachineryActor();
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> RotorMesh;

	/** Forgiving Visibility-only query shape for selecting the machine through the scanner. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> TargetingProxy;

	/** First visible light curtain; its matching overlap plane is active on the same cadence. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> PrimaryScannerPlane;

	/** Second visible light curtain, phase-offset from the first. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> SecondaryScannerPlane;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> PrimaryScannerDetection;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> SecondaryScannerDetection;

	/** Collision-free visual cadence marker carried by the authoritative rotating component. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> RotorTipPositive;

	/** Mirrored collision-free cadence marker carried by the authoritative rotating component. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> RotorTipNegative;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UTextRenderComponent> MachineryLabel;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<URealityEditableComponent> EditableComponent;

	/** Spatial motor bed whose pitch explicitly mirrors the authoritative local-Time preset. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UAudioComponent> MachineryAudio;

	UFUNCTION(BlueprintPure, Category = "Demo|Time|Audio")
	float GetPresentationAudioPitch() const;

	UFUNCTION(BlueprintPure, Category = "Demo|Time|Audio")
	int32 GetAudioStateUpdateCount() const { return AudioStateUpdateCount; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Demo|Time")
	float RotationDegreesPerSecond = 720.0f;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Demo|Time")
	float AccumulatedTickSeconds = 0.0f;

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Demo|Time")
	void ConfigureTimeMachinery(FText DisplayName, float InRotationDegreesPerSecond);

	UFUNCTION(BlueprintPure, Category = "Demo|Time|Scanner")
	bool IsPrimaryScannerActive() const { return bPrimaryScannerActive; }

	UFUNCTION(BlueprintPure, Category = "Demo|Time|Scanner")
	bool IsSecondaryScannerActive() const { return bSecondaryScannerActive; }

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UFUNCTION() void HandleRealityCheatEvent(const FRealityCheatEvent& CheatEvent);
	UFUNCTION() void HandleScannerOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	void RefreshTimeAudio();
	void RefreshScannerState();
	void SetScannerActive(UBoxComponent* Detection, UStaticMeshComponent* Visual, bool bActive);
	bool RejectPlayer(AActor* PlayerActor);

	UPROPERTY() TObjectPtr<USoundBase> ScannerViolationSound;
	FTimerHandle RejectionTimer;
	int32 AudioStateUpdateCount = 0;
	bool bPrimaryScannerActive = false;
	bool bSecondaryScannerActive = false;
	bool bRejectingPlayer = false;
};
