// Copyright 2022 CQUnreal. All Rights Reserved.

#include "Abilities/GameplayAbilityTargetActor_Transform.h"
#include "Abilities/GameplayAbilityTargetTypesEx.h"


AGameplayAbilityTargetActor_Transform::AGameplayAbilityTargetActor_Transform(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void AGameplayAbilityTargetActor_Transform::ConfirmTargetingAndContinue()
{
	K2_PreConfirmTargetingAndContinue();

	if (ShouldProduceTargetData())
	{
		FGameplayAbilityTargetDataHandle Handle;
		for (const FTransform& Value : ValueArray)
		{
			Handle.Add(new FGameplayAbilityTargetData_Transform(Value));
		}
		TargetDataReadyDelegate.Broadcast(Handle);
	}
}

void AGameplayAbilityTargetActor_Transform::Add(const FTransform& NewValue)
{
	ValueArray.Add(NewValue);
}

void AGameplayAbilityTargetActor_Transform::Clear()
{
	ValueArray.Empty();
}

int32 AGameplayAbilityTargetActor_Transform::Num() const
{
	return ValueArray.Num();
}





