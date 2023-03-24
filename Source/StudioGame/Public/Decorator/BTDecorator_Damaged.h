// Copyright 2022 CQUnreal. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_Damaged.generated.h"

struct FBTDamagedMemory
{
	FBTDamagedMemory()
		: bIsReachCondition(false)
		, CurrentDamage(0.f)
		, DamageValue(0.f)
		, ReachTime(0.f)
		, DelayTime(0.f)
	{

	}

	bool bIsReachCondition;

	float CurrentDamage;

	float DamageValue;

	float ReachTime;

	float DelayTime;
};

/**
 * UBTDecorator_Damaged
 */
UCLASS(HideCategories = (Condition))
class STUDIOGAME_API UBTDecorator_Damaged : public UBTDecorator
{
	GENERATED_UCLASS_BODY()

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	virtual uint16 GetInstanceMemorySize() const override;

protected:
	virtual void OnInstanceDestroyed(UBehaviorTreeComponent& OwnerComp);

	UFUNCTION()
	virtual void OnTakeAnyDamage(AActor* DamagedActor, float ActualDamage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

	UPROPERTY(EditAnywhere, Category = Damaged)
	float Damage;

	UPROPERTY(EditAnywhere, Category = Damaged)
	float DamageDeviation;

	UPROPERTY(EditAnywhere, Category = Damaged)
	float Delay;

	UPROPERTY(EditAnywhere, Category = Damaged)
	float DelayDeviation;

private:
	TMap<AActor*, FBTDamagedMemory> BTDamagedMemorys;
};
