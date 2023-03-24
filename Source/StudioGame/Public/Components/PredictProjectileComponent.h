// Copyright 2018 CQUnreal, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/GameplayStaticsTypes.h"
#include "Components/PrimitiveComponent.h"
#include "PredictProjectileComponent.generated.h"

UCLASS( ClassGroup=(Predict), meta=(BlueprintSpawnableComponent) )
class STUDIOGAME_API UPredictProjectileComponent : public UPrimitiveComponent
{
	GENERATED_UCLASS_BODY()

	virtual void InitializeComponent() override;

public:
	void InitPredictProjectile();
	void DeactivePredictProjectile();
	void DestroyPredictProjectile();
	void UpdatePredictProjectile(float DeltaTime);
	bool PredictProjectilePath(FPredictProjectilePathResult& PredictResult, bool bTraceWithCollision = false);

protected:
	UPROPERTY()
	bool bInitialed;

	UPROPERTY()
	bool bIsActivePredict;

public:
	UPROPERTY(EditAnywhere, Category = Predict)
	float InitialSpeed;

	UPROPERTY(EditAnywhere, Category = Predict)
	float GravityScale;

	UPROPERTY(EditAnywhere, Category = Predict)
	float Radius;

	UPROPERTY(EditAnywhere, Category = Predict)
	float SimFrequency;

	UPROPERTY(EditAnywhere, Category = Predict)
	float MaxSimTime;

	UPROPERTY(EditAnywhere, Category = Predict)
	FVector Tangent;

	UPROPERTY(EditAnywhere, Category = Predict)
	class UStaticMesh* SplineMesh;

	UPROPERTY(EditAnywhere, Category = Predict)
	TArray<UMaterialInterface*> SplineMaterials;

	UPROPERTY(EditAnywhere, Category = Predict)
	FVector SplineScale3D;

	UPROPERTY(Transient)
	float CurrentProjectDistance;

	UPROPERTY(EditAnywhere, Category = Predict)
	float ProjectForceFeedbackPlayDistance;

	UPROPERTY(EditAnywhere, Category = Predict)
	class UForceFeedbackEffect* ProjectForceFeedbackEffect;

	UPROPERTY(EditAnywhere, Category = Predict)
	TEnumAsByte<ECollisionChannel> TraceChannel;

	UPROPERTY(EditAnywhere, Category = Predict)
	bool bProjectPointToNavigation;

	UPROPERTY(EditAnywhere, Category = Predict)
	FVector ProjectNavExtend;

	UPROPERTY(EditAnywhere, Category = Predict)
	float ProjectNavLandOffsetZ;

	UPROPERTY(Transient)
	TArray<class USplineMeshComponent*> SplineComponents;

	UPROPERTY(EditAnywhere, Category = Predict)
	FName ColorName;

	UPROPERTY(EditAnywhere, Category = Predict)
	FColor PreviewColor;

	UPROPERTY(EditAnywhere, Category = Predict)
	FColor ProhibitColor;

	UPROPERTY(EditAnywhere, Category = Predict)
	FVector PointScale3D;

	UPROPERTY(EditAnywhere, Category = Predict)
	class UStaticMesh* PointMesh;

	UPROPERTY(EditAnywhere, Category = Predict)
	TArray<UMaterialInterface*> PointMaterials;

	UPROPERTY(Transient)
	class UStaticMeshComponent* PointMeshComponent;

	UPROPERTY(Transient)
	FPredictProjectilePathResult LastPredictResult;

	UPROPERTY(EditAnywhere, Category = Predict)
	class UForceFeedbackEffect* AdsorptionForceFeedbackEffect;

	UPROPERTY(Transient)
	AActor* LastAdsorptionActor;

	UPROPERTY(EditAnywhere, Category = Predict)
	TArray<TSubclassOf<AActor>> AdsorptionActorClasses;

protected:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;

public:
	virtual void Activate(bool bReset = false) override;
	virtual void Deactivate() override;
};
