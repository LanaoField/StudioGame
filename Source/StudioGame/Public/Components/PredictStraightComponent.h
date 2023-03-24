// Copyright 2018 CQUnreal, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SplineMeshComponent.h"
#include "PredictStraightComponent.generated.h"

UCLASS(ClassGroup = (Predict), ShowCategories=(Activation), meta = (BlueprintSpawnableComponent))
class STUDIOGAME_API UPredictStraightComponent : public USplineMeshComponent
{	
	GENERATED_UCLASS_BODY()

public:
	virtual void InitializeComponent() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;
	virtual void Activate(bool bReset = false) override;
	virtual void Deactivate() override;

	void InitPredictStraight();
	void UpdatePredictStraight(float DeltaTime);
	void SetLastHitResult(const FHitResult& HitResult);
	void SetEnableInternalTrace(bool bEnable);

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	virtual void PressPointerKey(FKey Key);

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	virtual void ReleasePointerKey(FKey Key);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Predict)
	TEnumAsByte<ECollisionChannel> TraceChannel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Predict)
	bool bIsSweepSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Predict, meta=(EditCondition = "bIsSweepSphere"))
	float SphereRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Predict)
	float TraceDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Predict)
	FVector SpotScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Predict)
	UStaticMesh* SpotMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Predict)
	TArray<UMaterialInterface*> SpotMaterials;

	UPROPERTY(Transient)
	class UStaticMeshComponent* SpotMeshComponent;

	UPROPERTY(Transient)
	FHitResult LastHitResult;

	UPROPERTY(Transient)
	UPrimitiveComponent* HoveredComponent;

	UPROPERTY(Transient)
	UPrimitiveComponent* InteractiveComponent;

protected:
	UPROPERTY()
	bool bInitialed;

	UPROPERTY()
	bool bIsActivePredict;

	UPROPERTY()
	bool bEnableInternalTrace;

	UPROPERTY()
	class UWidgetComponent* HoveredWidgetComponent;
};
