// Copyright Epic Games, Inc. All Rights Reserved.
#if WITH_DEV_AUTOMATION_TESTS

#include "Audio/DemoMusicComponent.h"
#include "Misc/AutomationTest.h"
#include "Sound/SoundBase.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FDemoMusicContractTest,
	"Reality.Audio.DemoMusic",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FDemoMusicContractTest::RunTest(const FString& Parameters)
{
	UDemoMusicComponent* Music = NewObject<UDemoMusicComponent>();
	TestNotNull(TEXT("Demo music component can be created"), Music);
	TestNotNull(TEXT("Behind the Parameters is assigned"), Music->Sound.Get());
	TestTrue(TEXT("Full track does not hard-loop"), Music->Sound && !Music->Sound->IsLooping());
	TestFalse(TEXT("Music is non-spatialized"), Music->bAllowSpatialization);
	TestTrue(TEXT("Music continues while the system menu pauses gameplay"), Music->bIsUISound);
	TestEqual(TEXT("Music uses conservative gain"), Music->MusicVolume, 0.30f);
	TestEqual(TEXT("Replay uses a quiet interval"), Music->ReplayDelaySeconds, 3.0f);
	TestEqual(TEXT("Replay fades in gently"), Music->FadeInSeconds, 2.0f);
	TestFalse(TEXT("Music does not require Tick"), Music->PrimaryComponentTick.bCanEverTick);
	return true;
}

#endif
