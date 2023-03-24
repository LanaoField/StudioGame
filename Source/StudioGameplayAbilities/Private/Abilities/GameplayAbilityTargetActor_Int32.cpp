// Copyright 2022 CQUnreal. All Rights Reserved.

#include "Abilities/GameplayAbilityTargetActor_Int32.h"
#include "Abilities/GameplayAbilityTargetTypesEx.h"


AGameplayAbilityTargetActor_Int32::AGameplayAbilityTargetActor_Int32(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void AGameplayAbilityTargetActor_Int32::ConfirmTargetingAndContinue()
{
	K2_PreConfirmTargetingAndContinue();

	if (ShouldProduceTargetData())
	{
		FGameplayAbilityTargetDataHandle Handle;
		for (const int32& Value : ValueArray)
		{
			Handle.Add(new FGameplayAbilityTargetData_Int32(Value));
		}
		TargetDataReadyDelegate.Broadcast(Handle);
	}
}

void AGameplayAbilityTargetActor_Int32::Add(const int32& NewValue)
{
	ValueArray.Add(NewValue);
}

void AGameplayAbilityTargetActor_Int32::Clear()
{
	ValueArray.Empty();
}

int32 AGameplayAbilityTargetActor_Int32::Num() const
{
	return ValueArray.Num();
}





