// Copyright Epic Games, Inc. All Rights Reserved.

#include "Puzzle/Demo/DemoChinesePresentationActor.h"

#include "Components/TextRenderComponent.h"
#include "Components/WidgetComponent.h"
#include "Developer/RealityEditableComponent.h"
#include "EngineUtils.h"
#include "Puzzle/Demo/DemoLanguage.h"
#include "TimerManager.h"

ADemoChinesePresentationActor::ADemoChinesePresentationActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ADemoChinesePresentationActor::BeginPlay()
{
	Super::BeginPlay();
	if (!RealityDemoLanguage::IsSimplifiedChinese(this)) return;
	GetWorldTimerManager().SetTimerForNextTick(this, &ADemoChinesePresentationActor::ApplyPresentation);
}

void ADemoChinesePresentationActor::ApplyPresentation()
{

	const TMap<FString, FString> WorldText = {
		{TEXT("OBJECTIVE: REACH SECURITY CORE"), TEXT("目标：前往安全核心")},
		{TEXT("SELECT MARKED TARGET\nF6 DEVELOPER CONSOLE"), TEXT("F6 开发者控制台\n选择标记目标")},
		{TEXT("ESC  SYSTEM\nRESTORE POSITION\nRESTART DEMO"), TEXT("ESC 系统\n恢复位置\n重新开始 Demo")},
		{TEXT("RLT MANAGED FACILITY\nF6 DEVELOPER ACCESS"), TEXT("RLT 管理设施\nF6 开发者权限")},
		{TEXT("RLT\nDEVELOPER ACCESS\nFACILITY MAINTENANCE [F6]"), TEXT("RLT\n开发者权限\n设施维护 [F6]")},
		{TEXT("AIRLOCK SEAL\nRESTORE AFTER CROSSING"), TEXT("气闸封锁\n通过后恢复")},
		{TEXT("OBJECTIVE: CROSS THE STORAGE BULKHEAD"), TEXT("目标：穿过仓储隔断")},
		{TEXT("CAMERA-OBSERVED SECURITY DOOR"), TEXT("摄像头监控的安全门")},
		{TEXT("MAINTENANCE DIAGNOSTICS ->"), TEXT("维修诊断 →")},
		{TEXT("MAINTENANCE LOOP\nE: RUN SENSOR DIAGNOSTIC"), TEXT("维修诊断回路\nE：运行传感器诊断")},
		{TEXT("FAILURE ACTIVE?\nRETURN TO THE OBSERVED DOOR"), TEXT("故障已激活？\n返回监控门")},
		{TEXT("OBJECTIVE: REACH THE UPPER TRANSFER DECK"), TEXT("目标：到达上层转运平台")},
		{TEXT("EMERGENCY RECOVERY ROUTE"), TEXT("紧急恢复路线")},
		{TEXT("OBJECTIVE: CROSS THE ACTIVE SCANNER"), TEXT("目标：穿过运行中的扫描器")},
		{TEXT("COMPARE LOCAL MACHINE CADENCE"), TEXT("比较机器的局部运行节奏")},
		{TEXT("HIGH-RISK SERVICE SHORTCUT"), TEXT("高风险维修捷径")},
		{TEXT("REALITY CORE EXIT\nE: COMPLETE DEMO"), TEXT("现实核心出口\nE：完成 Demo")},
		{TEXT("REFERENCE ROTOR\nBASELINE: 1.0x"), TEXT("参考转子\n基准：1.0x")},
		{TEXT("WITNESS\nFACING ->"), TEXT("目击者\n朝向 →")},
		{TEXT("HUMAN WITNESS\nFACING ->"), TEXT("人类目击者\n朝向 →")},
		{TEXT("SECURITY CAMERA\nFACING ->"), TEXT("安保摄像头\n朝向 →")},
		{TEXT("REALITY EDITABLE"), TEXT("可编辑现实属性")}
	};

	const TMap<FString, FString> TargetNames = {
		{TEXT("Main Opening Block"), TEXT("主通道障碍")},
		{TEXT("Service Step"), TEXT("维修踏台")},
		{TEXT("Security Door"), TEXT("安全门")},
		{TEXT("Security Rotor"), TEXT("安全扫描转子")},
		{TEXT("Transfer Crate"), TEXT("转运箱")},
		{TEXT("Physics Test Prop"), TEXT("物理测试物体")}
	};

	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		TInlineComponentArray<UTextRenderComponent*> TextComponents(*It);
		for (UTextRenderComponent* Component : TextComponents)
		{
			if (!Component) continue;
			const FString SourceText = Component->Text.ToString();
			FString DisplayText = SourceText;
			if (const FString* Translation = WorldText.Find(SourceText))
			{
				DisplayText = *Translation;
			}
			else if (SourceText.StartsWith(TEXT("REALITY EDITABLE\n")))
			{
				const FString EnglishName = SourceText.RightChop(17);
				const FString* ChineseName = TargetNames.Find(EnglishName);
				DisplayText = FString::Printf(TEXT("可编辑现实属性\n%s"), ChineseName ? **ChineseName : *EnglishName);
			}
			else if (SourceText.Contains(TEXT("LOCAL-TIME SCANNER DRIVE")))
			{
				const FString EnglishName = SourceText.LeftChop(SourceText.Len() - SourceText.Find(TEXT("\n")));
				const FString* ChineseName = TargetNames.Find(EnglishName);
				DisplayText = FString::Printf(TEXT("%s\n局部时间扫描驱动器\n降低扫描速度以延长通行窗口"), ChineseName ? **ChineseName : *EnglishName);
			}

			UWidgetComponent* WorldWidget = NewObject<UWidgetComponent>(*It, MakeUniqueObjectName(*It, UWidgetComponent::StaticClass(), TEXT("RLTChineseWorldText")));
			It->AddInstanceComponent(WorldWidget);
			WorldWidget->SetupAttachment(Component);
			WorldWidget->SetRelativeTransform(FTransform::Identity);
			WorldWidget->SetWidgetSpace(EWidgetSpace::World);
			WorldWidget->SetDrawAtDesiredSize(true);
			WorldWidget->SetTwoSided(false);
			WorldWidget->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			WorldWidget->SetGenerateOverlapEvents(false);
			WorldWidget->RegisterComponent();
			UDemoChineseTextWidget* Widget = CreateWidget<UDemoChineseTextWidget>(GetWorld(), UDemoChineseTextWidget::StaticClass());
			if (Widget)
			{
				Widget->Configure(FText::FromString(DisplayText), FMath::RoundToInt(Component->WorldSize), FLinearColor(Component->TextRenderColor));
				WorldWidget->SetWidget(Widget);
				Component->SetText(FText::GetEmpty());
			}
		}

		if (URealityEditableComponent* Editable = It->FindComponentByClass<URealityEditableComponent>())
		{
			if (const FString* Translation = TargetNames.Find(Editable->GetPlayerFacingName().ToString()))
			{
				Editable->SetPlayerFacingName(FText::FromString(*Translation));
			}
		}
	}
}
