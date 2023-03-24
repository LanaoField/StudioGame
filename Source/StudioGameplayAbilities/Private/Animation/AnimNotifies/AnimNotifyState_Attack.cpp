// Copyright 2022 CQUnreal. All Rights Reserved.

#include "Animation/AnimNotifies/AnimNotifyState_Attack.h"
#include "Abilities/GameplayAbilityTargetTypesEx.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectTypesEx.h"

#if WITH_EDITORONLY_DATA
#include "DrawDebugHelpers.h"
#endif


UAnimNotifyState_Attack::UAnimNotifyState_Attack(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, PredictDistance(20.f)
	, HalfExtent(10.f)
	, Radius(10.f)
	, HalfHeight(15.f)
	#if WITH_EDITORONLY_DATA
	, LifeTime(10.f)
	, DrawLifeTime(0.f)
	, Thickness(1.f)
	#endif
{
	bIsNativeBranchingPoint = true;

	FWorldDelegates::OnWorldCleanup.AddUObject(this, &UAnimNotifyState_Attack::OnWorldCleanup);
}

FTransform GetSweepTransform(USkeletalMeshComponent* MeshComp, const FName& SocketName, const FTransform& RelativeTransform)
{
	if (MeshComp != nullptr)
	{
		FTransform SocketTransform = MeshComp->GetSocketTransform(SocketName);
		return RelativeTransform * SocketTransform;
	}
	else
	{
		return FTransform::Identity;
	}
}

void UAnimNotifyState_Attack::BranchingPointNotifyBegin(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	// UE_LOG(LogStudioGame, Log, TEXT("%s MontageInstanceID: %d"), TEXT("BranchingPointNotifyBegin"), BranchingPointPayload.MontageInstanceID);

	SweepActors.Remove(BranchingPointPayload.MontageInstanceID);

	const FTransform& SweepTransform = GetSweepTransform(BranchingPointPayload.SkelMeshComponent, SocketName, RelativeTransform);
	StartLocation = SweepTransform.GetLocation();
}

void UAnimNotifyState_Attack::BranchingPointNotifyTick(FBranchingPointNotifyPayload& BranchingPointPayload, float FrameDeltaTime)
{
	if (BranchingPointPayload.SkelMeshComponent == nullptr)
	{
		return;
	}

	UWorld* World = BranchingPointPayload.SkelMeshComponent->GetWorld();
	if (World == nullptr)
	{
		return;
	}

	const FTransform& SweepTransform = GetSweepTransform(BranchingPointPayload.SkelMeshComponent, SocketName, RelativeTransform);
	const FVector Location = SweepTransform.GetLocation();

	const FVector Start = StartLocation;
	const FVector End = Location + (Location - Start).GetSafeNormal() * PredictDistance;

	FQuat Rot = SweepTransform.GetRotation();

	StartLocation = End;

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

void UAnimNotifyState_Attack::BranchingPointNotifyEnd(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	SweepActors.Remove(BranchingPointPayload.MontageInstanceID);
}

FString UAnimNotifyState_Attack::GetNotifyName_Implementation() const
{
	if (EventTag.IsValid())
	{
		return EventTag.ToString();
	}

	return TEXT("Attack");
}

void UAnimNotifyState_Attack::HandleGameplayEvent(FGameplayEventData& Payload)
{

}

void UAnimNotifyState_Attack::OnWorldCleanup(UWorld* World, bool bSessionEnded, bool bCleanupResources)
{
	SweepActors.Empty();
}
