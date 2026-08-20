// Copyright Epic Games, Inc. All Rights Reserved.
#include "UI/DemoSystemMenuWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "InputCoreTypes.h"
#include "Puzzle/Demo/DemoLanguage.h"
#include "RealityPlayerController.h"

void UDemoSystemMenuWidget::SetOwningRealityController(ARealityPlayerController* Controller)
{
	RealityController = Controller;
}

bool UDemoSystemMenuWidget::Initialize()
{
	if (!Super::Initialize()) return false;
	SetIsFocusable(true);
	if (WidgetTree && !WidgetTree->RootWidget) BuildLayout();
	SetRestartConfirmation(false);
	return true;
}

void UDemoSystemMenuWidget::BuildLayout()
{
	UCanvasPanel* Root = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("SystemMenuRoot"));
	WidgetTree->RootWidget = Root;
	UBorder* Background = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("SystemMenuBackground"));
	Background->SetBrushColor(FLinearColor(0.01f, 0.02f, 0.025f, 0.96f));
	Background->SetPadding(FMargin(30.0f));
	UCanvasPanelSlot* PanelSlot = Root->AddChildToCanvas(Background);
	PanelSlot->SetAnchors(FAnchors(0.5f, 0.5f));
	PanelSlot->SetAlignment(FVector2D(0.5f, 0.5f));
	PanelSlot->SetAutoSize(true);
	USizeBox* Size = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass());
	Size->SetWidthOverride(420.0f);
	Background->SetContent(Size);
	UVerticalBox* Body = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass());
	Size->SetContent(Body);
	auto AddText = [this, Body](const FString& Text, const int32 SizeValue)
	{
		UTextBlock* Label = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
		Label->SetText(FText::FromString(Text)); FSlateFontInfo Font = Label->GetFont(); Font.Size = SizeValue;
		if (RealityDemoLanguage::IsSimplifiedChinese(this)) Font = RealityDemoLanguage::GetSimplifiedChineseFont(SizeValue);
		Label->SetFont(Font);
		Label->SetColorAndOpacity(FSlateColor(FLinearColor(0.75f, 0.95f, 1.0f)));
		Body->AddChildToVerticalBox(Label)->SetPadding(FMargin(0, 0, 0, 14)); return Label;
	};
	auto AddButton = [this, Body](const FString& Text)
	{
		UButton* Button = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());
		UTextBlock* Label = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass()); Label->SetText(FText::FromString(Text));
		if (RealityDemoLanguage::IsSimplifiedChinese(this)) Label->SetFont(RealityDemoLanguage::GetSimplifiedChineseFont(16));
		Button->SetContent(Label); Body->AddChildToVerticalBox(Button)->SetPadding(FMargin(0, 5)); return Button;
	};
	auto L = [this](const TCHAR* English, const TCHAR* Chinese) { return RealityDemoLanguage::String(this, English, Chinese); };
	AddText(L(TEXT("SYSTEM"), TEXT("系统")), 28);
	PromptText = AddText(L(TEXT("Session controls"), TEXT("Demo 控制")), 16);
	ResumeButton = AddButton(L(TEXT("Resume"), TEXT("继续游戏"))); ResumeButton->OnClicked.AddDynamic(this, &UDemoSystemMenuWidget::HandleResume);
	RestorePositionButton = AddButton(L(TEXT("Restore Position"), TEXT("恢复位置"))); RestorePositionButton->OnClicked.AddDynamic(this, &UDemoSystemMenuWidget::HandleRestorePosition);
	RestartButton = AddButton(L(TEXT("Restart Demo"), TEXT("重新开始 Demo"))); RestartButton->OnClicked.AddDynamic(this, &UDemoSystemMenuWidget::HandleRestartRequest);
	ConfirmButton = AddButton(L(TEXT("Restart"), TEXT("确认重新开始"))); ConfirmButton->OnClicked.AddDynamic(this, &UDemoSystemMenuWidget::HandleRestartConfirm);
	CancelButton = AddButton(L(TEXT("Cancel"), TEXT("取消"))); CancelButton->OnClicked.AddDynamic(this, &UDemoSystemMenuWidget::HandleRestartCancel);
}

void UDemoSystemMenuWidget::SetRestartConfirmation(const bool bConfirming)
{
	bConfirmingRestart = bConfirming;
	if (PromptText) PromptText->SetText(bConfirming
		? RealityDemoLanguage::Text(this, TEXT("Restart Demo?\nCurrent session progress will be reset."), TEXT("重新开始 Demo？\n当前进度将被重置。"))
		: RealityDemoLanguage::Text(this, TEXT("ESC  RESUME\nRestore Position keeps world changes.\nRestart Demo resets this session."), TEXT("ESC  返回游戏\n恢复位置会保留世界状态。\n重新开始 Demo 会重置本次流程。")));
	if (ResumeButton) ResumeButton->SetVisibility(bConfirming ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
	if (RestartButton) RestartButton->SetVisibility(bConfirming ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
	if (RestorePositionButton) RestorePositionButton->SetVisibility(bConfirming ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
	if (ConfirmButton) ConfirmButton->SetVisibility(bConfirming ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	if (CancelButton) CancelButton->SetVisibility(bConfirming ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}

FReply UDemoSystemMenuWidget::NativeOnKeyDown(const FGeometry& Geometry, const FKeyEvent& Event)
{
	if (Event.GetKey() == EKeys::Escape)
	{
		if (bConfirmingRestart) SetRestartConfirmation(false); else HandleResume();
		return FReply::Handled();
	}
	if (Event.GetKey() == EKeys::F6) return FReply::Handled();
	return Super::NativeOnKeyDown(Geometry, Event);
}

void UDemoSystemMenuWidget::HandleResume() { if (ARealityPlayerController* Controller = RealityController.Get()) Controller->HideSystemMenu(); }
void UDemoSystemMenuWidget::HandleRestartRequest() { SetRestartConfirmation(true); }
void UDemoSystemMenuWidget::HandleRestorePosition() { if (ARealityPlayerController* Controller = RealityController.Get()) Controller->RecoverPlayerPosition(); }
void UDemoSystemMenuWidget::HandleRestartConfirm() { if (ARealityPlayerController* Controller = RealityController.Get()) Controller->ConfirmRestartDemo(); }
void UDemoSystemMenuWidget::HandleRestartCancel() { SetRestartConfirmation(false); }
