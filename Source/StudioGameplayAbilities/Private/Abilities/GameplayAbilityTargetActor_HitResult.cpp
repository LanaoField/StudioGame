// Copyright 2022 CQUnreal. All Rights Reserved.

#include "Abilities/GameplayAbilityTargetActor_HitResult.h"
#include "Abilities/GameplayAbilityTargetTypesEx.h"


AGameplayAbilityTargetActor_HitResult::AGameplayAbilityTargetActor_HitResult(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void AGameplayAbilityTargetActor_HitResult::ConfirmTargetingAndContinue()
{
	K2_PreConfirmTargetingAndContinue();

	if (ShouldProduceTargetData())
	{
		FGameplayAbilityTargetDataHandle Handle;
		for (const FHitResult& HitResult : ValueArray)
		{
			Handle.Add(new FGameplayAbilityTargetData_HitResult(HitResult));
		}
		TargetDataReadyDelegate.Broadcast(Handle);
	}
}

void AGameplayAbilityTargetActor_HitResult::Add(const FHitResult& NewValue)
{
	ValueArray.Add(NewValue);
}

void AGameplayAbilityTargetActor_HitResult::Clear()
{
	ValueArray.Empty();
}

int32 AGameplayAbilityTargetActor_HitResult::Num() const
{
	return ValueArray.Num();
}





