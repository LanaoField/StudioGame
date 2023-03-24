// Copyright 2022 CQUnreal. All Rights Reserved.

#include "Animation/AnimNotifies/AnimNotifyState_RangeAttack.h"
#include "Abilities/GameplayAbilityTargetTypesEx.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"

#if WITH_EDITORONLY_DATA
#include "DrawDebugHelpers.h"
#endif


UAnimNotifyState_RangeAttack::UAnimNotifyState_RangeAttack(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, AttackDistance(500.f)
	, OffsetLocation(50.f, 0.f, 0.f)
	, HalfExtent(10.f)
	, Radius(10.f)
	, HalfHeight(15.f)
	, bLinearInter(true)
	, ElapsedTime(1.f)
	#if WITH_EDITORONLY_DATA
	, LifeTime(10.f)
	, DrawLifeTime(0.f)
	, Thickness(1.f)
	#endif
{
	bIsNativeBranchingPoint = true;

	FWorldDelegates::OnWorldCleanup.AddUObject(this, &UAnimNotifyState_RangeAttack::OnWorldCleanup);
}

FVector UAnimNotifyState_RangeAttack::GetSweepLocation() const
{
	FVector Location;
	if (!bLinearInter && Curve)
	{
		//由曲线插值
		float StartTime;
		float EndTime;
		Curve->GetTimeRange(StartTime, EndTime);
		Location = FMath::Lerp(StartTransform.GetLocation(), EndLocation, Curve->GetFloatValue(StateConsumeDuration / ElapsedTime * EndTime));
	}
	else
	{
		//线性插值
		Location = FMath::Lerp(StartTransform.GetLocation(), EndLocation, StateConsumeDuration / ElapsedTime);
	}
	return Location;
}

void UAnimNotifyState_RangeAttack::BranchingPointNotifyBegin(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	SweepActors.Remove(BranchingPointPayload.MontageInstanceID);

	FTransform Transform = FTransform();
	AActor* Owner = BranchingPointPayload.SkelMeshComponent->GetOwner();
	if (Owner != nullptr)
	{
		LastLocation = Owner->GetActorLocation() + Owner->GetActorRotation().RotateVector(OffsetLocation);
		EndLocation = LastLocation + Owner->GetActorForwardVector() * AttackDistance;
		Transform.SetLocation(LastLocation);
		Transform.SetRotation(Owner->GetActorRotation().Quaternion());
	}
	StartTransform = Transform;
	StateConsumeDuration = 0.f;
}

void UAnimNotifyState_RangeAttack::BranchingPointNotifyTick(FBranchingPointNotifyPayload& BranchingPointPayload, float FrameDeltaTime)
{
	StateConsumeDuration += FrameDeltaTime;
	if (BranchingPointPayload.SkelMeshComponent == nullptr)
	{
		return;
	}
	UWorld* World = BranchingPointPayload.SkelMeshComponent->GetWorld();
	if (World == nullptr)
	{
		return;
	}
	const FVector& SweepLocation = GetSweepLocation();
	const FVector Location = SweepLocation;
	const FVector Start = LastLocation;
	const FVector End = Location + (Location - Start).GetSafeNormal();
	FQuat Rot = StartTransform.GetRotation();
	LastLocation = End;

	AActor* Owner = BranchingPointPayload.SkelMeshComponent->GetOwner();
	if (Owner != nullptr)
	{
		FCollisionQueryParams Params;
		Params.bTraceComplex = bTraceComplex;
		Params.bReturnPhysicalMaterial = bReturnPhysicalMaterial;
		Params.AddIgnoredActor(Owner);

		FCollisionObjectQueryParams ObjectParams;
		for (int32 Index = 0; Index < CollisionChannels.Num(); Index++)
		{
			ObjectParams.AddObjectTypesToQuery(CollisionChannels[Index]);
		}

		IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(Owner);
		UAbilitySystemComponent* AbilitySystemComponent = AbilitySystemInterface ? AbilitySystemInterface->GetAbilitySystemComponent() : nullptr;
		if (IsValid(AbilitySystemComponent))
		{
			FCollisionShape CollisionShape;
			switch (SweepShape)
			{
			case ESweepShape::Box:
				CollisionShape = FCollisionShape::MakeBox(HalfExtent);
				break;
			case ESweepShape::Sphere:
				CollisionShape = FCollisionShape::MakeSphere(Radius);
				break;
			case ESweepShape::Capsule:
				CollisionShape = FCollisionShape::MakeCapsule(Radius, HalfHeight);
				break;
			default:
				break;
			}

			TArray<FHitResult> OutHit;
			World->SweepMultiByObjectType(OutHit, Start, End, Rot, ObjectParams, CollisionShape, Params);

			FGameplayEventData Payload;

			Payload.Instigator = Owner;

			TArray<AActor*>& HitActors = SweepActors.FindOrAdd(BranchingPointPayload.MontageInstanceID);
			for (const FHitResult& HitResult : OutHit)
			{
				AActor* HitActor = HitResult.GetActor();
				if (!HitActors.Contains(HitActor))
				{
					HandleGameplayEvent(Payload);
					Payload.TargetData.Add(new FGameplayAbilityTargetData_HitResult(HitResult));

					HitActors.Add(HitActor);

#if WITH_EDITORONLY_DATA
					if (bDrawDebug)
					{
						uint8 DepthPriority = 0;

						DrawDebugPoint(World, HitResult.ImpactPoint, 10.f, FColor::Red, false, LifeTime);
						DrawDebugLine(World, HitResult.ImpactPoint, HitResult.ImpactPoint + HitResult.ImpactNormal * 50.f, FColor::Cyan, false, LifeTime, 0, Thickness);

						switch (SweepShape)
						{
						case ESweepShape::Box:
							DrawDebugBox(World, Start, HalfExtent, Rot, FColor::Silver, false, LifeTime, DepthPriority, Thickness);
							DrawDebugBox(World, End, HalfExtent, Rot, FColor::Emerald, false, LifeTime, DepthPriority, Thickness);
							DrawDebugBox(World, Location, HalfExtent, Rot, FColor::Cyan, false, LifeTime, DepthPriority, Thickness);
							break;
						case ESweepShape::Sphere:
							DrawDebugSphere(World, Start, Radius, 10, FColor::Silver, false, LifeTime, DepthPriority, Thickness);
							DrawDebugSphere(World, End, Radius, 10, FColor::Emerald, false, LifeTime, DepthPriority, Thickness);
							DrawDebugSphere(World, Location, Radius, 10, FColor::Cyan, false, LifeTime, DepthPriority, Thickness);
							break;
						case ESweepShape::Capsule:
							DrawDebugCapsule(World, Start, Radius, HalfHeight, Rot, FColor::Silver, false, LifeTime, DepthPriority, Thickness);
							DrawDebugCapsule(World, End, Radius, HalfHeight, Rot, FColor::Emerald, false, LifeTime, DepthPriority, Thickness);
							DrawDebugCapsule(World, Location, Radius, HalfHeight, Rot, FColor::Cyan, false, LifeTime, DepthPriority, Thickness);
							break;
						default:
							DrawDebugLine(World, Start, End, FColor::Silver, false, LifeTime, DepthPriority, Thickness);
							break;
						}
					}
#endif
				}
			}

			if (Payload.TargetData.Num() > 0)
			{
				FScopedPredictionWindow NewScopedWindow(AbilitySystemComponent, true);
				AbilitySystemComponent->HandleGameplayEvent(EventTag, &Payload);
			}
		}
	}

#if WITH_EDITORONLY_DATA
	if (bDrawDebug)
	{
		FColor const& Color = FColor::Green;
		uint8 DepthPriority = 0;
		switch (SweepShape)
		{
		case ESweepShape::Box:
			DrawDebugBox(World, Location, HalfExtent, Rot, Color, false, DrawLifeTime, DepthPriority, Thickness);
			break;
		case ESweepShape::Sphere:
			DrawDebugSphere(World, Location, Radius, 10, Color, false, DrawLifeTime, DepthPriority, Thickness);
			break;
		case ESweepShape::Capsule:
			DrawDebugCapsule(World, Location, Radius, HalfHeight, Rot, Color, false, DrawLifeTime, DepthPriority, Thickness);
			break;
		default:
			DrawDebugLine(World, Location, End, Color, false, DrawLifeTime, DepthPriority, Thickness);
			break;
		}
	}
#endif
}


void UAnimNotifyState_RangeAttack::BranchingPointNotifyEnd(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	SweepActors.Remove(BranchingPointPayload.MontageInstanceID);
}

FString UAnimNotifyState_RangeAttack::GetNotifyName_Implementation() const
{
	if (EventTag.IsValid())
	{
		return EventTag.ToString();
	}
	return TEXT("RangeAttack");
}

void UAnimNotifyState_RangeAttack::HandleGameplayEvent(FGameplayEventData& Payload)
{

}

void UAnimNotifyState_RangeAttack::OnWorldCleanup(UWorld* World, bool bSessionEnded, bool bCleanupResources)
{
	SweepActors.Empty();
}
