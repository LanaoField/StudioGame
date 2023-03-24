// Copyright 2022 CQUnreal. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "GameplayAbilityTargetTypesEx.generated.h"


USTRUCT(BlueprintType)
struct STUDIOGAMEPLAYABILITIES_API FGameplayAbilityTargetData_Boolean : public FGameplayAbilityTargetData
{
	GENERATED_USTRUCT_BODY()

	FGameplayAbilityTargetData_Boolean()
		: Value(false)
	{

	}

	FGameplayAbilityTargetData_Boolean(const bool& InValue)
		: Value(InValue)
	{

	}

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
	{
		Ar << Value;

		bOutSuccess = true;

		return true;
	}

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FGameplayAbilityTargetData_Boolean::StaticStruct();
	}

	UPROPERTY(BlueprintReadOnly, Category = Value)
	bool Value;
};

FArchive& operator<<(FArchive& Ar, FGameplayAbilityTargetData_Boolean& TargetData);

template<>
struct TStructOpsTypeTraits<FGameplayAbilityTargetData_Boolean> : public TStructOpsTypeTraitsBase2<FGameplayAbilityTargetData_Boolean>
{
	enum
	{
		WithNetSerializer = true	// For now this is REQUIRED for FGameplayAbilityTargetDataHandle net serialization to work
	};
};

USTRUCT(BlueprintType)
struct STUDIOGAMEPLAYABILITIES_API FGameplayAbilityTargetData_Float : public FGameplayAbilityTargetData
{
	GENERATED_USTRUCT_BODY()

	FGameplayAbilityTargetData_Float()
		: Value(0.f)
	{

	}

	FGameplayAbilityTargetData_Float(const float& InValue)
		: Value(InValue)
	{

	}

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
	{
		Ar << Value;

		bOutSuccess = true;

		return true;
	}

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FGameplayAbilityTargetData_Float::StaticStruct();
	}

	UPROPERTY(BlueprintReadOnly, Category = Value)
	float Value;
};

FArchive& operator<<(FArchive& Ar, FGameplayAbilityTargetData_Float& TargetData);

template<>
struct TStructOpsTypeTraits<FGameplayAbilityTargetData_Float> : public TStructOpsTypeTraitsBase2<FGameplayAbilityTargetData_Float>
{
	enum
	{
		WithNetSerializer = true	// For now this is REQUIRED for FGameplayAbilityTargetDataHandle net serialization to work
	};
};

USTRUCT(BlueprintType)
struct STUDIOGAMEPLAYABILITIES_API FGameplayAbilityTargetData_Int32 : public FGameplayAbilityTargetData
{
	GENERATED_USTRUCT_BODY()

	FGameplayAbilityTargetData_Int32()
		: Value(0)
	{

	}

	FGameplayAbilityTargetData_Int32(const float& InValue)
		: Value(InValue)
	{

	}

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
	{
		Ar << Value;

		bOutSuccess = true;

		return true;
	}

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FGameplayAbilityTargetData_Int32::StaticStruct();
	}

	UPROPERTY(BlueprintReadOnly, Category = Value)
	int32 Value;
};

FArchive& operator<<(FArchive& Ar, FGameplayAbilityTargetData_Int32& TargetData);

template<>
struct TStructOpsTypeTraits<FGameplayAbilityTargetData_Int32> : public TStructOpsTypeTraitsBase2<FGameplayAbilityTargetData_Int32>
{
	enum
	{
		WithNetSerializer = true	// For now this is REQUIRED for FGameplayAbilityTargetDataHandle net serialization to work
	};
};


USTRUCT(BlueprintType)
struct STUDIOGAMEPLAYABILITIES_API FGameplayAbilityTargetData_Vector : public FGameplayAbilityTargetData
{
	GENERATED_USTRUCT_BODY()

	FGameplayAbilityTargetData_Vector()
		: Value(FVector::ZeroVector)
	{

	}

	FGameplayAbilityTargetData_Vector(const FVector& InValue)
		: Value(InValue)
	{

	}

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
	{
		Ar << Value;

		bOutSuccess = true;

		return true;
	}

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FGameplayAbilityTargetData_Vector::StaticStruct();
	}

	UPROPERTY(BlueprintReadOnly, Category = Value)
	FVector Value;
};

FArchive& operator<<(FArchive& Ar, FGameplayAbilityTargetData_Vector& TargetData);

template<>
struct TStructOpsTypeTraits<FGameplayAbilityTargetData_Vector> : public TStructOpsTypeTraitsBase2<FGameplayAbilityTargetData_Vector>
{
	enum
	{
		WithNetSerializer = true	// For now this is REQUIRED for FGameplayAbilityTargetDataHandle net serialization to work
	};
};

