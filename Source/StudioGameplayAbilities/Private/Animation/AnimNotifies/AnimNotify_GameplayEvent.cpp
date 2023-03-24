// Copyright 2022 CQUnreal. All Rights Reserved.

#include "Animation/AnimNotifies/AnimNotify_GameplayEvent.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"


UAnimNotify_GameplayEvent::UAnimNotify_GameplayEvent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

#if ENGINE_MAJOR_VERSION > 4
void UAnimNotify_GameplayEvent::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
#else
void UAnimNotify_GameplayEvent::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
#endif

	AActor* Actor = MeshComp ? MeshComp->GetOwner() : nullptr;
	if (IsValid(Actor))
	{
		IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(Actor);
		UAbilitySystemComponent* AbilitySystemComponent = AbilitySystemInterface ? AbilitySystemInterface->GetAbilitySystemComponent() : nullptr;
		if (IsValid(AbilitySystemComponent))
		{
			FScopedPredictionWindow NewScopedWindow(AbilitySystemComponent, true);
			HandleGameplayEvent(Payload);
			AbilitySystemComponent->HandleGameplayEvent(EventTag, &Payload);
		}
	}
}

FString UAnimNotify_GameplayEvent::GetNotifyName_Implementation() const
{
	if (EventTag.IsValid())
	{
		return EventTag.ToString();
	}

	return TEXT("GameplayEvent");
}

void UAnimNotify_GameplayEvent::HandleGameplayEvent(FGameplayEventData& InPayload)
{
}
