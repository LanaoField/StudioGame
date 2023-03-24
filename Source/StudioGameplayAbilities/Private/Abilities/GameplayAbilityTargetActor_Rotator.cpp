// Copyright 2022 CQUnreal. All Rights Reserved.

#include "Abilities/GameplayAbilityTargetActor_Rotator.h"
#include "Abilities/GameplayAbilityTargetTypesEx.h"


AGameplayAbilityTargetActor_Rotator::AGameplayAbilityTargetActor_Rotator(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void AGameplayAbilityTargetActor_Rotator::ConfirmTargetingAndContinue()
{
	K2_PreConfirmTargetingAndContinue();

	if (ShouldProduceTargetData())
	{
		FGameplayAbilityTargetDataHandle Handle;
		for (const FRotator& Value : ValueArray)
		{
			Handle.Add(new FGameplayAbilityTargetData_Rotator(Value));
		}
		TargetDataReadyDelegate.Broadcast(Handle);
	}
}

void AGameplayAbilityTargetActor_Rotator::Add(const FRotator& NewValue)
{
	ValueArray.Add(NewValue);
}

void AGameplayAbilityTargetActor_Rotator::Clear()
{
	ValueArray.Empty();
}

int32 AGameplayAbilityTargetActor_Rotator::Num() const
{
	return ValueArray.Num();
}





