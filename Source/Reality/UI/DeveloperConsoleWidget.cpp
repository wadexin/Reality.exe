// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/DeveloperConsoleWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/ScrollBox.h"
#include "Components/SizeBox.h"
#include "Components/Spacer.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Developer/DeveloperModeComponent.h"
#include "Engine/World.h"
#include "GameplayTagContainer.h"
#include "InputCoreTypes.h"
#include "RealitySystem/RealityManagerSubsystem.h"

namespace DeveloperConsoleStyle
{
	const FLinearColor Accent(0.1f, 0.85f, 1.0f, 1.0f);
	const FLinearColor PrimaryText(0.92f, 0.96f, 1.0f, 1.0f);
	const FLinearColor SecondaryText(0.62f, 0.72f, 0.78f, 1.0f);
}

bool UDeveloperConsoleWidget::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}

	SetIsFocusable(true);
	if (WidgetTree && !WidgetTree->RootWidget)
	{
		BuildPrototypeLayout();
	}
	RefreshConsole();
	return true;
}

void UDeveloperConsoleWidget::NativeDestruct()
{
	UnbindDeveloperModeComponent();
	Super::NativeDestruct();
}

void UDeveloperConsoleWidget::SetDeveloperModeComponent(UDeveloperModeComponent* NewDeveloperModeComponent)
{
	if (DeveloperModeComponent.Get() == NewDeveloperModeComponent)
	{
		RefreshConsole();
		return;
	}

	UnbindDeveloperModeComponent();
	DeveloperModeComponent = NewDeveloperModeComponent;
	if (IsValid(NewDeveloperModeComponent))
	{
		NewDeveloperModeComponent->OnDeveloperConsoleRefresh.AddUniqueDynamic(this, &UDeveloperConsoleWidget::HandleRefreshRequested);
	}
	RefreshConsole();
}

void UDeveloperConsoleWidget::UnbindDeveloperModeComponent()
{
	if (UDeveloperModeComponent* Component = DeveloperModeComponent.Get())
	{
		Component->OnDeveloperConsoleRefresh.RemoveDynamic(this, &UDeveloperConsoleWidget::HandleRefreshRequested);
	}
	DeveloperModeComponent.Reset();
}

