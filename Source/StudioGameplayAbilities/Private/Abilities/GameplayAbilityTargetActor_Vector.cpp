// Copyright 2022 CQUnreal. All Rights Reserved.

#include "Abilities/GameplayAbilityTargetActor_Vector.h"
#include "Abilities/GameplayAbilityTargetTypesEx.h"


AGameplayAbilityTargetActor_Vector::AGameplayAbilityTargetActor_Vector(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void AGameplayAbilityTargetActor_Vector::ConfirmTargetingAndContinue()
{
	K2_PreConfirmTargetingAndContinue();

	if (ShouldProduceTargetData())
	{
		FGameplayAbilityTargetDataHandle Handle;
		for (const FVector& Vector : ValueArray)
		{
			Handle.Add(new FGameplayAbilityTargetData_Vector(Vector));
		}
		TargetDataReadyDelegate.Broadcast(Handle);
	}
}

void AGameplayAbilityTargetActor_Vector::Add(const FVector& NewValue)
{
	ValueArray.Add(NewValue);
}

void AGameplayAbilityTargetActor_Vector::Clear()
{
	ValueArray.Empty();
}

int32 AGameplayAbilityTargetActor_Vector::Num() const
{
	return ValueArray.Num();
}





