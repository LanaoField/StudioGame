// Copyright 2022 CQUnreal. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/InteractiveActor.h"
#include "FollowCameraActor.generated.h"

UCLASS()
class STUDIOGAME_API AFollowCameraActor : public AActor
{
	GENERATED_UCLASS_BODY()
	
public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetLifeSpan(float InLifespan) override;

public:
	UFUNCTION(BlueprintCallable, Category = HotUpdate)
	virtual void SetOffsetTransform(const FVector& NewOffsetLocation, const FRotator& NewOffsetRotation);

protected:
	UPROPERTY(EditAnywhere, Category = Follow)
	float ViewDistance;

	UPROPERTY(EditAnywhere, Category = Follow)
	bool bKeepViewTransform;

	UPROPERTY(EditAnywhere, Category = Follow, Meta = (EditCondition = "bKeepViewTransform"))
	float AdjustThreshold;

	UPROPERTY(EditAnywhere, Category = Follow, Meta = (EditCondition = "bKeepViewTransform"))
	float AdjustSpeed;

	UPROPERTY(EditAnywhere, Category = Follow, Meta = (EditCondition = "bKeepViewTransform"))
	FVector OffsetLocation;

	UPROPERTY(EditAnywhere, Category = Follow, Meta = (EditCondition = "bKeepViewTransform"))
	FRotator OffsetRotation;

private:
	bool bPenddingActivate;
	bool bAutoActivate;
	bool bShouldBlend;

	FVector KeepLocation;
	FRotator KeepRotation;
};