USTRUCT(BlueprintType)
struct STUDIOGAMEPLAYABILITIES_API FGameplayAbilityTargetData_Rotator : public FGameplayAbilityTargetData
{
	GENERATED_USTRUCT_BODY()

	FGameplayAbilityTargetData_Rotator()
		: Value(FRotator::ZeroRotator)
	{

	}

	FGameplayAbilityTargetData_Rotator(const FRotator& InValue)
		: Value(InValue)
	{

	}

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
	{
		Ar << Value;

		bOutSuccess = true;

		return true;
	}

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FGameplayAbilityTargetData_Rotator::StaticStruct();
	}

	UPROPERTY(BlueprintReadOnly, Category = Value)
	FRotator Value;
};

FArchive& operator<<(FArchive& Ar, FGameplayAbilityTargetData_Rotator& TargetData);

template<>
struct TStructOpsTypeTraits<FGameplayAbilityTargetData_Rotator> : public TStructOpsTypeTraitsBase2<FGameplayAbilityTargetData_Rotator>
{
	enum
	{
		WithNetSerializer = true	// For now this is REQUIRED for FGameplayAbilityTargetDataHandle net serialization to work
	};
};

USTRUCT(BlueprintType)
struct STUDIOGAMEPLAYABILITIES_API FGameplayAbilityTargetData_Transform : public FGameplayAbilityTargetData
{
	GENERATED_USTRUCT_BODY()

	FGameplayAbilityTargetData_Transform()
		: Value(FTransform::Identity)
	{

	}

	FGameplayAbilityTargetData_Transform(const FTransform& InValue)
		: Value(InValue)
	{

	}

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
	{
		Ar << Value;

		bOutSuccess = true;

		return true;
	}

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FGameplayAbilityTargetData_Transform::StaticStruct();
	}

	UPROPERTY(BlueprintReadOnly, Category = Value)
	FTransform Value;
};

FArchive& operator<<(FArchive& Ar, FGameplayAbilityTargetData_Transform& TargetData);

template<>
struct TStructOpsTypeTraits<FGameplayAbilityTargetData_Transform> : public TStructOpsTypeTraitsBase2<FGameplayAbilityTargetData_Transform>
{
	enum
	{
		WithNetSerializer = true	// For now this is REQUIRED for FGameplayAbilityTargetDataHandle net serialization to work
	};
};

USTRUCT(BlueprintType)
struct STUDIOGAMEPLAYABILITIES_API FGameplayAbilityTargetData_HitResult : public FGameplayAbilityTargetData
{
	GENERATED_USTRUCT_BODY()

	FGameplayAbilityTargetData_HitResult()
		: Value()
	{

	}

	FGameplayAbilityTargetData_HitResult(const FHitResult& InValue)
		: Value(InValue)
	{

	}

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
	{
		return Value.NetSerialize(Ar, Map, bOutSuccess);
	}

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FGameplayAbilityTargetData_HitResult::StaticStruct();
	}

	virtual bool HasHitResult() const override
	{
		return true;
	}

	virtual const FHitResult* GetHitResult() const
	{
		return &Value;
	}

	virtual bool HasOrigin() const override
	{
		return true;
	}

	virtual FTransform GetOrigin() const override
	{
		return FTransform((Value.TraceEnd - Value.TraceStart).Rotation(), Value.TraceStart);
	}

	virtual TArray<TWeakObjectPtr<AActor> >	GetActors() const override
	{
		TArray<TWeakObjectPtr<AActor>> Actors;
		#if ENGINE_MAJOR_VERSION <= 4
		if (Value.Actor.IsValid())
		{
			Actors.Push(Value.Actor);
		}
		#else
		if (Value.GetActor())
		{
			Actors.Push(Value.GetActor());
		}
		#endif
		return Actors;
	}

	virtual bool HasEndPoint() const override
	{
		return true;
	}

	virtual FVector GetEndPoint() const override
	{
		return Value.Location;
	}

	virtual void ReplaceHitWith(AActor* NewHitActor, const FHitResult* NewHitResult)
	{
		Value = FHitResult();
		if (NewHitResult != nullptr)
		{
			Value = *NewHitResult;
		}
	}

	UPROPERTY(BlueprintReadOnly, Category = Value)
	FHitResult Value;
};

FArchive& operator<<(FArchive& Ar, FGameplayAbilityTargetData_HitResult& TargetData);

template<>
struct TStructOpsTypeTraits<FGameplayAbilityTargetData_HitResult> : public TStructOpsTypeTraitsBase2<FGameplayAbilityTargetData_HitResult>
{
	enum
	{
		WithNetSerializer = true	// For now this is REQUIRED for FGameplayAbilityTargetDataHandle net serialization to work
	};
};