// Copyright 2022 CQUnreal. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_TimeLimitAdvanced.generated.h"

/**
使用黑板值来设置时间、并可以选择超时后成功还是失败
 */
UCLASS(HideCategories=(Condition))
class STUDIOGAME_API UBTDecorator_TimeLimitAdvanced : public UBTDecorator
{
	GENERATED_UCLASS_BODY()

	virtual void DescribeRuntimeValues(const UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTDescriptionVerbosity::Type Verbosity, TArray<FString>& Values) const override;
	virtual FString GetStaticDescription() const override;

#if WITH_EDITOR
	virtual FName GetNodeIconName() const override;
#endif // WITH_EDITOR

protected:
	virtual void OnNodeActivation(FBehaviorTreeSearchData& SearchData) override;
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
	//黑板值
	UPROPERTY(Category = Decorator, EditAnywhere)
	FBlackboardKeySelector TimeBlackboardKey;

	//超时后是否以成功为结果
	UPROPERTY(Category = Decorator, EditAnywhere)
	bool bSuccesOnTimeLimit;
};
