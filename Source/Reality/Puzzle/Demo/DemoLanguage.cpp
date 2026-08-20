// Copyright Epic Games, Inc. All Rights Reserved.

#include "Puzzle/Demo/DemoLanguage.h"

#include "Blueprint/WidgetTree.h"
#include "Components/TextBlock.h"
#include "Components/TextRenderComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Styling/CoreStyle.h"
#include "TimerManager.h"

bool UDemoChineseTextWidget::Initialize()
{
	if (!Super::Initialize()) return false;
	if (WidgetTree && !WidgetTree->RootWidget)
	{
		Label = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("ChineseText"));
		Label->SetJustification(ETextJustify::Center);
		Label->SetShadowOffset(FVector2D(1.5f, 1.5f));
		Label->SetShadowColorAndOpacity(FLinearColor(0.0f, 0.0f, 0.0f, 0.9f));
		WidgetTree->RootWidget = Label;
	}
	return true;
}

void UDemoChineseTextWidget::Configure(const FText& Text, const int32 Size, const FLinearColor& Color)
{
	if (!Label) return;
	Label->SetText(Text);
	Label->SetFont(RealityDemoLanguage::GetSimplifiedChineseFont(Size));
	Label->SetColorAndOpacity(FSlateColor(Color));
}

bool RealityDemoLanguage::IsSimplifiedChinese(const UObject* WorldContext)
{
	const UWorld* World = WorldContext ? WorldContext->GetWorld() : nullptr;
	return World && World->GetMapName().Contains(TEXT("Lvl_Demo_Graybox_ZH"));
}

FText RealityDemoLanguage::Text(const UObject* WorldContext, const TCHAR* English, const TCHAR* SimplifiedChinese)
{
	return FText::FromString(IsSimplifiedChinese(WorldContext) ? SimplifiedChinese : English);
}

FString RealityDemoLanguage::String(const UObject* WorldContext, const TCHAR* English, const TCHAR* SimplifiedChinese)
{
	return IsSimplifiedChinese(WorldContext) ? SimplifiedChinese : English;
}

FSlateFontInfo RealityDemoLanguage::GetSimplifiedChineseFont(const int32 Size)
{
	return FCoreStyle::GetDefaultFontStyle(TEXT("Regular"), Size);
}

void RealityDemoLanguage::SetWorldText(UTextRenderComponent* Source, const UObject* WorldContext, const FText& English, const FText& Chinese)
{
	if (!Source) return;
	if (!IsSimplifiedChinese(WorldContext))
	{
		Source->SetText(English);
		return;
	}

	TArray<USceneComponent*> Children;
	Source->GetChildrenComponents(false, Children);
	for (USceneComponent* Child : Children)
	{
		if (UWidgetComponent* WidgetComponent = Cast<UWidgetComponent>(Child))
		{
			if (UDemoChineseTextWidget* Widget = Cast<UDemoChineseTextWidget>(WidgetComponent->GetWidget()))
			{
				Widget->Configure(Chinese, FMath::RoundToInt(Source->WorldSize), FLinearColor(Source->TextRenderColor));
				Source->SetText(FText::GetEmpty());
				return;
			}
		}
	}
	Source->SetText(Chinese);
}

void RealityDemoLanguage::ShowChineseNotice(APlayerController* Controller, const FText& Text, const float Duration)
{
	if (!Controller || !IsSimplifiedChinese(Controller)) return;
	UDemoChineseTextWidget* Notice = CreateWidget<UDemoChineseTextWidget>(Controller, UDemoChineseTextWidget::StaticClass());
	if (!Notice) return;
	Notice->Configure(Text, 30, FLinearColor(0.75f, 0.95f, 1.0f));
	Notice->AddToViewport(200);
	int32 Width = 0, Height = 0;
	Controller->GetViewportSize(Width, Height);
	Notice->SetAlignmentInViewport(FVector2D(0.5f, 0.5f));
	Notice->SetPositionInViewport(FVector2D(Width * 0.5f, Height * 0.32f), false);
	TWeakObjectPtr<UDemoChineseTextWidget> WeakNotice(Notice);
	Controller->GetWorldTimerManager().SetTimerForNextTick(FTimerDelegate::CreateWeakLambda(Controller, [Controller, WeakNotice, Duration]()
	{
		FTimerHandle Handle;
		Controller->GetWorldTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([WeakNotice]()
		{
			if (UDemoChineseTextWidget* Widget = WeakNotice.Get()) Widget->RemoveFromParent();
		}), Duration, false);
	}));
}