FReply UDeveloperConsoleWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::F6)
	{
		if (UDeveloperModeComponent* Component = DeveloperModeComponent.Get())
		{
			Component->ExitDeveloperMode();
			return FReply::Handled();
		}
	}
	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UDeveloperConsoleWidget::BuildPrototypeLayout()
{
	UCanvasPanel* Root = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("DeveloperConsoleRoot"));
	WidgetTree->RootWidget = Root;

	USizeBox* PanelSize = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("ConsolePanelSize"));
	PanelSize->SetWidthOverride(520.0f);
	PanelSize->SetMaxDesiredHeight(850.0f);
	UCanvasPanelSlot* PanelSlot = Root->AddChildToCanvas(PanelSize);
	PanelSlot->SetAnchors(FAnchors(0.0f, 0.5f));
	PanelSlot->SetAlignment(FVector2D(0.0f, 0.5f));
	PanelSlot->SetPosition(FVector2D(32.0f, 0.0f));
	PanelSlot->SetAutoSize(true);

	UBorder* Background = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("ConsoleBackground"));
	Background->SetBrushColor(FLinearColor(0.015f, 0.025f, 0.035f, 0.94f));
	Background->SetPadding(FMargin(22.0f));
	PanelSize->SetContent(Background);

	UScrollBox* Scroll = WidgetTree->ConstructWidget<UScrollBox>(UScrollBox::StaticClass(), TEXT("ConsoleScroll"));
	Background->SetContent(Scroll);
	UVerticalBox* Body = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("ConsoleBody"));
	Scroll->AddChild(Body);

	AddLabel(Body, TEXT("DEVELOPER CONSOLE"), 28, DeveloperConsoleStyle::Accent);
	AddLabel(Body, TEXT("F6  CLOSE"), 12, DeveloperConsoleStyle::SecondaryText);
	TargetText = AddLabel(Body, TEXT("TARGET\nNo editable object selected"), 20, DeveloperConsoleStyle::PrimaryText);
	TargetDetailsText = AddLabel(Body, TEXT(""), 14, DeveloperConsoleStyle::SecondaryText);

	CollisionSection = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("CollisionSection"));
	Body->AddChildToVerticalBox(CollisionSection)->SetPadding(FMargin(0.0f, 16.0f, 0.0f, 0.0f));
	AddLabel(CollisionSection, TEXT("COLLISION"), 18, DeveloperConsoleStyle::Accent);
	CollisionStateText = AddLabel(CollisionSection, TEXT("State: Normal"), 14, DeveloperConsoleStyle::PrimaryText);
	CollisionButton = AddButton(CollisionSection, TEXT("Disable Collision"));
	CollisionButtonText = Cast<UTextBlock>(CollisionButton->GetChildAt(0));
	CollisionButton->OnClicked.AddDynamic(this, &UDeveloperConsoleWidget::HandleCollisionClicked);

	ScaleSection = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("ScaleSection"));
	Body->AddChildToVerticalBox(ScaleSection)->SetPadding(FMargin(0.0f, 16.0f, 0.0f, 0.0f));
	AddLabel(ScaleSection, TEXT("SCALE"), 18, DeveloperConsoleStyle::Accent);
	ScaleStateText = AddLabel(ScaleSection, TEXT("Current: 1.0x"), 14, DeveloperConsoleStyle::PrimaryText);
	UButton* ScaleQuarter = AddButton(ScaleSection, TEXT("0.25x")); ScaleQuarter->OnClicked.AddDynamic(this, &UDeveloperConsoleWidget::HandleScaleQuarterClicked);
	UButton* ScaleHalf = AddButton(ScaleSection, TEXT("0.5x")); ScaleHalf->OnClicked.AddDynamic(this, &UDeveloperConsoleWidget::HandleScaleHalfClicked);
	UButton* ScaleOne = AddButton(ScaleSection, TEXT("1.0x")); ScaleOne->OnClicked.AddDynamic(this, &UDeveloperConsoleWidget::HandleScaleOneClicked);
	UButton* ScaleDouble = AddButton(ScaleSection, TEXT("2.0x")); ScaleDouble->OnClicked.AddDynamic(this, &UDeveloperConsoleWidget::HandleScaleDoubleClicked);
	UButton* ScaleQuadruple = AddButton(ScaleSection, TEXT("4.0x")); ScaleQuadruple->OnClicked.AddDynamic(this, &UDeveloperConsoleWidget::HandleScaleQuadrupleClicked);
	ScaleRestoreButton = AddButton(ScaleSection, TEXT("Restore Scale")); ScaleRestoreButton->OnClicked.AddDynamic(this, &UDeveloperConsoleWidget::HandleScaleRestoreClicked);

	GravitySection = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("GravitySection"));
	Body->AddChildToVerticalBox(GravitySection)->SetPadding(FMargin(0.0f, 16.0f, 0.0f, 0.0f));
	AddLabel(GravitySection, TEXT("GRAVITY"), 18, DeveloperConsoleStyle::Accent);
	GravityStateText = AddLabel(GravitySection, TEXT("Current: Normal"), 14, DeveloperConsoleStyle::PrimaryText);
	UButton* GravityNormal = AddButton(GravitySection, TEXT("Normal")); GravityNormal->OnClicked.AddDynamic(this, &UDeveloperConsoleWidget::HandleGravityNormalClicked);
	UButton* GravityLow = AddButton(GravitySection, TEXT("Low")); GravityLow->OnClicked.AddDynamic(this, &UDeveloperConsoleWidget::HandleGravityLowClicked);
	UButton* GravityZero = AddButton(GravitySection, TEXT("Zero")); GravityZero->OnClicked.AddDynamic(this, &UDeveloperConsoleWidget::HandleGravityZeroClicked);
	GravityRestoreButton = AddButton(GravitySection, TEXT("Restore Gravity")); GravityRestoreButton->OnClicked.AddDynamic(this, &UDeveloperConsoleWidget::HandleGravityRestoreClicked);

	MassSection = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("MassSection"));
	Body->AddChildToVerticalBox(MassSection)->SetPadding(FMargin(0.0f, 16.0f, 0.0f, 0.0f));
	AddLabel(MassSection, TEXT("MASS"), 18, DeveloperConsoleStyle::Accent);
	MassStateText = AddLabel(MassSection, TEXT("Modified: No\nPreset: 1.0x"), 14, DeveloperConsoleStyle::PrimaryText);
	UButton* MassQuarter = AddButton(MassSection, TEXT("0.25x")); MassQuarter->OnClicked.AddDynamic(this, &UDeveloperConsoleWidget::HandleMassQuarterClicked);
	UButton* MassHalf = AddButton(MassSection, TEXT("0.5x")); MassHalf->OnClicked.AddDynamic(this, &UDeveloperConsoleWidget::HandleMassHalfClicked);
	UButton* MassOne = AddButton(MassSection, TEXT("1.0x")); MassOne->OnClicked.AddDynamic(this, &UDeveloperConsoleWidget::HandleMassOneClicked);
	UButton* MassDouble = AddButton(MassSection, TEXT("2.0x")); MassDouble->OnClicked.AddDynamic(this, &UDeveloperConsoleWidget::HandleMassDoubleClicked);
	UButton* MassQuadruple = AddButton(MassSection, TEXT("4.0x")); MassQuadruple->OnClicked.AddDynamic(this, &UDeveloperConsoleWidget::HandleMassQuadrupleClicked);
	MassRestoreButton = AddButton(MassSection, TEXT("Restore Mass")); MassRestoreButton->OnClicked.AddDynamic(this, &UDeveloperConsoleWidget::HandleMassRestoreClicked);

	RealityText = AddLabel(Body, TEXT("REALITY\nSuspicion: 0 / 100\nState: Stable"), 14, DeveloperConsoleStyle::PrimaryText);
	if (UVerticalBoxSlot* RealitySlot = Cast<UVerticalBoxSlot>(RealityText->Slot))
	{
		RealitySlot->SetPadding(FMargin(0.0f, 18.0f, 0.0f, 0.0f));
	}
}

