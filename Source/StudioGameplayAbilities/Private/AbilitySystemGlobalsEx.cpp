// Copyright 2022 CQUnreal. All Rights Reserved.

#include "AbilitySystemGlobalsEx.h"
#include "GameplayEffectTypesEx.h"


FGameplayEffectContext* UAbilitySystemGlobalsEx::AllocGameplayEffectContext() const
{
	return new FGameplayEffectContextEx();
}