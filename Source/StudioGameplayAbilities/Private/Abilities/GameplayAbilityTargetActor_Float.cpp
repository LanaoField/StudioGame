// Copyright 2022 CQUnreal. All Rights Reserved.

#include "Abilities/GameplayAbilityTargetActor_Float.h"
#include "Abilities/GameplayAbilityTargetTypesEx.h"


AGameplayAbilityTargetActor_Float::AGameplayAbilityTargetActor_Float(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void AGameplayAbilityTargetActor_Float::ConfirmTargetingAndContinue()
{
	K2_PreConfirmTargetingAndContinue();

	if (ShouldProduceTargetData())
	{
		FGameplayAbilityTargetDataHandle Handle;
		for (const float& Value : ValueArray)
		{
			Handle.Add(new FGameplayAbilityTargetData_Float(Value));
		}
		TargetDataReadyDelegate.Broadcast(Handle);
	}
}

void AGameplayAbilityTargetActor_Float::Add(const float& NewValue)
{
	ValueArray.Add(NewValue);
}

void AGameplayAbilityTargetActor_Float::Clear()
{
	ValueArray.Empty();
}

int32 AGameplayAbilityTargetActor_Float::Num() const
{
	return ValueArray.Num();
}