UTextBlock* UDeveloperConsoleWidget::AddLabel(UVerticalBox* Parent, const FString& Text, const int32 Size, const FLinearColor& Color)
{
	UTextBlock* Label = WidgetTree->ConstructWidget<UTextBlock>();
	Label->SetText(FText::FromString(Text));
	Label->SetColorAndOpacity(FSlateColor(Color));
	FSlateFontInfo Font = Label->GetFont();
	Font.Size = Size;
	Label->SetFont(Font);
	Label->SetAutoWrapText(true);
	Parent->AddChildToVerticalBox(Label)->SetPadding(FMargin(0.0f, 3.0f));
	return Label;
}

UButton* UDeveloperConsoleWidget::AddButton(UVerticalBox* Parent, const FString& Text)
{
	UButton* Button = WidgetTree->ConstructWidget<UButton>();
	Button->SetBackgroundColor(FLinearColor(0.07f, 0.16f, 0.2f, 1.0f));
	UTextBlock* Label = WidgetTree->ConstructWidget<UTextBlock>();
	Label->SetText(FText::FromString(Text));
	Label->SetColorAndOpacity(FSlateColor(DeveloperConsoleStyle::PrimaryText));
	Label->SetJustification(ETextJustify::Center);
	Button->AddChild(Label);
	Parent->AddChildToVerticalBox(Button)->SetPadding(FMargin(0.0f, 3.0f));
	return Button;
}

