// Copyright 2022 CQUnreal. All Rights Reserved.

#include "Abilities/GameplayAbilityTargetTypesEx.h"


#ifndef IMPLEMENT_TARGETDATA_BITOPERATION
#define IMPLEMENT_TARGETDATA_BITOPERATION(Type) \
FArchive& operator<<(FArchive& Ar, Type& TargetData) \
{	\
	if (Ar.IsError())	\
	{	\
		return Ar;	\
	}	\
	Ar << TargetData;	\
	return Ar;	\
}

IMPLEMENT_TARGETDATA_BITOPERATION(FGameplayAbilityTargetData_Boolean)
IMPLEMENT_TARGETDATA_BITOPERATION(FGameplayAbilityTargetData_Float)
IMPLEMENT_TARGETDATA_BITOPERATION(FGameplayAbilityTargetData_Int32)
IMPLEMENT_TARGETDATA_BITOPERATION(FGameplayAbilityTargetData_Vector)
IMPLEMENT_TARGETDATA_BITOPERATION(FGameplayAbilityTargetData_Rotator)
IMPLEMENT_TARGETDATA_BITOPERATION(FGameplayAbilityTargetData_Transform)
IMPLEMENT_TARGETDATA_BITOPERATION(FGameplayAbilityTargetData_HitResult)
#endif // !IMPLEMENT_TARGETDATA_BITOPERATION
#undef IMPLEMENT_TARGETDATA_BITOPERATION