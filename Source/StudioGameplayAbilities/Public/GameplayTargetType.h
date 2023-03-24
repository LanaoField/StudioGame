// Copyright 2022 CQUnreal. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "GameplayTargetType.generated.h"

class AActor;
struct FGameplayEventData;

/**
 * Class that is used to determine targeting for abilities
 * It is meant to be blueprinted to run target logic
 * This does not subclass GameplayAbilityTargetActor because this class is never instanced into the world
 * This can be used as a basis for a game-specific targeting blueprint
 * If your targeting is more complicated you may need to instance into the world once or as a pooled actor
 */
UCLASS(Blueprintable, meta = (ShowWorldContextPin))
class STUDIOGAMEPLAYABILITIES_API UGameplayTargetType : public UObject
{
	GENERATED_BODY()

public:
	// Constructor and overrides
	UGameplayTargetType() {}

	/** Called to determine targets to apply gameplay effects to */
	UFUNCTION(BlueprintNativeEvent)
	void GetTargets(ACharacter* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const;
};

/** Trivial target type that uses the owner */
UCLASS(NotBlueprintable)
class STUDIOGAMEPLAYABILITIES_API UGameplayTargetType_UseOwner : public UGameplayTargetType
{
	GENERATED_BODY()

public:
	// Constructor and overrides
	UGameplayTargetType_UseOwner() {}

	/** Uses the passed in event data */
	virtual void GetTargets_Implementation(ACharacter* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const override;
};

/** Trivial target type that pulls the target out of the event data */
UCLASS(NotBlueprintable)
class STUDIOGAMEPLAYABILITIES_API UGameplayTargetType_UseEventData : public UGameplayTargetType
{
	GENERATED_BODY()

public:
	// Constructor and overrides
	UGameplayTargetType_UseEventData() {}

	/** Uses the passed in event data */
	virtual void GetTargets_Implementation(ACharacter* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const override;
};

/** Trivial target type that pulls the target out of the event data */
UCLASS(NotBlueprintable)
class STUDIOGAMEPLAYABILITIES_API UGameplayTargetType_SphereTrace : public UGameplayTargetType
{
	GENERATED_BODY()

public:
	// Constructor and overrides
	UGameplayTargetType_SphereTrace()
		: SphereRadius(100.f)
		, TraceLength(1.f)
		, bTraceComplex(true)
		, bReturnPhysicalMaterial(true)
	{
		CollisionChannels.Add(ECollisionChannel::ECC_Pawn);
	}

	/** Uses the passed in event data */
	virtual void GetTargets_Implementation(ACharacter* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const override;

public:
	UPROPERTY(EditDefaultsOnly, Category = TargetType)
	FVector OffsetFromActor;

	UPROPERTY(EditDefaultsOnly, Category = TargetType)
	float SphereRadius;

	UPROPERTY(EditDefaultsOnly, Category = TargetType)
	float TraceLength;

	UPROPERTY(EditDefaultsOnly, Category = TargetType)
	bool bTraceComplex;

	UPROPERTY(EditDefaultsOnly, Category = TargetType)
	bool bReturnPhysicalMaterial;

	UPROPERTY(EditDefaultsOnly, Category = TargetType)
	TArray<TEnumAsByte<ECollisionChannel>> CollisionChannels;
};
