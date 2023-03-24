// Copyright 2022 CQUnreal. All Rights Reserved.

#include "GameplayEffectTypesEx.h"


/** FGameplayEffectContextEx */
FGameplayEffectContextEx::FGameplayEffectContextEx()
	: FGameplayEffectContext()
{

}

FGameplayEffectContextEx::FGameplayEffectContextEx(AActor* InInstigator, AActor* InEffectCauser)
	: FGameplayEffectContext(InInstigator, InEffectCauser)
{

}

FGameplayEffectContextEx::~FGameplayEffectContextEx()
{

}

FGameplayAbilityTargetDataHandle FGameplayEffectContextEx::GetTargetData() const
{
	return TargetData;
}

void FGameplayEffectContextEx::AddTargetData(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	TargetData.Append(TargetDataHandle);
}

FGameplayEffectContext* FGameplayEffectContextEx::Duplicate() const
{
	FGameplayEffectContextEx* NewContext = new FGameplayEffectContextEx();
	*NewContext = *this;
	NewContext->AddActors(Actors);
	if (GetHitResult())
	{
		// Does a deep copy of the hit result
		NewContext->AddHitResult(*GetHitResult(), true);
	}
	NewContext->TargetData.Append(TargetData);

	return NewContext;
}

bool FGameplayEffectContextEx::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	return Super::NetSerialize(Ar, Map, bOutSuccess) && TargetData.NetSerialize(Ar, Map, bOutSuccess);
}

UScriptStruct* FGameplayEffectContextEx::GetScriptStruct() const
{
	return FGameplayEffectContextEx::StaticStruct();
}