// Copyright 2022 CQUnreal. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_Attack.h"
#include "AnimNotifyState_RangeAttack.generated.h"

class UAbilitySystemComponent;

/**
 * 从角色位置发出远距离攻击
 */
UCLASS(editinlinenew, const, hidecategories = Object, collapsecategories, meta = (DisplayName = "RangeAttack"))
class STUDIOGAMEPLAYABILITIES_API UAnimNotifyState_RangeAttack : public UAnimNotifyState
{
	GENERATED_UCLASS_BODY()

public:
	/** Sweep one or more actors for HandleGameplayEvent */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AnimNotify)
	FGameplayTag EventTag;

	/** 是否应该追踪复杂的碰撞 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AnimNotify)
	bool bTraceComplex;

	/** 是否想要在结果中包含物理材料. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AnimNotify)
	bool bReturnPhysicalMaterial;

	/** 碰撞检测通道 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AnimNotify)
	TArray<TEnumAsByte<ECollisionChannel>> CollisionChannels;

	/** 攻击距离 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AnimNotify)
	float AttackDistance;

	/** 角色偏移位置，默认角色正前方50 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AnimNotify)
	FVector OffsetLocation;

	/** 碰撞的形状 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AnimNotify)
	ESweepShape SweepShape;

	/** 范围 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AnimNotify, Meta = (EditConditionHides, EditCondition = "SweepShape == ESweepShape::Box"))
	FVector HalfExtent;

	/** 球形或胶囊的半径 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AnimNotify, Meta = (EditConditionHides, EditCondition = "SweepShape == ESweepShape::Sphere || SweepShape == ESweepShape::Capsule"))
	float Radius;

	/** 球形或胶囊的高 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AnimNotify, Meta = (EditConditionHides, EditCondition = "SweepShape == ESweepShape::Capsule"))
	float HalfHeight;

	/** 碰撞盒移动曲线 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AnimNotify, Meta = (EditConditionHides, EditCondition = "!bLinearInter"))
	UCurveFloat* Curve;

	/** 是否是线性插值移动 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AnimNotify)
	bool bLinearInter;

	/** 碰撞盒移动总耗时，小于该状态时间时，碰撞盒会停留在终点 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AnimNotify)
	float ElapsedTime;

#if WITH_EDITORONLY_DATA
	/** 绘制调试信息 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Debug)
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

	FVector GetSweepLocation() const;

	virtual void BranchingPointNotifyBegin(FBranchingPointNotifyPayload& BranchingPointPayload) override;
	virtual void BranchingPointNotifyTick(FBranchingPointNotifyPayload& BranchingPointPayload, float FrameDeltaTime) override;
	virtual void BranchingPointNotifyEnd(FBranchingPointNotifyPayload& BranchingPointPayload) override;

	virtual FString GetNotifyName_Implementation() const override;

	virtual void HandleGameplayEvent(FGameplayEventData& Payload);
	virtual void OnWorldCleanup(UWorld* World, bool bSessionEnded, bool bCleanupResources);

private:
	//该状态已经用的时间
	UPROPERTY(Transient)
	float StateConsumeDuration;

	UPROPERTY(Transient)
	FTransform StartTransform;

	UPROPERTY(Transient)
	FVector LastLocation;

	UPROPERTY(Transient)
	FVector EndLocation;

	TMap<int32, TArray<AActor*>> SweepActors;
};
