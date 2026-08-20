// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "CoreMinimal.h"
#include "Fonts/SlateFontInfo.h"
#include "DemoLanguage.generated.h"

class APlayerController;
class UTextBlock;
class UTextRenderComponent;

/** Minimal text-only widget used for packaged Chinese world signs and security notices. */
UCLASS(NotBlueprintable)
class REALITY_API UDemoChineseTextWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual bool Initialize() override;
	void Configure(const FText& Text, int32 Size, const FLinearColor& Color = FLinearColor::White);

private:
	UPROPERTY(Transient) TObjectPtr<UTextBlock> Label;
};

/** Small, Demo-only language switch used by the dedicated Simplified Chinese map candidate. */
namespace RealityDemoLanguage
{
	/** Returns true only while the dedicated Lvl_Demo_Graybox_ZH map is running. */
	REALITY_API bool IsSimplifiedChinese(const UObject* WorldContext);

	/** Selects concise player-facing copy without introducing a project-wide localization pipeline. */
	REALITY_API FText Text(const UObject* WorldContext, const TCHAR* English, const TCHAR* SimplifiedChinese);

	/** FString counterpart for dynamically formatted prototype UI. */
	REALITY_API FString String(const UObject* WorldContext, const TCHAR* English, const TCHAR* SimplifiedChinese);

	/** Explicit packaged CJK font used consistently by Chinese Demo UMG text. */
	REALITY_API FSlateFontInfo GetSimplifiedChineseFont(int32 Size);

	/** Updates a translated world widget after its original TextRender source changes. */
	REALITY_API void SetWorldText(UTextRenderComponent* Source, const UObject* WorldContext, const FText& English, const FText& Chinese);

	/** Shows a short Chinese screen-space notice with the same packaged CJK font. */
	REALITY_API void ShowChineseNotice(APlayerController* Controller, const FText& Text, float Duration);
}
