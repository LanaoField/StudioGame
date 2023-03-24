// Copyright 2022 CQUnreal. All Rights Reserved.

#include "Abilities/GameplayAbilityEx.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GameplayEffectTypesEx.h"
#include "GameplayTargetType.h"
#include "Containers/Ticker.h"


UGameplayAbilityEx::UGameplayAbilityEx(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UGameplayAbilityEx::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	K2_InputPressed(Handle, *ActorInfo, ActivationInfo);
}

void UGameplayAbilityEx::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	K2_InputReleased(Handle, *ActorInfo, ActivationInfo);
}

void UGameplayAbilityEx::K2_ExecuteGameplayCueWithTargetData(FGameplayTag GameplayCueTag, const FGameplayAbilityTargetDataHandle& TargetDataHandle, UObject* SourceObject, AActor* Instigator, AActor* EffectCauser, FVector Origin)
{
	FGameplayEffectContextEx* NewEffectContext = new FGameplayEffectContextEx();
	NewEffectContext->SetAbility(this);
	NewEffectContext->AddSourceObject(SourceObject);
	NewEffectContext->AddInstigator(Instigator, EffectCauser);
	NewEffectContext->AddOrigin(Origin);
	NewEffectContext->AddTargetData(TargetDataHandle);

	K2_ExecuteGameplayCue(GameplayCueTag, FGameplayEffectContextHandle(NewEffectContext));
}

void UGameplayAbilityEx::BP_RemoveGameplayEffectFromOwnerWithHandleArray(TArray<FActiveGameplayEffectHandle> HandleArray, int32 StacksToRemove)
{
	if (HasAuthority(&CurrentActivationInfo) == false)
	{
		return;
	}

	UAbilitySystemComponent* const AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo_Ensured();
	if (AbilitySystemComponent)
	{
		for (const FActiveGameplayEffectHandle& Handle : HandleArray)
		{
			AbilitySystemComponent->RemoveActiveGameplayEffect(Handle, StacksToRemove);
		}
	}
}

FActiveGameplayEffectHandle UGameplayAbilityEx::K2_ApplyGameplayEffectClassToOwner(TSubclassOf<UGameplayEffect> GameplayEffectClass)
{
	FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(GameplayEffectClass);
	return ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpecHandle);
}

TArray<FActiveGameplayEffectHandle> UGameplayAbilityEx::K2_ApplyGameplayEffectClassToTarget(TSubclassOf<UGameplayEffect> GameplayEffectClass, FGameplayAbilityTargetDataHandle TargetData)
{
	FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(GameplayEffectClass);
	return ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpecHandle, TargetData);
}

FGameplayEffectContainerSpec UGameplayAbilityEx::MakeEffectContainerSpecFromContainer(const FGameplayEffectContainer& Container, const FGameplayEventData& EventData, int32 OverrideGameplayLevel)
{
	// First figure out our actor info
	FGameplayEffectContainerSpec ReturnSpec;
	AActor* OwningActor = GetOwningActorFromActorInfo();
	ACharacter* OwningCharacter = Cast<ACharacter>(OwningActor);
	UAbilitySystemComponent* OwningASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwningActor);

	if (OwningASC)
	{
		// If we have a target type, run the targeting logic. This is optional, targets can be added later
		if (Container.TargetType.Get())
		{
			TArray<FHitResult> HitResults;
			TArray<AActor*> TargetActors;
			const UGameplayTargetType* TargetTypeCDO = Container.TargetType.GetDefaultObject();
			AActor* AvatarActor = GetAvatarActorFromActorInfo();
			TargetTypeCDO->GetTargets(OwningCharacter, AvatarActor, EventData, HitResults, TargetActors);
			ReturnSpec.AddTargets(HitResults, TargetActors);
		}

		// If we don't have an override level, use the default on the ability itself
		if (OverrideGameplayLevel == INDEX_NONE)
		{
			OverrideGameplayLevel = OverrideGameplayLevel = this->GetAbilityLevel(); //OwningASC->GetDefaultAbilityLevel();
		}

		// Build GameplayEffectSpecs for each applied effect
		for (const TSubclassOf<UGameplayEffect>& EffectClass : Container.TargetGameplayEffectClasses)
		{
			ReturnSpec.TargetGameplayEffectSpecs.Add(MakeOutgoingGameplayEffectSpec(EffectClass, OverrideGameplayLevel));
		}
	}
	return ReturnSpec;
}

FGameplayEffectContainerSpec UGameplayAbilityEx::MakeEffectContainerSpec(FGameplayTag ContainerTag, const FGameplayEventData& EventData, int32 OverrideGameplayLevel)
{
	FGameplayEffectContainer* FoundContainer = EffectContainerMap.Find(ContainerTag);

	if (FoundContainer)
	{
		return MakeEffectContainerSpecFromContainer(*FoundContainer, EventData, OverrideGameplayLevel);
	}
	return FGameplayEffectContainerSpec();
}

TArray<FActiveGameplayEffectHandle> UGameplayAbilityEx::ApplyEffectContainerSpec(const FGameplayEffectContainerSpec& ContainerSpec)
{
	TArray<FActiveGameplayEffectHandle> AllEffects;

	// Iterate list of effect specs and apply them to their target data
	for (const FGameplayEffectSpecHandle& SpecHandle : ContainerSpec.TargetGameplayEffectSpecs)
	{
		AllEffects.Append(K2_ApplyGameplayEffectSpecToTarget(SpecHandle, ContainerSpec.TargetData));
	}
	return AllEffects;
}

TArray<FActiveGameplayEffectHandle> UGameplayAbilityEx::ApplyEffectContainer(FGameplayTag ContainerTag, const FGameplayEventData& EventData, int32 OverrideGameplayLevel)
{
	FGameplayEffectContainerSpec Spec = MakeEffectContainerSpec(ContainerTag, EventData, OverrideGameplayLevel);
	return ApplyEffectContainerSpec(Spec);
}