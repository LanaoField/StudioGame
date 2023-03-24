// Copyright 2022 CQUnreal. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_DistanceTo.generated.h"

struct FBTDistanceToMemory
{
	FBTDistanceToMemory()
		: ElapsedTime(0.f)
		, DelayTime(0.f)
	{

	}

	float ElapsedTime;

	float DelayTime;
};

/**
 * UBTDecorator_DistanceTo
 */
UCLASS(HideCategories = (Condition))
class STUDIOGAME_API UBTDecorator_DistanceTo : public UBTDecorator
{
	GENERATED_UCLASS_BODY()

	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;

	/** calculates raw, core value of decorator's condition. Should not include calling IsInversed */
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

	/** called when execution flow controller becomes active */
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	/** called when execution flow controller becomes inactive */
	virtual void OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	/** this function should be considered as const (don't modify state of object) if node is not instanced! */
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	/** size of instance memory */
	virtual uint16 GetInstanceMemorySize() const override;

protected:
	UPROPERTY(EditAnywhere, Category = Distance)
	float MinDistance;

	UPROPERTY(EditAnywhere, Category = Distance)
	float MaxDistance;

	UPROPERTY(EditAnywhere, Category = Distance)
	float Delay;

	UPROPERTY(EditAnywhere, Category = Distance)
	float DelayDeviation;

	UPROPERTY(EditAnywhere, Category = Distance)
	struct FBlackboardKeySelector Target;
};
