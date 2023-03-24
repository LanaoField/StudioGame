// Copyright 2022 CQUnreal. All Rights Reserved.

#include "Animation/AnimNotifies/AnimNotifyState_GameplayEffect.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"


UAnimNotifyState_GameplayEffect::UAnimNotifyState_GameplayEffect(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	FWorldDelegates::OnWorldCleanup.AddUObject(this, &UAnimNotifyState_GameplayEffect::OnWorldCleanup);
}

void UAnimNotifyState_GameplayEffect::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
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
				for (TPair<FName, float> Pair : SetByCallerMagnitudeNames)
				{
					SpecHandle.Data->SetSetByCallerMagnitude(Pair.Key, Pair.Value);
				}
				for (TPair<FGameplayTag, float> Pair : SetByCallerMagnitudeTags)
				{
					SpecHandle.Data->SetSetByCallerMagnitude(Pair.Key, Pair.Value);
				}

				FActiveGameplayEffectHandle EffectHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
				EffectHandles.Add(Actor, EffectHandle);
			}
		}
	}
}

void UAnimNotifyState_GameplayEffect::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{

}

void UAnimNotifyState_GameplayEffect::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	FActiveGameplayEffectHandle EffectHandle;
	if (EffectHandles.RemoveAndCopyValue(MeshComp ? MeshComp->GetOwner() : nullptr, EffectHandle))
	{
		AActor* Actor = MeshComp ? MeshComp->GetOwner() : nullptr;
		if (IsValid(Actor) && Actor->GetLocalRole() == ENetRole::ROLE_Authority)
		{
			IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(Actor);
			UAbilitySystemComponent* AbilitySystemComponent = AbilitySystemInterface ? AbilitySystemInterface->GetAbilitySystemComponent() : nullptr;
			if (IsValid(AbilitySystemComponent))
			{
				AbilitySystemComponent->RemoveActiveGameplayEffect(EffectHandle);
			}
		}
	}
}

FString UAnimNotifyState_GameplayEffect::GetNotifyName_Implementation() const
{
	if (GameplayEffectClass != nullptr)
	{
		return *GetNameSafe(GameplayEffectClass);
	}

	return TEXT("GameplayEffect");
}

void UAnimNotifyState_GameplayEffect::OnWorldCleanup(UWorld* World, bool bSessionEnded, bool bCleanupResources)
{
	EffectHandles.Empty();
}