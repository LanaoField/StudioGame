// Copyright 2018 CQUnreal, Inc. All Rights Reserved.

#include "Components/PredictProjectileComponent.h"
#include "Components/SplineMeshComponent.h"
#include "NavigationSystem.h"
#include "Engine/StaticMeshActor.h"
#include "Kismet/GameplayStatics.h"


UPredictProjectileComponent::UPredictProjectileComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bInitialed(false)
	, InitialSpeed(2000.f)
	, GravityScale(1.f)
	, Radius(0.f)
	, SimFrequency(30.f)
	, MaxSimTime(2.f)
	, Tangent(0.5f, 0.f, 0.f)
	, SplineScale3D(1.f)
	, bProjectPointToNavigation(false)
	, ProjectNavExtend(50.f, 50.f, 50.f)
	, ProjectNavLandOffsetZ(5.f)
	, PreviewColor(0, 153, 233)
	, ProhibitColor(255, 0, 0)
	, PointScale3D(1.f, 1.f, 1.f)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickGroup = ETickingGroup::TG_PrePhysics;
}

void UPredictProjectileComponent::InitializeComponent()
{
	Super::InitializeComponent();

	InitPredictProjectile();
}

void UPredictProjectileComponent::InitPredictProjectile()
{
	if (bInitialed || !IsActive())
	{
		return;
	}

	DestroyPredictProjectile();

	bInitialed = true;

	int32 const StepLimit = FMath::CeilToInt(SimFrequency * MaxSimTime);
	for (int32 Index = 0; Index < StepLimit; Index++)
	{
		const FName SplineMeshName = FName(*FString::Printf(TEXT("%s_%s_%d"), *GetName(), TEXT("SplineMesh"), Index));
		USplineMeshComponent* NewSplineMesh = NewObject<USplineMeshComponent>(GetOwner(), SplineMeshName);
		if (NewSplineMesh != nullptr)
		{
			GetOwner()->AddOwnedComponent(NewSplineMesh);

			NewSplineMesh->CastShadow = false;
			NewSplineMesh->bReceivesDecals = false;
			NewSplineMesh->bAllowSplineEditingPerInstance = true;
			NewSplineMesh->RegisterComponentWithWorld(GetWorld());
			NewSplineMesh->SetOnlyOwnerSee(bOnlyOwnerSee);
			NewSplineMesh->SetOwnerNoSee(bOwnerNoSee);
			NewSplineMesh->SetMobility(EComponentMobility::Movable);
			NewSplineMesh->SetCollisionProfileName(TEXT("NoCollision"));
			NewSplineMesh->SetStaticMesh(SplineMesh);
			for (int32 ElementIndex = 0; ElementIndex < SplineMaterials.Num(); ElementIndex++)
			{
				NewSplineMesh->SetMaterial(ElementIndex, SplineMaterials[ElementIndex]);
			}
			NewSplineMesh->UpdateMesh();
			NewSplineMesh->AttachToComponent(this, FAttachmentTransformRules::SnapToTargetIncludingScale);
			NewSplineMesh->SetRelativeTransform(FTransform::Identity);
			NewSplineMesh->SetWorldScale3D(SplineScale3D);

			SplineComponents.AddUnique(NewSplineMesh);
		}
	}

	const FName CylinderMeshName = FName(*FString::Printf(TEXT("%s_%s"), *GetName(), TEXT("CylinderMesh")));
	PointMeshComponent = NewObject<UStaticMeshComponent>(GetOwner(), CylinderMeshName);
	PointMeshComponent->SetMobility(EComponentMobility::Movable);
	PointMeshComponent->SetOnlyOwnerSee(bOnlyOwnerSee);
	PointMeshComponent->SetOwnerNoSee(bOwnerNoSee);
	PointMeshComponent->SetCastShadow(false);
	PointMeshComponent->SetCanEverAffectNavigation(false);
	PointMeshComponent->SetCollisionProfileName(TEXT("NoCollision"));
	PointMeshComponent->SetStaticMesh(PointMesh);
	for (int32 ElementIndex = 0; ElementIndex < PointMaterials.Num(); ElementIndex++)
	{
		PointMeshComponent->SetMaterial(ElementIndex, PointMaterials[ElementIndex]);
	}
	PointMeshComponent->SetWorldScale3D(PointScale3D);
	PointMeshComponent->RegisterComponentWithWorld(GetWorld());

	GetOwner()->AddOwnedComponent(PointMeshComponent);
}

