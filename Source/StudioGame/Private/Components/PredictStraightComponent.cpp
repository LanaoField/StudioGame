// Copyright 2018 CQUnreal, Inc. All Rights Reserved.

#include "Components/PredictStraightComponent.h"
#include "Components/WidgetComponent.h"
#include "Interfaces/InteractiveInterface.h"


#define DISPLAY_POSITION FVector(100.f, 0.f, 0.f)

UPredictStraightComponent::UPredictStraightComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, TraceChannel(ECollisionChannel::ECC_Visibility)
	, SphereRadius(5.f)
	, TraceDistance(500.f)
	, SpotScale(1.f)
	, bEnableInternalTrace(true)
{
	Mobility = EComponentMobility::Movable;
	bAutoActivate = true;
	bWantsInitializeComponent = true;

	SplineParams.StartPos = FVector::ZeroVector;
	SplineParams.StartTangent = FVector::ZeroVector;
	SplineParams.EndPos = DISPLAY_POSITION;
	SplineParams.EndTangent = FVector::ZeroVector;

	PrimaryComponentTick.bCanEverTick = true;
}

void UPredictStraightComponent::InitializeComponent()
{
	Super::InitializeComponent();

	InitPredictStraight();
}

void UPredictStraightComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdatePredictStraight(DeltaTime);
}

void UPredictStraightComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
{
	if (SpotMeshComponent != nullptr)
	{
		SpotMeshComponent->SetHiddenInGame(true, true);
		SpotMeshComponent->DestroyComponent();
		SpotMeshComponent = nullptr;
	}

	Super::OnComponentDestroyed(bDestroyingHierarchy);
}

void UPredictStraightComponent::Activate(bool bReset)
{
	Super::Activate(bReset);

	InitPredictStraight();

	SetHiddenInGame(false, true);
}

void UPredictStraightComponent::Deactivate()
{
	Super::Deactivate();

	if (bIsActivePredict)
	{
		bIsActivePredict = false;

		SetHiddenInGame(true, true);
	}
}

void UPredictStraightComponent::InitPredictStraight()
{
	SetHiddenInGame(!IsActive(), true);

	if (bInitialed || !IsActive())
	{
		return;
	}

	bInitialed = true;

	if (SpotMesh != nullptr)
	{
		const FName SpotMeshName = FName(*FString::Printf(TEXT("%s_%s"), *GetName(), TEXT("SpotMesh")));

		SpotMeshComponent = NewObject<UStaticMeshComponent>(GetOwner(), SpotMeshName);
		if (SpotMeshComponent != nullptr)
		{
			GetOwner()->AddOwnedComponent(SpotMeshComponent);

			SpotMeshComponent->CastShadow = false;
			SpotMeshComponent->SetMobility(Mobility);
			SpotMeshComponent->SetOnlyOwnerSee(bOnlyOwnerSee);
			SpotMeshComponent->SetOwnerNoSee(bOwnerNoSee);
			SpotMeshComponent->SetTickableWhenPaused(PrimaryComponentTick.bTickEvenWhenPaused);
			SpotMeshComponent->SetCanEverAffectNavigation(false);
			SpotMeshComponent->SetCollisionProfileName(TEXT("NoCollision"));
			SpotMeshComponent->AttachToComponent(this, FAttachmentTransformRules::SnapToTargetIncludingScale);
			SpotMeshComponent->RegisterComponentWithWorld(GetWorld());
			SpotMeshComponent->SetStaticMesh(SpotMesh);
			SpotMeshComponent->SetRelativeScale3D(SpotScale);
			SpotMeshComponent->SetRelativeLocation(DISPLAY_POSITION);
			for (int32 Index = 0; Index < SpotMaterials.Num(); Index++)
			{
				SpotMeshComponent->SetMaterial(Index, SpotMaterials[Index]);
			}
		}
	}
}

void UPredictStraightComponent::UpdatePredictStraight(float DeltaTime)
{
	if (bInitialed && IsActive())
	{
		bIsActivePredict = true;

		if (!bEnableInternalTrace)
		{
			return;
		}

		FVector Start = GetComponentLocation();
		FVector End = GetComponentLocation() + GetComponentRotation().Vector() * TraceDistance;

		FCollisionQueryParams CollisionQueryParams;
		CollisionQueryParams.bTraceComplex = true;

		APawn* Instigator = GetOwner() ? GetOwner()->GetInstigator() : nullptr;
		CollisionQueryParams.AddIgnoredActor(Instigator);

		FHitResult HitResult;
		if (bIsSweepSphere)
		{
			GetWorld()->SweepSingleByChannel(HitResult, Start, End, FQuat::Identity, TraceChannel, FCollisionShape::MakeSphere(SphereRadius), CollisionQueryParams);
		}
		else
		{
			GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, TraceChannel, CollisionQueryParams);
		}
		
		SetLastHitResult(HitResult);
	}
}

