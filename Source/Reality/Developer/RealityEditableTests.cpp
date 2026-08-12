// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"

#include "Developer/RealityEditableComponent.h"
#include "GameFramework/Actor.h"
#include "Interaction/Interactable.h"
#include "Interaction/InteractionTestActor.h"
#include "RealitySystem/RealityCheatEvent.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRealityEditableComponentTest,
	"Reality.Developer.EditableComponent",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::EngineFilter)

bool FRealityEditableComponentTest::RunTest(const FString& Parameters)
{
	const FGameplayTag CollisionTag = FGameplayTag::RequestGameplayTag(TEXT("Cheat.Collision"));
	const FGameplayTag ScaleTag = FGameplayTag::RequestGameplayTag(TEXT("Cheat.Scale"));
	const FGameplayTag GravityTag = FGameplayTag::RequestGameplayTag(TEXT("Cheat.Gravity"));
	const FGameplayTag DoorTag = FGameplayTag::RequestGameplayTag(TEXT("Object.Door"));
	const FGameplayTag PropTag = FGameplayTag::RequestGameplayTag(TEXT("Object.Prop"));

	AActor* EditableActor = NewObject<AActor>();
	URealityEditableComponent* EditableComponent = NewObject<URealityEditableComponent>(EditableActor, TEXT("RealityEditableComponent"));
	EditableActor->AddInstanceComponent(EditableComponent);
	TestNotNull(TEXT("An editable component can exist on an actor"), EditableActor->FindComponentByClass<URealityEditableComponent>());
	TestTrue(TEXT("The editable component supports a local Low Gravity Tick"), EditableComponent->PrimaryComponentTick.bCanEverTick);
	TestFalse(TEXT("The Low Gravity Tick is disabled by default"), EditableComponent->IsComponentTickEnabled());

	TestFalse(TEXT("An empty capability container supports no valid cheat"), EditableComponent->SupportsCheat(CollisionTag));
	TestFalse(TEXT("An empty object container has no valid object tag"), EditableComponent->HasObjectTag(DoorTag));
	TestFalse(TEXT("An invalid Gameplay Tag is never a supported cheat"), EditableComponent->SupportsCheat(FGameplayTag()));
	TestFalse(TEXT("An invalid Gameplay Tag is never an object classification"), EditableComponent->HasObjectTag(FGameplayTag()));

	FGameplayTagContainer SupportedCheats;
	SupportedCheats.AddTag(CollisionTag);
	SupportedCheats.AddTag(ScaleTag);
	EditableComponent->SetSupportedCheats(SupportedCheats);

	FGameplayTagContainer ObjectTags;
	ObjectTags.AddTag(DoorTag);
	EditableComponent->SetObjectTags(ObjectTags);

	TestTrue(TEXT("A configured Collision capability is supported"), EditableComponent->SupportsCheat(CollisionTag));
	TestTrue(TEXT("A configured Scale capability is supported"), EditableComponent->SupportsCheat(ScaleTag));
	TestFalse(TEXT("An unconfigured Gravity capability is unsupported"), EditableComponent->SupportsCheat(GravityTag));
	TestTrue(TEXT("A configured Door object tag is present"), EditableComponent->HasObjectTag(DoorTag));
	TestFalse(TEXT("An unconfigured Prop object tag is absent"), EditableComponent->HasObjectTag(PropTag));
	FGameplayTagContainer ReturnedCheats = EditableComponent->GetSupportedCheats();
	ReturnedCheats.AddTag(GravityTag);
	TestFalse(TEXT("Mutating a returned cheat container does not alter component configuration"), EditableComponent->SupportsCheat(GravityTag));
	FGameplayTagContainer ReturnedObjectTags = EditableComponent->GetObjectTags();
	ReturnedObjectTags.AddTag(PropTag);
	TestFalse(TEXT("Mutating a returned object container does not alter component configuration"), EditableComponent->HasObjectTag(PropTag));
	TestTrue(TEXT("The debug description identifies the actor"), EditableComponent->GetEditableDebugDescription().Contains(EditableActor->GetName()));

	TestFalse(TEXT("An editable actor does not implicitly implement interaction"), EditableActor->Implements<UInteractable>());
	const AInteractionTestActor* InteractionActorCDO = AInteractionTestActor::StaticClass()->GetDefaultObject<AInteractionTestActor>();
	TestNull(TEXT("The interaction test actor has no required editable component"), InteractionActorCDO->FindComponentByClass<URealityEditableComponent>());

	AActor* InstigatingActor = NewObject<AActor>();
	const FRealityCheatEvent CheatEvent(EditableActor, CollisionTag, InstigatingActor, ERealityCheatOperation::Restore);
	TestEqual(TEXT("A cheat event retains its target"), CheatEvent.TargetActor.Get(), EditableActor);
	TestEqual(TEXT("A cheat event retains its instigator"), CheatEvent.InstigatingActor.Get(), InstigatingActor);
	TestEqual(TEXT("A cheat event retains its semantic cheat tag"), CheatEvent.CheatTag, CollisionTag);
	TestEqual(TEXT("A cheat event distinguishes Restore from Apply"), CheatEvent.Operation, ERealityCheatOperation::Restore);

	URealityEditableComponent* OwnerlessComponent = NewObject<URealityEditableComponent>();
	TestFalse(TEXT("An ownerless empty component fails capability queries safely"), OwnerlessComponent->SupportsCheat(CollisionTag));
	TestTrue(TEXT("An ownerless component can still produce safe debug text"), !OwnerlessComponent->GetEditableDebugDescription().IsEmpty());

	UClass* FirstPersonCharacterClass = LoadClass<AActor>(nullptr, TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter.BP_FirstPersonCharacter_C"));
	TestNotNull(TEXT("The existing First Person character Blueprint still loads"), FirstPersonCharacterClass);

	return true;
}

#endif