void UPredictProjectileComponent::DeactivePredictProjectile()
{
	if (bIsActivePredict)
	{
		bIsActivePredict = false;

		for (USplineMeshComponent* SplineMeshComponent : SplineComponents)
		{
			if (SplineMeshComponent != nullptr)
			{
				SplineMeshComponent->SetHiddenInGame(true);
			}
		}

		if (PointMeshComponent != nullptr)
		{
			PointMeshComponent->SetHiddenInGame(true, true);
		}
	}
}

void UPredictProjectileComponent::DestroyPredictProjectile()
{
	bInitialed = false;

	for (class USplineMeshComponent* SplineMeshComponent : SplineComponents)
	{
		if (SplineMeshComponent)
		{
			SplineMeshComponent->SetHiddenInGame(true);
			SplineMeshComponent->DestroyComponent();
		}
	}

	SplineComponents.Empty();

	if (PointMeshComponent != nullptr)
	{
		PointMeshComponent->SetVisibility(false);
		PointMeshComponent->SetHiddenInGame(true);
		PointMeshComponent->DestroyComponent();
		PointMeshComponent = nullptr;
	}
}

void UPredictProjectileComponent::UpdatePredictProjectile(float DeltaTime)
{
	if (bHiddenInGame || !IsActive())
	{
		DeactivePredictProjectile();
	}
	else if (bInitialed)
	{
		bIsActivePredict = true;

		FPredictProjectilePathResult PredictResult;

		bool bIsProhibitNav = PredictProjectilePath(PredictResult, true);

		for (int32 Index = 0; Index < SplineComponents.Num(); Index++)
		{
			USplineMeshComponent* SplineComponent = SplineComponents[Index];
			if (SplineComponent == nullptr)
			{
				continue;
			}

			bool bHiddenSpline = true;
			if (Index < PredictResult.PathData.Num() - 1)
			{
				bHiddenSpline = false;

				const FTransform& ComponentTransform = GetComponentTransform();
				const FQuat& ComponentQuat = GetComponentQuat();

				FVector StartPostion = ComponentTransform.InverseTransformPosition(PredictResult.PathData[Index].Location) / SplineScale3D;
				FVector EndPostion = ComponentTransform.InverseTransformPosition(PredictResult.PathData[Index + 1].Location) / SplineScale3D;

				SplineComponent->SetStartAndEnd(StartPostion,
					Tangent,
					EndPostion,
					Tangent
				);
			}
			SplineComponent->SetHiddenInGame(bHiddenSpline);
		}

		if (PointMeshComponent != nullptr)
		{
			if (bIsProhibitNav)
			{
				PointMeshComponent->SetVectorParameterValueOnMaterials(ColorName, FVector(PreviewColor));
			}
			else
			{
				PointMeshComponent->SetVectorParameterValueOnMaterials(ColorName, FVector(ProhibitColor));
			}
		}

		if (PointMeshComponent != nullptr)
		{
			if (PredictResult.HitResult.bBlockingHit)
			{
				FVector DecalLocation = PredictResult.HitResult.ImpactPoint;

				PointMeshComponent->SetHiddenInGame(false, true);
				PointMeshComponent->SetWorldLocation(DecalLocation);

				if (bIsProhibitNav)
				{
					PointMeshComponent->SetWorldRotation(FRotator::ZeroRotator);
				}
				else
				{
					FRotator DecalRotation = PredictResult.HitResult.ImpactNormal.Rotation() + FRotator(90.f, 0.f, 0.f);
					PointMeshComponent->SetWorldRotation(DecalRotation);
				}
			}
			else
			{
				PointMeshComponent->SetHiddenInGame(true, true);
			}
		}

		if (bIsProhibitNav)
		{
			APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
			if (PlayerController != nullptr)
			{
				CurrentProjectDistance += FVector::Distance(PredictResult.HitResult.Location, LastPredictResult.HitResult.Location);
				if (ProjectForceFeedbackPlayDistance > 0 && CurrentProjectDistance >= ProjectForceFeedbackPlayDistance)
				{
					CurrentProjectDistance = 0.f;

					FForceFeedbackParameters ForceFeedbackParameters;
					ForceFeedbackParameters.Tag = TEXT("Project");
					ForceFeedbackParameters.bLooping = false;
					ForceFeedbackParameters.bIgnoreTimeDilation = false;
					ForceFeedbackParameters.bPlayWhilePaused = false;

					PlayerController->ClientPlayForceFeedback(ProjectForceFeedbackEffect, ForceFeedbackParameters);
				}

				AActor* AdsorptionActor = PredictResult.HitResult.GetActor();
				if (AdsorptionActor != nullptr && !AdsorptionActorClasses.Contains(AdsorptionActor->GetClass()))
				{
					AdsorptionActor = nullptr;
				}

				if (LastAdsorptionActor != AdsorptionActor)
				{
					LastAdsorptionActor = AdsorptionActor;

					FForceFeedbackParameters ForceFeedbackParameters;
					ForceFeedbackParameters.Tag = TEXT("Adsorption");
					ForceFeedbackParameters.bLooping = false;
					ForceFeedbackParameters.bIgnoreTimeDilation = false;
					ForceFeedbackParameters.bPlayWhilePaused = false;

					PlayerController->ClientPlayForceFeedback(AdsorptionForceFeedbackEffect, ForceFeedbackParameters);
				}
			}
		}

		LastPredictResult = PredictResult;
	}
}

