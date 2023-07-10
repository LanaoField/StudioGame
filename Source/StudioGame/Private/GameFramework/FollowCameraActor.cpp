// Copyright 2022 CQUnreal. All Rights Reserved.

#include "GameFramework/FollowCameraActor.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "MotionControllerComponent.h"


AFollowCameraActor::AFollowCameraActor(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, ViewDistance(50.f)
	, bEnableFadeout(true)
	, bKeepViewTransform(true)
	, AdjustThreshold(50.f)
	, AdjustSpeed(5.f)
	, OffsetLocation(FVector::ZeroVector)
	, OffsetRotation(FRotator::ZeroRotator)
{
	PrimaryActorTick.bCanEverTick = true;
}

void AFollowCameraActor::BeginPlay()
{
	Super::BeginPlay();
}

void AFollowCameraActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UWorld* World = GetWorld();
	APlayerController* PlayerController = World ? World->GetFirstPlayerController() : nullptr;
	APlayerCameraManager* PlayerCameraManager = PlayerController ? PlayerController->PlayerCameraManager : nullptr;
	if (PlayerCameraManager != nullptr)
	{
		const FVector& CameraLocation = PlayerCameraManager->GetCameraLocation();
		const FRotator& CameraRotation = PlayerCameraManager->GetCameraRotation();

		const FVector& ViewLocation = CameraLocation + FRotator(0.f, CameraRotation.Yaw, 0.f).Vector() * ViewDistance;
		const FRotator& ViewRotation = FRotator(0.f, CameraRotation.Yaw, 0.f);

		if (bShouldBlend)
		{
			const FTransform& ViewTransform = FTransform(ViewRotation, ViewLocation);

			const FVector& ActorLocation = GetActorLocation();
			const FRotator& ActorRotation = GetActorRotation();

			const FVector& TargetLocation = ViewTransform.TransformPosition(OffsetLocation); 
			const FRotator& TargetRotation = ViewTransform.TransformRotation(OffsetRotation.Quaternion()).Rotator(); 
			
			const FVector& NewLocation = FMath::VInterpTo(ActorLocation, TargetLocation, DeltaTime, AdjustSpeed);
			const FRotator& NewRotation = FMath::RInterpTo(ActorRotation, TargetRotation, DeltaTime, AdjustSpeed);

			SetActorLocation(NewLocation);
			SetActorRotation(NewRotation);

			if (NewLocation.Equals(TargetLocation, 1.f))
			{
				bShouldBlend = false;

				KeepLocation = ViewLocation;
				KeepRotation = ViewRotation;
			}
		}
		else
		{
			const float AdjustSquared = AdjustThreshold * AdjustThreshold;
			if (FVector::DistSquared(KeepLocation, ViewLocation) > AdjustSquared)
			{
				bShouldBlend = true;
			}
		}
	}

	const float LifeSpan = GetLifeSpan();
	const bool bEnableCollision = GetActorEnableCollision();
	if (bEnableFadeout && !bEnableCollision && LifeSpan > 0.f)
	{
		const FVector ActorScale = GetActorScale3D();
		const FVector NewActorScale = FMath::VInterpTo(ActorScale, FVector::ZeroVector, DeltaTime, LifeSpan * 10.f);
		SetActorScale3D(NewActorScale);
	}
}

void AFollowCameraActor::SetLifeSpan(float InLifespan)
{
	Super::SetLifeSpan(InLifespan);

	if (InLifespan > 0.f)
	{
		SetActorEnableCollision(false);		
	}
}

void AFollowCameraActor::SetOffsetTransform(const FVector& NewOffsetLocation, const FRotator& NewOffsetRotation)
{
	OffsetLocation = NewOffsetLocation;
	OffsetRotation = NewOffsetRotation;
}