void UDeveloperConsoleWidget::RefreshConsole()
{
	if (!TargetText || !RealityText)
	{
		return;
	}

	UDeveloperModeComponent* DeveloperComponent = DeveloperModeComponent.Get();
	URealityEditableComponent* Editable = IsValid(DeveloperComponent) ? DeveloperComponent->GetFocusedEditableComponent() : nullptr;
	const FGameplayTag CollisionTag = FGameplayTag::RequestGameplayTag(TEXT("Cheat.Collision"));
	const FGameplayTag ScaleTag = FGameplayTag::RequestGameplayTag(TEXT("Cheat.Scale"));
	const FGameplayTag GravityTag = FGameplayTag::RequestGameplayTag(TEXT("Cheat.Gravity"));
	const FGameplayTag MassTag = FGameplayTag::RequestGameplayTag(TEXT("Cheat.Mass"));

	if (IsValid(Editable) && IsValid(Editable->GetOwner()))
	{
		TargetText->SetText(FText::FromString(FString::Printf(TEXT("TARGET\n%s"), *GetNameSafe(Editable->GetOwner()))));
		TargetDetailsText->SetText(FText::FromString(FString::Printf(TEXT("Object Tags: %s\nSupported: %s"),
			*Editable->GetObjectTags().ToStringSimple(), *Editable->GetSupportedCheats().ToStringSimple())));
	}
	else
	{
		TargetText->SetText(FText::FromString(TEXT("TARGET\nNo editable object selected")));
		TargetDetailsText->SetText(FText::GetEmpty());
	}

	const bool bCollisionSupported = IsValid(Editable) && Editable->SupportsCheat(CollisionTag);
	const bool bScaleSupported = IsValid(Editable) && Editable->SupportsCheat(ScaleTag);
	const bool bGravitySupported = IsValid(Editable) && Editable->SupportsCheat(GravityTag);
	const bool bMassSupported = IsValid(Editable) && Editable->SupportsCheat(MassTag);
	CollisionSection->SetVisibility(bCollisionSupported ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	ScaleSection->SetVisibility(bScaleSupported ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	GravitySection->SetVisibility(bGravitySupported ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	MassSection->SetVisibility(bMassSupported ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);

	if (bCollisionSupported)
	{
		const bool bModified = Editable->IsCollisionModified();
		CollisionStateText->SetText(FText::FromString(bModified ? TEXT("State: Modified") : TEXT("State: Normal")));
		CollisionButtonText->SetText(FText::FromString(bModified ? TEXT("Restore Collision") : TEXT("Disable Collision")));
	}
	if (bScaleSupported)
	{
		const UEnum* PresetEnum = StaticEnum<ERealityScalePreset>();
		ScaleStateText->SetText(FText::FromString(FString::Printf(TEXT("Modified: %s\nCurrent: %s\nActor Scale: %s"),
			Editable->IsScaleModified() ? TEXT("Yes") : TEXT("No"),
			PresetEnum ? *PresetEnum->GetDisplayNameTextByValue(static_cast<int64>(Editable->GetCurrentScalePreset())).ToString() : TEXT("Unknown"),
			*Editable->GetOwner()->GetActorScale3D().ToCompactString())));
		ScaleRestoreButton->SetVisibility(Editable->IsScaleModified() ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
	if (bGravitySupported)
	{
		const UEnum* PresetEnum = StaticEnum<ERealityGravityPreset>();
		GravityStateText->SetText(FText::FromString(FString::Printf(TEXT("Modified: %s\nCurrent: %s\nPhysics Components: %d"),
			Editable->IsGravityModified() ? TEXT("Yes") : TEXT("No"),
			PresetEnum ? *PresetEnum->GetDisplayNameTextByValue(static_cast<int64>(Editable->GetCurrentGravityPreset())).ToString() : TEXT("Unknown"),
			Editable->GetEligibleGravityComponentCount())));
		GravityRestoreButton->SetVisibility(Editable->IsGravityModified() ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
	if (bMassSupported)
	{
		const UEnum* PresetEnum = StaticEnum<ERealityMassPreset>();
		MassStateText->SetText(FText::FromString(FString::Printf(TEXT("Modified: %s\nPreset: %s\nBodies: %d\nBaseline Mass: %.2f kg\nCurrent Mass: %.2f kg"),
			Editable->IsMassModified() ? TEXT("Yes") : TEXT("No"),
			PresetEnum ? *PresetEnum->GetDisplayNameTextByValue(static_cast<int64>(Editable->GetCurrentMassPreset())).ToString() : TEXT("Unknown"),
			Editable->GetEligibleMassComponentCount(), Editable->GetBaselineEffectiveMassKg(), Editable->GetCurrentEffectiveMassKg())));
		MassRestoreButton->SetVisibility(Editable->IsMassModified() ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}

	FString RealityDisplay = TEXT("REALITY\nSuspicion: 0 / 100\nState: Stable");
	if (const UWorld* World = GetWorld())
	{
		if (const URealityManagerSubsystem* Manager = World->GetSubsystem<URealityManagerSubsystem>())
		{
			const UEnum* StateEnum = StaticEnum<ERealityState>();
			RealityDisplay = FString::Printf(TEXT("REALITY\nSuspicion: %.0f / 100\nState: %s"), Manager->GetSuspicion(),
				StateEnum ? *StateEnum->GetDisplayNameTextByValue(static_cast<int64>(Manager->GetRealityState())).ToString() : TEXT("Unknown"));
			if (const FRealityProcessedCheatRecord* Event = Manager->GetMostRecentEvent())
			{
				RealityDisplay += FString::Printf(TEXT("\n\nLast Event: %s %s %+.0f\nBase: %+.0f  Witness: %+.0f  Context: -%.0f\nObserved By: %d  Matched Contexts: %d"),
					*Event->CheatTag.ToString(), Event->Operation == ERealityCheatOperation::Apply ? TEXT("Apply") : TEXT("Restore"),
					Event->SuspicionDelta, Event->BaseSuspicionDelta, Event->WitnessSuspicionDelta, Event->ContextSuspicionReduction,
					Event->ObservingWitnessCount, Event->MatchedContextCount);
			}
		}
	}
	RealityText->SetText(FText::FromString(RealityDisplay));
}

bool UDeveloperConsoleWidget::HasEditableTarget() const
{
	const UDeveloperModeComponent* Component = DeveloperModeComponent.Get();
	return IsValid(Component) && IsValid(Component->GetFocusedEditableComponent());
}

bool UDeveloperConsoleWidget::IsCheatSectionAvailable(const FGameplayTag CheatTag) const
{
	const UDeveloperModeComponent* Component = DeveloperModeComponent.Get();
	const URealityEditableComponent* Editable = IsValid(Component) ? Component->GetFocusedEditableComponent() : nullptr;
	return IsValid(Editable) && CheatTag.IsValid() && Editable->SupportsCheat(CheatTag);
}

bool UDeveloperConsoleWidget::ExecuteCollisionToggle()
{
	UDeveloperModeComponent* Component = DeveloperModeComponent.Get();
	return IsValid(Component) && Component->ToggleFocusedCollisionModification();
}

bool UDeveloperConsoleWidget::ExecuteScalePreset(const ERealityScalePreset Preset)
{
	UDeveloperModeComponent* Component = DeveloperModeComponent.Get();
	return IsValid(Component) && Component->ApplyFocusedScaleModification(Preset);
}

bool UDeveloperConsoleWidget::ExecuteScaleRestore()
{
	UDeveloperModeComponent* Component = DeveloperModeComponent.Get();
	return IsValid(Component) && Component->RestoreFocusedScaleModification();
}

bool UDeveloperConsoleWidget::ExecuteGravityPreset(const ERealityGravityPreset Preset)
{
	UDeveloperModeComponent* Component = DeveloperModeComponent.Get();
	return IsValid(Component) && Component->ApplyFocusedGravityModification(Preset);
}

bool UDeveloperConsoleWidget::ExecuteGravityRestore()
{
	UDeveloperModeComponent* Component = DeveloperModeComponent.Get();
	return IsValid(Component) && Component->RestoreFocusedGravityModification();
}

bool UDeveloperConsoleWidget::ExecuteMassPreset(const ERealityMassPreset Preset)
{
	UDeveloperModeComponent* Component = DeveloperModeComponent.Get();
	return IsValid(Component) && Component->ApplyFocusedMassModification(Preset);
}

bool UDeveloperConsoleWidget::ExecuteMassRestore()
{
	UDeveloperModeComponent* Component = DeveloperModeComponent.Get();
	return IsValid(Component) && Component->RestoreFocusedMassModification();
}

void UDeveloperConsoleWidget::HandleRefreshRequested() { RefreshConsole(); }
void UDeveloperConsoleWidget::HandleCollisionClicked() { ExecuteCollisionToggle(); }
void UDeveloperConsoleWidget::HandleScaleQuarterClicked() { ExecuteScalePreset(ERealityScalePreset::Quarter); }
void UDeveloperConsoleWidget::HandleScaleHalfClicked() { ExecuteScalePreset(ERealityScalePreset::Half); }
void UDeveloperConsoleWidget::HandleScaleOneClicked() { ExecuteScalePreset(ERealityScalePreset::One); }
void UDeveloperConsoleWidget::HandleScaleDoubleClicked() { ExecuteScalePreset(ERealityScalePreset::Double); }
void UDeveloperConsoleWidget::HandleScaleQuadrupleClicked() { ExecuteScalePreset(ERealityScalePreset::Quadruple); }
void UDeveloperConsoleWidget::HandleScaleRestoreClicked() { ExecuteScaleRestore(); }
void UDeveloperConsoleWidget::HandleGravityNormalClicked() { ExecuteGravityPreset(ERealityGravityPreset::Normal); }
void UDeveloperConsoleWidget::HandleGravityLowClicked() { ExecuteGravityPreset(ERealityGravityPreset::Low); }
void UDeveloperConsoleWidget::HandleGravityZeroClicked() { ExecuteGravityPreset(ERealityGravityPreset::Zero); }
void UDeveloperConsoleWidget::HandleGravityRestoreClicked() { ExecuteGravityRestore(); }
void UDeveloperConsoleWidget::HandleMassQuarterClicked() { ExecuteMassPreset(ERealityMassPreset::Quarter); }
void UDeveloperConsoleWidget::HandleMassHalfClicked() { ExecuteMassPreset(ERealityMassPreset::Half); }
void UDeveloperConsoleWidget::HandleMassOneClicked() { ExecuteMassPreset(ERealityMassPreset::One); }
void UDeveloperConsoleWidget::HandleMassDoubleClicked() { ExecuteMassPreset(ERealityMassPreset::Double); }
void UDeveloperConsoleWidget::HandleMassQuadrupleClicked() { ExecuteMassPreset(ERealityMassPreset::Quadruple); }
void UDeveloperConsoleWidget::HandleMassRestoreClicked() { ExecuteMassRestore(); }