bool UPredictProjectileComponent::PredictProjectilePath(FPredictProjectilePathResult& PredictResult, bool bTraceWithCollision)
{
	FPredictProjectilePathParams ProjectilePathParams;
	ProjectilePathParams.StartLocation = GetComponentLocation();
	ProjectilePathParams.LaunchVelocity = GetComponentRotation().Vector() * InitialSpeed;
	ProjectilePathParams.bTraceWithCollision = bTraceWithCollision;
	ProjectilePathParams.bTraceComplex = false;
	ProjectilePathParams.ProjectileRadius = Radius;
	ProjectilePathParams.TraceChannel = TraceChannel;
	ProjectilePathParams.SimFrequency = SimFrequency;
	ProjectilePathParams.MaxSimTime = MaxSimTime;
	ProjectilePathParams.OverrideGravityZ = GetWorld()->GetGravityZ() * GravityScale;

	bool bResult = false;

	if (UGameplayStatics::PredictProjectilePath(GetWorld(), ProjectilePathParams, PredictResult))
	{
		bResult = true;

		FHitResult& HitResult = PredictResult.HitResult;

		AActor* HitActor = HitResult.GetActor();
		if (HitActor && AdsorptionActorClasses.Contains(HitActor->GetClass()))
		{
			HitResult.ImpactPoint = HitActor->GetActorLocation();
		}

		if (bProjectPointToNavigation)
		{
			FNavLocation NavLocation;

			UNavigationSystemV1* NavigationSystem = Cast<UNavigationSystemV1>(GetWorld()->GetNavigationSystem());
			if (NavigationSystem && NavigationSystem->ProjectPointToNavigation(HitResult.ImpactPoint, NavLocation, FVector(ProjectNavExtend)))
			{
				HitResult.ImpactPoint = NavLocation.Location;
			}
			else
			{
				bResult = false;
			}
		}

		HitResult.ImpactPoint.Z -= ProjectNavLandOffsetZ;
	}

	return bResult;
}

void UPredictProjectileComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdatePredictProjectile(DeltaTime);
}

void UPredictProjectileComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
{
	DestroyPredictProjectile();

	Super::OnComponentDestroyed(bDestroyingHierarchy);
}

void UPredictProjectileComponent::Activate(bool bReset)
{
	Super::Activate(bReset);

	if (bReset)
	{
		DestroyPredictProjectile();
	}

	InitPredictProjectile();
}

void UPredictProjectileComponent::Deactivate()
{
	Super::Deactivate();

	DeactivePredictProjectile();
}