void UPredictStraightComponent::SetLastHitResult(const FHitResult& HitResult)
{
	LastHitResult = HitResult;

	HoveredComponent = HitResult.GetComponent();

	HoveredWidgetComponent = Cast<UWidgetComponent>(HoveredComponent);
	if (HoveredWidgetComponent != nullptr)
	{
		if (HoveredWidgetComponent->GetGeometryMode() == EWidgetGeometryMode::Cylinder)
		{
			const FTransform WorldTransform = GetComponentTransform();
			FVector WorldDirection = WorldTransform.GetUnitAxis(EAxis::X);

			TTuple<FVector, FVector2D> CylinderHitLocation = HoveredWidgetComponent->GetCylinderHitLocation(LastHitResult.ImpactPoint, WorldDirection);
			LastHitResult.ImpactPoint = CylinderHitLocation.Get<0>();
		}
		else
		{
			ensure(HoveredWidgetComponent->GetGeometryMode() == EWidgetGeometryMode::Plane);
			//HoveredWidgetComponent->GetLocalHitLocation(LastHitResult.ImpactPoint, LastHitResult.Location);
		}
	}

	if (HitResult.GetActor() && HitResult.GetActor()->Implements<UInteractiveInterface>())
	{
		if (HitResult.GetComponent() == InteractiveComponent)
		{
			IInteractiveInterface::Execute_Focus(HitResult.GetActor(), GetOwner()->GetInstigator(), this, HitResult.GetComponent());
		}
		else
		{
			if (InteractiveComponent != nullptr)
			{
				IInteractiveInterface::Execute_EndFocus(InteractiveComponent->GetOwner(), GetOwner()->GetInstigator(), this, InteractiveComponent);
			}

			InteractiveComponent = HitResult.GetComponent();

			IInteractiveInterface::Execute_BeginFocus(HitResult.GetActor(), GetOwner()->GetInstigator(), this, HitResult.GetComponent());
		}
	}
	else
	{
		if (InteractiveComponent != nullptr)
		{
			IInteractiveInterface::Execute_EndFocus(InteractiveComponent->GetOwner(), GetOwner()->GetInstigator(), this, InteractiveComponent);
		}		

		InteractiveComponent = nullptr;
	}

	if (SpotMeshComponent != nullptr)
	{
		bool bIsHidden = !HitResult.bBlockingHit;
		if (!bIsHidden)
		{
			APawn* Instigator = GetOwner() ? GetOwner()->GetInstigator() : nullptr;
			if (bOwnerNoSee && Instigator && Instigator->IsLocallyControlled())
			{
				bIsHidden = true;
			}

			if (bOnlyOwnerSee && (Instigator == nullptr || !Instigator->IsLocallyControlled()))
			{
				bIsHidden = true;
			}
		}

		SpotMeshComponent->SetHiddenInGame(bIsHidden);

		if (!bIsHidden)
		{
			const float MinDistance = 5.0f;
			const float ScreenDistance = 10000.f;
			const FVector NewLocation = HitResult.ImpactPoint;
			const FRotator NewRotation = FRotator(0);
			const FVector NewScale = FMath::Max(1.f, FMath::Clamp(HitResult.Distance, MinDistance, ScreenDistance) * 0.002f) * SpotScale;

			FTransform NewTransform = FTransform(NewRotation, NewLocation, NewScale);
			SpotMeshComponent->SetWorldTransform(NewTransform, false, nullptr, ETeleportType::TeleportPhysics);

			//UE_LOG(LogTemp, Log, TEXT("Distance[%.2f] NewScale[%s] HitActor[%s] HitComponent[%s] Onwer[%s]"), HitResult.Distance, *NewScale.ToString(), *GetNameSafe(HitResult.GetActor()), *GetNameSafe(HitResult.GetComponent()), *GetNameSafe(GetOwner()));
		}
	}

	FVector HitPosition = HitResult.bBlockingHit ? HitResult.ImpactPoint : HitResult.TraceEnd;
	FVector EndPosition = FTransform(GetComponentQuat(), HitPosition).GetRelativeTransform(GetComponentTransform()).GetLocation();

	SetEndPosition(EndPosition);
}

void UPredictStraightComponent::SetEnableInternalTrace(bool bEnable)
{
	bEnableInternalTrace = bEnable;
}

void UPredictStraightComponent::PressPointerKey(FKey Key)
{
	if (HoveredComponent != nullptr)
	{
		HoveredComponent->DispatchOnClicked(Key);
	}

	if (InteractiveComponent != nullptr)
	{
		IInteractiveInterface::Execute_PresseInteraction(InteractiveComponent->GetOwner(), GetOwner()->GetInstigator(), this, InteractiveComponent, Key);
	}
}

void UPredictStraightComponent::ReleasePointerKey(FKey Key)
{
	if (HoveredComponent != nullptr)
	{
		HoveredComponent->DispatchOnReleased(Key);
	}

	if (InteractiveComponent != nullptr)
	{
		IInteractiveInterface::Execute_ReleaseInteraction(InteractiveComponent->GetOwner(), GetOwner()->GetInstigator(), this, InteractiveComponent, Key);
	}
}