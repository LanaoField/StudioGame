// Copyright 2022 CQUnreal. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_Attack.generated.h"


class UAbilitySystemComponent;

UENUM(BlueprintType)
enum class ESweepShape : uint8
{
	Line,
	Box,
	Sphere,
	Capsule
};

/**
 * Attack
 */
UCLASS(editinlinenew, const, hidecategories = Object, collapsecategories, meta = (DisplayName = "Attack"))
class STUDIOGAMEPLAYABILITIES_API UAnimNotifyState_Attack : public UAnimNotifyState
{
	GENERATED_UCLASS_BODY()

	/** Sweep one or more actors for HandleGameplayEvent */
	UPROPERTY(EditAnywhere, Category = AnimNotify)
	FGameplayTag EventTag;

	/** The position of the socket on the collision detection mesh */
	UPROPERTY(EditAnywhere, Category = AnimNotify)
	FName SocketName;

	/** Collision detection channels */
	UPROPERTY(EditAnywhere, Category = AnimNotify)
	TArray<TEnumAsByte<ECollisionChannel>> CollisionChannels;

	/** Whether we should trace against complex collision */
	UPROPERTY(EditAnywhere, Category = AnimNotify)
	bool bTraceComplex;

	/** Whether we want to include the physical material in the results. */
	UPROPERTY(EditAnywhere, Category = AnimNotify)
	bool bReturnPhysicalMaterial;

	/** Predict distance */
	UPROPERTY(EditAnywhere, Category = AnimNotify)
	float PredictDistance;

	/** Relative transform */
	UPROPERTY(EditAnywhere, Category = AnimNotify)
	FTransform RelativeTransform;

	/** Collision shape */
	UPROPERTY(EditAnywhere, Category = AnimNotify)
	ESweepShape SweepShape;

	/** Half extent */
	UPROPERTY(EditAnywhere, Category = AnimNotify, Meta = (EditConditionHides, EditCondition = "SweepShape == ESweepShape::Box"))
	FVector HalfExtent;

	/** Sphere or capsule of radius */
	UPROPERTY(EditAnywhere, Category = AnimNotify, Meta = (EditConditionHides, EditCondition = "SweepShape == ESweepShape::Sphere || SweepShape == ESweepShape::Capsule"))
	float Radius;

	/** Capsule of half height */
	UPROPERTY(EditAnywhere, Category = AnimNotify, Meta = (EditConditionHides, EditCondition = "SweepShape == ESweepShape::Capsule"))
	float HalfHeight;

#if WITH_EDITORONLY_DATA
	/** Draw debugging information */
	UPROPERTY(EditAnywhere, Category = Debug)
	bool bDrawDebug;

	/** 调试命中框持续时间 */
	UPROPERTY(EditAnywhere, Category = Debug, Meta = (EditCondition = "bDrawDebug"))
	float LifeTime;

	/** 调试框持续时间 */
	UPROPERTY(EditAnywhere, Category = Debug, Meta = (EditCondition = "bDrawDebug"))
	float DrawLifeTime;

	/** 调试框厚度 */
	UPROPERTY(EditAnywhere, Category = Debug, Meta = (EditCondition = "bDrawDebug"))
	float Thickness;
#endif

	virtual void BranchingPointNotifyBegin(FBranchingPointNotifyPayload& BranchingPointPayload) override;
	virtual void BranchingPointNotifyTick(FBranchingPointNotifyPayload& BranchingPointPayload, float FrameDeltaTime) override;
	virtual void BranchingPointNotifyEnd(FBranchingPointNotifyPayload& BranchingPointPayload) override;

	virtual FString GetNotifyName_Implementation() const override;

	virtual void HandleGameplayEvent(FGameplayEventData& Payload);
	virtual void OnWorldCleanup(UWorld* World, bool bSessionEnded, bool bCleanupResources);

private:
	UPROPERTY(Transient)
	FVector StartLocation;

	TMap<int32, TArray<AActor*>> SweepActors;
};
