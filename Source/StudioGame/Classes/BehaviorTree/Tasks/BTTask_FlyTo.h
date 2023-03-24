// Copyright 2018 CQUnreal, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_FlyTo.generated.h"

/**
 * 飞行到指定位置
 */
UCLASS(config=Game)
class STUDIOGAME_API UBTTask_FlyTo : public UBTTask_BlackboardBase
{
	GENERATED_UCLASS_BODY()

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	/** fixed distance added to threshold between AI and goal location in destination reach test */
	UPROPERTY(config, Category = Node, EditAnywhere, meta = (ClampMin = "0.0", UIMin = "0.0"))
	float AcceptableRadius;

	UPROPERTY(config, Category = Node, EditAnywhere, meta = (ClampMin = "0.0", UIMin = "0.0"))
	float TimeOut;

	/** set automatically if fly should use GameplayTasks */
	uint32 bUseGameplayTasks : 1;

private:
	float FlyingTime;
};
