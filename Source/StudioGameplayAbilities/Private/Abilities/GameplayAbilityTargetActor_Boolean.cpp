// Copyright 2022 CQUnreal. All Rights Reserved.

#include "Abilities/GameplayAbilityTargetActor_Boolean.h"
#include "Abilities/GameplayAbilityTargetTypesEx.h"


AGameplayAbilityTargetActor_Boolean::AGameplayAbilityTargetActor_Boolean(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void AGameplayAbilityTargetActor_Boolean::ConfirmTargetingAndContinue()
{
	K2_PreConfirmTargetingAndContinue();

	if (ShouldProduceTargetData())
	{
		FGameplayAbilityTargetDataHandle Handle;
		for (const bool& Value : ValueArray)
		{
			Handle.Add(new FGameplayAbilityTargetData_Boolean(Value));
		}
		TargetDataReadyDelegate.Broadcast(Handle);
	}
}

void AGameplayAbilityTargetActor_Boolean::Add(const bool& NewValue)
{
	ValueArray.Add(NewValue);
}

void AGameplayAbilityTargetActor_Boolean::Clear()
{
	ValueArray.Empty();
}

int32 AGameplayAbilityTargetActor_Boolean::Num() const
{
	return ValueArray.Num();
}





