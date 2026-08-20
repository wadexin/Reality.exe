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
		Label->SetText(FText::FromString(Text)); FSlateFontInfo Font = Label->GetFont(); Font.Size = SizeValue; Label->SetFont(Font);
		Label->SetColorAndOpacity(FSlateColor(FLinearColor(0.75f, 0.95f, 1.0f)));
		Body->AddChildToVerticalBox(Label)->SetPadding(FMargin(0, 0, 0, 14)); return Label;
	};
	auto AddButton = [this, Body](const FString& Text)
	{
		UButton* Button = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());
		UTextBlock* Label = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass()); Label->SetText(FText::FromString(Text));
		Button->SetContent(Label); Body->AddChildToVerticalBox(Button)->SetPadding(FMargin(0, 5)); return Button;
	};
	AddText(TEXT("SYSTEM"), 28);
	PromptText = AddText(TEXT("Session controls"), 14);
	ResumeButton = AddButton(TEXT("Resume")); ResumeButton->OnClicked.AddDynamic(this, &UDemoSystemMenuWidget::HandleResume);
	RestorePositionButton = AddButton(TEXT("Restore Position")); RestorePositionButton->OnClicked.AddDynamic(this, &UDemoSystemMenuWidget::HandleRestorePosition);
	RestartButton = AddButton(TEXT("Restart Demo")); RestartButton->OnClicked.AddDynamic(this, &UDemoSystemMenuWidget::HandleRestartRequest);
	ConfirmButton = AddButton(TEXT("Restart")); ConfirmButton->OnClicked.AddDynamic(this, &UDemoSystemMenuWidget::HandleRestartConfirm);
	CancelButton = AddButton(TEXT("Cancel")); CancelButton->OnClicked.AddDynamic(this, &UDemoSystemMenuWidget::HandleRestartCancel);
}

void UDemoSystemMenuWidget::SetRestartConfirmation(const bool bConfirming)
{
	bConfirmingRestart = bConfirming;
	if (PromptText) PromptText->SetText(bConfirming
		? FText::FromString(TEXT("Restart Demo?\nCurrent session progress will be reset."))
		: FText::FromString(TEXT("ESC  RESUME\nRestore Position keeps world changes.\nRestart Demo resets this session.")));
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
