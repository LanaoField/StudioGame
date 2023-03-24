// Copyright 2022 CQUnreal. All Rights Reserved.

#include "Decorator/BTDecorator_Damaged.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "GameFramework/Actor.h"


UBTDecorator_Damaged::UBTDecorator_Damaged(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, Damage(0.f)
	, DamageDeviation(0.f)
	, Delay(0.5f)
	, DelayDeviation(0.f)
{
	bNotifyBecomeRelevant = true;
	bNotifyCeaseRelevant = true;
	bNotifyTick = true;

	NodeName = "Damaged Condition";
}

bool UBTDecorator_Damaged::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	FBTDamagedMemory* DamagedMemory = (FBTDamagedMemory*)NodeMemory;
	return !DamagedMemory->bIsReachCondition;
}

void UBTDecorator_Damaged::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	FBTDamagedMemory* DamagedMemory = (FBTDamagedMemory*)NodeMemory;
	DamagedMemory->bIsReachCondition = false;
	DamagedMemory->CurrentDamage = 0.f;
	DamagedMemory->DamageValue = FMath::FRandRange(Damage - DamageDeviation, Damage + DamageDeviation);
	DamagedMemory->ReachTime = 0.f;
	DamagedMemory->DelayTime = FMath::FRandRange(Delay - DelayDeviation, Delay + DelayDeviation);

	AAIController* AIController = Cast<AAIController>(OwnerComp.GetOwner());
	APawn* Pawn = AIController ? Cast<APawn>(AIController->GetPawn()) : nullptr;
	if (Pawn != nullptr)
	{
		BTDamagedMemorys.Add(Pawn, *DamagedMemory);

		if (!Pawn->IsPendingKillPending())
		{
			Pawn->OnTakeAnyDamage.AddUniqueDynamic(this, &UBTDecorator_Damaged::OnTakeAnyDamage);
		}
		
		//Pawn->OnDestroyed.
	}
}

void UBTDecorator_Damaged::OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	FBTDamagedMemory* DamagedMemory = (FBTDamagedMemory*)NodeMemory;
	DamagedMemory->bIsReachCondition = false;
	DamagedMemory->CurrentDamage = 0.f;
	DamagedMemory->DamageValue = 0.f;
	DamagedMemory->ReachTime = 0.f;
	DamagedMemory->DelayTime = 0.f;

	AAIController* AIController = Cast<AAIController>(OwnerComp.GetOwner());
	APawn* Pawn = AIController ? Cast<APawn>(AIController->GetPawn()) : nullptr;
	if (Pawn != nullptr)
	{
		BTDamagedMemorys.Remove(Pawn);

		Pawn->OnTakeAnyDamage.RemoveDynamic(this, &UBTDecorator_Damaged::OnTakeAnyDamage);
	}
}

void UBTDecorator_Damaged::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	FBTDamagedMemory* DamagedMemory = (FBTDamagedMemory*)NodeMemory;
	if (DamagedMemory->bIsReachCondition &&
		DamagedMemory->CurrentDamage >= DamagedMemory->DamageValue &&
		OwnerComp.GetWorld()->TimeSeconds - DamagedMemory->ReachTime >= DamagedMemory->DelayTime)
	{
		ConditionalFlowAbort(OwnerComp, EBTDecoratorAbortRequest::ConditionPassing);

		//UE_LOG(LogTemp, Log, TEXT("CurrentDamage [%.2f] DamageValue[%.2f] ReachTime[%.2f] DelayTime[%.2f]"), DamagedMemory->CurrentDamage, DamagedMemory->DamageValue, DamagedMemory->ReachTime, DamagedMemory->DelayTime);
	}
}

uint16 UBTDecorator_Damaged::GetInstanceMemorySize() const
{
	return sizeof(FBTDamagedMemory);
}

void UBTDecorator_Damaged::OnInstanceDestroyed(UBehaviorTreeComponent& OwnerComp)
{
	AAIController* AIController = Cast<AAIController>(OwnerComp.GetOwner());
	APawn* Pawn = AIController ? Cast<APawn>(AIController->GetPawn()) : nullptr;
	if (Pawn != nullptr)
	{
		BTDamagedMemorys.Remove(Pawn);

		Pawn->OnTakeAnyDamage.RemoveDynamic(this, &UBTDecorator_Damaged::OnTakeAnyDamage);
	}

	Super::OnInstanceDestroyed(OwnerComp);
}

void UBTDecorator_Damaged::OnTakeAnyDamage(AActor* DamagedActor, float ActualDamage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	FBTDamagedMemory* BTDamagedMemory = BTDamagedMemorys.Find(DamagedActor);
	if (BTDamagedMemory != nullptr)
	{
		if (!BTDamagedMemory->bIsReachCondition)
		{
			BTDamagedMemory->CurrentDamage += ActualDamage;
			if (BTDamagedMemory->CurrentDamage >= BTDamagedMemory->DamageValue)
			{
				BTDamagedMemory->bIsReachCondition = true;
				float TimeSeconds = DamagedActor->GetWorld()->TimeSeconds;
				BTDamagedMemory->ReachTime = TimeSeconds;
			}
		}
	}
}