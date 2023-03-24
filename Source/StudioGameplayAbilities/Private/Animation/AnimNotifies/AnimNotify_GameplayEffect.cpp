// Copyright 2022 CQUnreal. All Rights Reserved.

#include "Animation/AnimNotifies/AnimNotify_GameplayEffect.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"


UAnimNotify_GameplayEffect::UAnimNotify_GameplayEffect(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, Level(1.f)
	, bLockDuration(true)
{

}

#if ENGINE_MAJOR_VERSION > 4
void UAnimNotify_GameplayEffect::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
#else
void UAnimNotify_GameplayEffect::Notify(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation)
{
	Super::Notify(MeshComp, Animation);
#endif

	AActor* Actor = MeshComp ? MeshComp->GetOwner() : nullptr;
	if (IsValid(Actor) && Actor->GetLocalRole() == ENetRole::ROLE_Authority)
	{
		IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(Actor);
		UAbilitySystemComponent* AbilitySystemComponent = AbilitySystemInterface ? AbilitySystemInterface->GetAbilitySystemComponent() : nullptr;
		if (IsValid(AbilitySystemComponent))
		{
			FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(GameplayEffectClass, Level, FGameplayEffectContextHandle());
			if (SpecHandle.Data.IsValid())
			{
				const float RateScale = Animation ? Animation->RateScale : 1.0f;

				SpecHandle.Data->SetDuration(RateScale * Duration, bLockDuration);
				for (TPair<FName, float> Pair : SetByCallerMagnitudeNames)
				{
					SpecHandle.Data->SetSetByCallerMagnitude(Pair.Key, Pair.Value);
				}
				for (TPair<FGameplayTag, float> Pair : SetByCallerMagnitudeTags)
				{
					SpecHandle.Data->SetSetByCallerMagnitude(Pair.Key, Pair.Value);
				}
				AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			}
		}
	}
}

FString UAnimNotify_GameplayEffect::GetNotifyName_Implementation() const
{
	if (GameplayEffectClass != nullptr)
	{
		return *GetNameSafe(GameplayEffectClass);
	}

	return TEXT("GameplayEffect");
}