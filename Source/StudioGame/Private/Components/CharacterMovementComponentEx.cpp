// Copyright 2022 CQUnreal. All Rights Reserved.

#include "Components/CharacterMovementComponentEx.h"
#include "Components/CapsuleComponent.h"
#include "Interfaces/CharacterMovementExInterce.h"
#include "Engine/EngineTypes.h"

namespace CharacterMovementComponentExStatics
{
	static const FName ProneTraceName = FName(TEXT("ProneTrace"));
	static const FName CrouchTraceName = FName(TEXT("CrouchTrace"));
}

DECLARE_CYCLE_STAT(TEXT("Char ClientUpdatePositionAfterServerUpdate"), STAT_CharacterMovementClientUpdatePositionAfterServerUpdate, STATGROUP_Character);

UCharacterMovementComponentEx::UCharacterMovementComponentEx(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer)
	, StartingSpeedRate(0.f)
	, MaxWalkSpeedProned(MaxWalkSpeedCrouched * 0.5f)
	, MaxWalkSpeedStaticing(MaxWalkSpeed * 0.5f)
	, MaxWalkSpeedSprinting(MaxWalkSpeed * 1.5f)
	, SprintAstrictAngle(45.f)
	, AimingDecelerationRate(0.5f)
	, PronedHalfHeight(32.f)
{
	bAlwaysCheckFloor = false;

	NetworkSmoothingMode = ENetworkSmoothingMode::Linear;

	MaxSimulationIterations = 2;
	MaxSimulationTimeStep = 0.5f;
}

void UCharacterMovementComponentEx::PostLoad()
{
	Super::PostLoad();

	CharacterMovementExInterce = Cast<ACharacter>(PawnOwner);
}

void UCharacterMovementComponentEx::PerformMovement(float DeltaTime)
{
	Super::PerformMovement(DeltaTime);
}

void UCharacterMovementComponentEx::SimulateMovement(float DeltaTime)
{
	Super::SimulateMovement(DeltaTime);

	//if (bNetworkMovementModeChanged)
	//{
	//	bNetworkMovementModeChanged = false;
	//	ApplyNetworkMovementMode(CharacterOwner->GetReplicatedMovementMode());
	//}

	//UpdateComponentVelocity();
	//ClearAccumulatedForces();
	//bNetworkUpdateReceived = false;
	//bJustTeleported = false;

	//LastUpdateLocation = UpdatedComponent ? UpdatedComponent->GetComponentLocation() : FVector::ZeroVector;
	//LastUpdateRotation = UpdatedComponent ? UpdatedComponent->GetComponentQuat() : FQuat::Identity;
	//LastUpdateVelocity = Velocity;
}

void UCharacterMovementComponentEx::SetUpdatedComponent(USceneComponent* NewUpdatedComponent)
{
	if (NewUpdatedComponent)
	{
		CharacterMovementExInterce = Cast<ACharacter>(NewUpdatedComponent->GetOwner());
	}

	Super::SetUpdatedComponent(NewUpdatedComponent);
}

bool UCharacterMovementComponentEx::HasValidData() const
{
	return Super::HasValidData() && CharacterMovementExInterce != nullptr;
}

void UCharacterMovementComponentEx::SetSpeedRate(float NewSpeedRate)
{
	SpeedRate = FMath::Max(0.f, NewSpeedRate);
}

void UCharacterMovementComponentEx::CalcVelocity(float DeltaTime, float Friction, bool bFluid, float BrakingDeceleration)
{
	// Do not update velocity when using root motion or when SimulatedProxy and not simulating root motion - SimulatedProxy are repped their Velocity
	if (!HasValidData() || HasAnimRootMotion() || DeltaTime < MIN_TICK_TIME || (CharacterOwner && CharacterOwner->GetLocalRole() == ROLE_SimulatedProxy && !bWasSimulatingRootMotion))
	{
		return;
	}

	Friction = FMath::Max(0.f, Friction);
	const float MaxAccel = GetMaxAcceleration();
	float MaxSpeed = GetMaxSpeed();

	// Check if path following requested movement
	bool bZeroRequestedAcceleration = true;
	FVector RequestedAcceleration = FVector::ZeroVector;
	float RequestedSpeed = 0.0f;
	if (ApplyRequestedMove(DeltaTime, MaxAccel, MaxSpeed, Friction, BrakingDeceleration, RequestedAcceleration, RequestedSpeed))
	{
		bZeroRequestedAcceleration = false;
	}

	if (bForceMaxAccel)
	{
		// Force acceleration at full speed.
		// In consideration order for direction: Acceleration, then Velocity, then Pawn's rotation.
		if (Acceleration.SizeSquared() > SMALL_NUMBER)
		{
			Acceleration = Acceleration.GetSafeNormal() * MaxAccel;
		}
		else
		{
			Acceleration = MaxAccel * (Velocity.SizeSquared() < SMALL_NUMBER ? UpdatedComponent->GetForwardVector() : Velocity.GetSafeNormal());
		}

		AnalogInputModifier = 1.f;
	}

	// Path following above didn't care about the analog modifier, but we do for everything else below, so get the fully modified value.
	// Use max of requested speed and max speed if we modified the speed in ApplyRequestedMove above.
	const float MaxInputSpeed = FMath::Max(MaxSpeed * AnalogInputModifier, GetMinAnalogSpeed());
	MaxSpeed = FMath::Max(RequestedSpeed, MaxInputSpeed);

	// Apply braking or deceleration
	const bool bZeroAcceleration = Acceleration.IsZero();
	const bool bVelocityOverMax = IsExceedingMaxSpeed(MaxSpeed);

	// Only apply braking if there is no acceleration, or we are over our max speed and need to slow down to it.
	if ((bZeroAcceleration && bZeroRequestedAcceleration) || bVelocityOverMax)
	{
		const FVector OldVelocity = Velocity;

		const float ActualBrakingFriction = (bUseSeparateBrakingFriction ? BrakingFriction : Friction);
		ApplyVelocityBraking(DeltaTime, ActualBrakingFriction, BrakingDeceleration);

		// Don't allow braking to lower us below max speed if we started above it.
		if (bVelocityOverMax && Velocity.SizeSquared() < FMath::Square(MaxSpeed) && FVector::DotProduct(Acceleration, OldVelocity) > 0.0f)
		{
			Velocity = OldVelocity.GetSafeNormal() * MaxSpeed;
		}
	}
	else if (!bZeroAcceleration)
	{
		// Friction affects our ability to change direction. This is only done for input acceleration, not path following.
		const FVector AccelDir = Acceleration.GetSafeNormal();
		const float VelSize = Velocity.Size();
		Velocity = Velocity - (Velocity - AccelDir * VelSize) * FMath::Min(DeltaTime * Friction, 1.f);
	}

	// Apply fluid friction
	if (bFluid)
	{
		Velocity = Velocity * (1.f - FMath::Min(Friction * DeltaTime, 1.f));
	}

	// Apply input acceleration
	if (!bZeroAcceleration)
	{
		if (Velocity.IsZero())
		{
			Velocity = Acceleration.GetSafeNormal() * GetMaxSpeed() * StartingSpeedRate;
		}

		const float NewMaxInputSpeed = IsExceedingMaxSpeed(MaxInputSpeed) ? Velocity.Size() : MaxInputSpeed;
		Velocity += Acceleration * DeltaTime;
		Velocity = Velocity.GetClampedToMaxSize(NewMaxInputSpeed);
	}

	// Apply additional requested acceleration
	if (!bZeroRequestedAcceleration)
	{
		const float NewMaxRequestedSpeed = IsExceedingMaxSpeed(RequestedSpeed) ? Velocity.Size() : RequestedSpeed;
		Velocity += RequestedAcceleration * DeltaTime;
		Velocity = Velocity.GetClampedToMaxSize(NewMaxRequestedSpeed);
	}

	if (bUseRVOAvoidance)
	{
		CalcAvoidanceVelocity(DeltaTime);
	}
}

float UCharacterMovementComponentEx::GetMaxSpeed() const
{
	if (bIsSwitchingState)
	{
		return 0.f;
	}

	float MaxSpeed = 0.f;
	switch (MovementMode)
	{
	case MOVE_Walking:
	case MOVE_NavWalking:
		if (IsProning())
		{
			MaxSpeed = MaxWalkSpeedProned;
		}
		else if (IsCrouching())
		{
			MaxSpeed = MaxWalkSpeedCrouched;
		}
		else if (IsStaticing())
		{
			MaxSpeed = MaxWalkSpeedStaticing;
		}
		else if (IsSprinting())
		{
			const float DotProduct = FVector::DotProduct(Velocity.GetSafeNormal(), CharacterOwner->GetActorForwardVector());
			if (FMath::DegreesToRadians(SprintAstrictAngle) < DotProduct)
			{
				MaxSpeed = MaxWalkSpeedSprinting;
			}
			else
			{
				MaxSpeed = MaxWalkSpeed;
			}
		}
		else
		{
			MaxSpeed = MaxWalkSpeed;
		}

		if (IsAiming())
		{
			MaxSpeed *= AimingDecelerationRate;
		}
		break;
	case MOVE_Falling:
		MaxSpeed = MaxWalkSpeed;
		break;
	case MOVE_Swimming:
		MaxSpeed = MaxSwimSpeed;
		break;
	case MOVE_Flying:
		MaxSpeed = MaxFlySpeed;
		break;
	case MOVE_Custom:
		MaxSpeed = MaxCustomMovementSpeed;
		break;
	case MOVE_None:
	default:
		return 0.f;
	}

	return MaxSpeed * SpeedRate;
}

void UCharacterMovementComponentEx::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	if (!HasValidData())
	{
		return;
	}

	// Update collision settings if needed
	if (MovementMode == MOVE_NavWalking)
	{
		SetGroundMovementMode(MovementMode);

		// Walking uses only XY velocity
		Velocity.Z = 0.f;
		SetNavWalkingPhysics(true);
	}
	else if (PreviousMovementMode == MOVE_NavWalking)
	{
		if (MovementMode == DefaultLandMovementMode || IsWalking())
		{
			const bool bSucceeded = TryToLeaveNavWalking();
			if (!bSucceeded)
			{
				return;
			}
		}
		else
		{
			SetNavWalkingPhysics(false);
		}
	}

	// React to changes in the movement mode.
	if (MovementMode == MOVE_Walking)
	{
		// Walking uses only XY velocity, and must be on a walkable floor, with a Base.
		Velocity.Z = 0.f;
		bProneMaintainsBaseLocation = true;
		bCrouchMaintainsBaseLocation = true;
		SetGroundMovementMode(MovementMode);

		// make sure we update our new floor/base on initial entry of the walking physics
		FindFloor(UpdatedComponent->GetComponentLocation(), CurrentFloor, false);
		AdjustFloorHeight();
		SetBaseFromFloor(CurrentFloor);
	}
	else
	{
		CurrentFloor.Clear();
		bCrouchMaintainsBaseLocation = false;
		bProneMaintainsBaseLocation = false;

		if (MovementMode == MOVE_Falling)
		{
			Velocity += GetImpartedMovementBaseVelocity();
			CharacterOwner->Falling();
		}

		SetBase(NULL);

		if (MovementMode == MOVE_None)
		{
			// Kill velocity and clear queued up events
			StopMovementKeepPathing();
			CharacterOwner->ResetJumpState();
			ClearAccumulatedForces();
		}
	}

	if (MovementMode == MOVE_Falling && PreviousMovementMode != MOVE_Falling)
	{
		IPathFollowingAgentInterface* PFAgent = GetPathFollowingAgent();
		if (PFAgent)
		{
			PFAgent->OnStartedFalling();
		}
	}

	CharacterOwner->OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);
	ensureMsgf(GetGroundMovementMode() == MOVE_Walking || GetGroundMovementMode() == MOVE_NavWalking, TEXT("Invalid GroundMovementMode %d. MovementMode: %d, PreviousMovementMode: %d"), GetGroundMovementMode(), MovementMode.GetValue(), PreviousMovementMode);
}

void UCharacterMovementComponentEx::UpdateCharacterStateBeforeMovement(float DeltaSeconds)
{
	// Check for a change in crouch state. Players toggle crouch by changing bWantsToCrouch.
	const bool bIsCrouching = IsCrouching();
	const bool bIsProning = IsProning();
	if (bIsProning && (!bWantsToProne || !CanProneInCurrentState()))
	{
		UnProne(false);
	}
	else if (!bIsProning && bWantsToProne && CanProneInCurrentState())
	{
		Prone(false);
	}
	else if (!bIsProning && bIsCrouching && (!bWantsToCrouch || !CanCrouchInCurrentState()))
	{
		UnCrouch(false);
	}
	else if (!bIsProning && !bIsCrouching && bWantsToCrouch && CanCrouchInCurrentState())
	{
		Crouch(false);
	}

	const bool bIsSprinting = IsSprinting();
	if (bIsSprinting && (!bWantsToSprint || !CanSprintInCurrentState()))
	{
		UnSprint(false);
	}
	else if (!bIsSprinting && bWantsToSprint && CanSprintInCurrentState())
	{
		Sprint(false);
	}

	const bool bIsAiming = IsAiming();
	if (bIsAiming && (!bWantsToAim || !CanAimInCurrentState()))
	{
		UnAim(false);
	}
	else if (!bIsAiming && bWantsToAim && CanAimInCurrentState())
	{
		Aim(false);
	}
}

void UCharacterMovementComponentEx::UpdateCharacterStateAfterMovement(float DeltaSeconds)
{
	Super::UpdateCharacterStateAfterMovement(DeltaSeconds);

	// Unprone if no longer allowed to be proned
	if (IsProning() && !CanProneInCurrentState())
	{
		UnProne(false);
	}
}

bool UCharacterMovementComponentEx::CanProneInCurrentState() const
{
	return IsMovingOnGround();
}

void UCharacterMovementComponentEx::Prone(bool bClientSimulation)
{
	if (!HasValidData())
	{
		return;
	}

	if (!CanProneInCurrentState())
	{
		return;
	}

	// See if collision is already at desired size.
	if (CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() == PronedHalfHeight)
	{
		if (!bClientSimulation)
		{
			CharacterMovementExInterce->SetIsCrouched(false);
			CharacterMovementExInterce->SetIsCrouched(false);
		}
		CharacterMovementExInterce->OnStartProne(0.f, 0.f);
		return;
	}

	if (bClientSimulation && CharacterMovementExInterce->GetCharacterOwner()->GetLocalRole() == ROLE_SimulatedProxy)
	{
		// restore collision size before Proning
		ACharacter* DefaultCharacter = CharacterOwner->GetClass()->GetDefaultObject<ACharacter>();
		CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleRadius(), DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight());
		bShrinkProxyCapsule = true;
	}

	// Change collision size to proning dimensions
	const float ComponentScale = CharacterOwner->GetCapsuleComponent()->GetShapeScale();
	const float OldUnscaledHalfHeight = CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	const float OldUnscaledRadius = CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleRadius();
	// Height is not allowed to be smaller than radius.
	const float ClampedPronedHalfHeight = FMath::Max3(0.f, OldUnscaledRadius, PronedHalfHeight);
	CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(OldUnscaledRadius, ClampedPronedHalfHeight);
	float HalfHeightAdjust = (OldUnscaledHalfHeight - ClampedPronedHalfHeight);
	float ScaledHalfHeightAdjust = HalfHeightAdjust * ComponentScale;

	if (!bClientSimulation)
	{
		// Proning to a larger height? (this is rare)
		if (ClampedPronedHalfHeight > OldUnscaledHalfHeight)
		{
			FCollisionQueryParams CapsuleParams(CharacterMovementComponentExStatics::ProneTraceName, false, CharacterOwner);
			FCollisionResponseParams ResponseParam;
			InitCollisionParams(CapsuleParams, ResponseParam);
			const bool bEncroached = GetWorld()->OverlapBlockingTestByChannel(UpdatedComponent->GetComponentLocation() - FVector(0.f, 0.f, ScaledHalfHeightAdjust), FQuat::Identity,
				UpdatedComponent->GetCollisionObjectType(), GetPawnCapsuleCollisionShape(SHRINK_None), CapsuleParams, ResponseParam);

			// If encroached, cancel
			if (bEncroached)
			{
				CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(OldUnscaledRadius, OldUnscaledHalfHeight);
				return;
			}
		}

		if (bProneMaintainsBaseLocation)
		{
			// Intentionally not using MoveUpdatedComponent, where a horizontal plane constraint would prevent the base of the capsule from staying at the same spot.
			UpdatedComponent->MoveComponent(FVector(0.f, 0.f, -ScaledHalfHeightAdjust), UpdatedComponent->GetComponentQuat(), true, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
		}

		CharacterMovementExInterce->SetIsCrouched(false);
		CharacterMovementExInterce->SetIsProne(true);
	}

	bForceNextFloorCheck = true;

	// OnStartProne takes the change from the Default size, not the current one (though they are usually the same).
	const float MeshAdjust = ScaledHalfHeightAdjust;
	ACharacter* DefaultCharacter = CharacterOwner->GetClass()->GetDefaultObject<ACharacter>();
	HalfHeightAdjust = (DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() - ClampedPronedHalfHeight);
	ScaledHalfHeightAdjust = HalfHeightAdjust * ComponentScale;

	AdjustProxyCapsuleSize();
	CharacterMovementExInterce->OnStartProne(HalfHeightAdjust, ScaledHalfHeightAdjust);

	// Don't smooth this change in mesh position
	if (bClientSimulation && CharacterOwner->GetLocalRole() == ROLE_SimulatedProxy)
	{
		FNetworkPredictionData_Client_Character* ClientData = GetPredictionData_Client_Character();
		if (ClientData && ClientData->MeshTranslationOffset.Z != 0.f)
		{
			ClientData->MeshTranslationOffset -= FVector(0.f, 0.f, MeshAdjust);
			ClientData->OriginalMeshTranslationOffset = ClientData->MeshTranslationOffset;
		}
	}
}

void UCharacterMovementComponentEx::UnProne(bool bClientSimulation)
{
	if (!HasValidData())
	{
		return;
	}

	ACharacter* DefaultCharacter = CharacterOwner->GetClass()->GetDefaultObject<ACharacter>();

	// See if collision is already at desired size.
	if (CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() == DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight())
	{
		if (!bClientSimulation)
		{
			CharacterMovementExInterce->SetIsProne(false);
		}
		CharacterMovementExInterce->OnEndProne(0.f, 0.f);
		return;
	}

	const float CurrentPronedHalfHeight = CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	const float ComponentScale = CharacterOwner->GetCapsuleComponent()->GetShapeScale();
	const float OldUnscaledHalfHeight = CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	const float HalfHeightAdjust = DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() - OldUnscaledHalfHeight;
	const float ScaledHalfHeightAdjust = HalfHeightAdjust * ComponentScale;
	const FVector PawnLocation = UpdatedComponent->GetComponentLocation();

	// Grow to unproned size.
	check(CharacterOwner->GetCapsuleComponent());

	if (!bClientSimulation)
	{
		// Try to stay in place and see if the larger capsule fits. We use a slightly taller capsule to avoid penetration.
		const float SweepInflation = KINDA_SMALL_NUMBER * 10.f;
		FCollisionQueryParams CapsuleParams(CharacterMovementComponentExStatics::ProneTraceName, false, CharacterOwner);
		FCollisionResponseParams ResponseParam;
		InitCollisionParams(CapsuleParams, ResponseParam);

		// Compensate for the difference between current capsule size and standing size
		const FCollisionShape StandingCapsuleShape = GetPawnCapsuleCollisionShape(SHRINK_HeightCustom, -SweepInflation - ScaledHalfHeightAdjust); // Shrink by negative amount, so actually grow it.
		const ECollisionChannel CollisionChannel = UpdatedComponent->GetCollisionObjectType();
		bool bEncroached = true;

		if (!bProneMaintainsBaseLocation)
		{
			// Expand in place
			bEncroached = GetWorld()->OverlapBlockingTestByChannel(PawnLocation, FQuat::Identity, CollisionChannel, StandingCapsuleShape, CapsuleParams, ResponseParam);

			if (bEncroached)
			{
				// Try adjusting capsule position to see if we can avoid encroachment.
				if (ScaledHalfHeightAdjust > 0.f)
				{
					// Shrink to a short capsule, sweep down to base to find where that would hit something, and then try to stand up from there.
					float PawnRadius, PawnHalfHeight;
					CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleSize(PawnRadius, PawnHalfHeight);
					const float ShrinkHalfHeight = PawnHalfHeight - PawnRadius;
					const float TraceDist = PawnHalfHeight - ShrinkHalfHeight;
					const FVector Down = FVector(0.f, 0.f, -TraceDist);

					FHitResult Hit(1.f);
					const FCollisionShape ShortCapsuleShape = GetPawnCapsuleCollisionShape(SHRINK_HeightCustom, ShrinkHalfHeight);
					const bool bBlockingHit = GetWorld()->SweepSingleByChannel(Hit, PawnLocation, PawnLocation + Down, FQuat::Identity, CollisionChannel, ShortCapsuleShape, CapsuleParams);
					if (Hit.bStartPenetrating)
					{
						bEncroached = true;
					}
					else
					{
						// Compute where the base of the sweep ended up, and see if we can stand there
						const float DistanceToBase = (Hit.Time * TraceDist) + ShortCapsuleShape.Capsule.HalfHeight;
						const FVector NewLoc = FVector(PawnLocation.X, PawnLocation.Y, PawnLocation.Z - DistanceToBase + PawnHalfHeight + SweepInflation + MIN_FLOOR_DIST / 2.f);
						bEncroached = GetWorld()->OverlapBlockingTestByChannel(NewLoc, FQuat::Identity, CollisionChannel, StandingCapsuleShape, CapsuleParams, ResponseParam);
						if (!bEncroached)
						{
							// Intentionally not using MoveUpdatedComponent, where a horizontal plane constraint would prevent the base of the capsule from staying at the same spot.
							UpdatedComponent->MoveComponent(NewLoc - PawnLocation, UpdatedComponent->GetComponentQuat(), false, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
						}
					}
				}
			}
		}
		else
		{
			// Expand while keeping base location the same.
			FVector StandingLocation = PawnLocation + FVector(0.f, 0.f, StandingCapsuleShape.GetCapsuleHalfHeight() - CurrentPronedHalfHeight);
			bEncroached = GetWorld()->OverlapBlockingTestByChannel(StandingLocation, FQuat::Identity, CollisionChannel, StandingCapsuleShape, CapsuleParams, ResponseParam);

			if (bEncroached)
			{
				if (IsMovingOnGround())
				{
					// Something might be just barely overhead, try moving down closer to the floor to avoid it.
					const float MinFloorDist = KINDA_SMALL_NUMBER * 10.f;
					if (CurrentFloor.bBlockingHit && CurrentFloor.FloorDist > MinFloorDist)
					{
						StandingLocation.Z -= CurrentFloor.FloorDist - MinFloorDist;
						bEncroached = GetWorld()->OverlapBlockingTestByChannel(StandingLocation, FQuat::Identity, CollisionChannel, StandingCapsuleShape, CapsuleParams, ResponseParam);
					}
				}
			}

			if (!bEncroached)
			{
				// Commit the change in location.
				UpdatedComponent->MoveComponent(StandingLocation - PawnLocation, UpdatedComponent->GetComponentQuat(), false, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
				bForceNextFloorCheck = true;
			}
		}

		// If still encroached then abort.
		if (bEncroached)
		{
			return;
		}

		CharacterMovementExInterce->SetIsProne(false);
	}
	else
	{
		bShrinkProxyCapsule = true;
	}

	// Now call SetCapsuleSize() to cause touch/untouch events and actually grow the capsule
	CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleRadius(), DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight(), true);

	const float MeshAdjust = ScaledHalfHeightAdjust;
	AdjustProxyCapsuleSize();
	CharacterMovementExInterce->OnEndProne(HalfHeightAdjust, ScaledHalfHeightAdjust);

	// Don't smooth this change in mesh position
	if (bClientSimulation && CharacterOwner->GetLocalRole() == ROLE_SimulatedProxy)
	{
		FNetworkPredictionData_Client_Character* ClientData = GetPredictionData_Client_Character();
		if (ClientData && ClientData->MeshTranslationOffset.Z != 0.f)
		{
			ClientData->MeshTranslationOffset += FVector(0.f, 0.f, MeshAdjust);
			ClientData->OriginalMeshTranslationOffset = ClientData->MeshTranslationOffset;
		}
	}
}

void UCharacterMovementComponentEx::Crouch(bool bClientSimulation)
{
	if (!HasValidData())
	{
		return;
	}

	if (!CanCrouchInCurrentState())
	{
		return;
	}

	// See if collision is already at desired size.
#if ENGINE_MAJOR_VERSION > 4
	if (CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() == GetCrouchedHalfHeight())
#else
	if (CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() == CrouchedHalfHeight)
#endif
	{
		if (!bClientSimulation)
		{
			CharacterMovementExInterce->SetIsCrouched(true);
			CharacterMovementExInterce->SetIsProne(false);
		}
		CharacterOwner->OnStartCrouch(0.f, 0.f);
		return;
	}

	if (bClientSimulation && CharacterOwner->GetLocalRole() == ROLE_SimulatedProxy)
	{
		// restore collision size before crouching
		ACharacter* DefaultCharacter = CharacterOwner->GetClass()->GetDefaultObject<ACharacter>();
		CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleRadius(), DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight());
		bShrinkProxyCapsule = true;
	}

	// Change collision size to crouching dimensions
	const float ComponentScale = CharacterOwner->GetCapsuleComponent()->GetShapeScale();
	const float OldUnscaledHalfHeight = CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	const float OldUnscaledRadius = CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleRadius();
	// Height is not allowed to be smaller than radius.
#if ENGINE_MAJOR_VERSION > 4
	const float ClampedCrouchedHalfHeight = FMath::Max3(0.f, OldUnscaledRadius, GetCrouchedHalfHeight());
#else
	const float ClampedCrouchedHalfHeight = FMath::Max3(0.f, OldUnscaledRadius, CrouchedHalfHeight);
#endif
	CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(OldUnscaledRadius, ClampedCrouchedHalfHeight);
	float HalfHeightAdjust = (OldUnscaledHalfHeight - ClampedCrouchedHalfHeight);
	float ScaledHalfHeightAdjust = HalfHeightAdjust * ComponentScale;

	if (!bClientSimulation)
	{
		// Crouching to a larger height? (this is rare)
		if (ClampedCrouchedHalfHeight > OldUnscaledHalfHeight)
		{
			FCollisionQueryParams CapsuleParams(CharacterMovementComponentExStatics::CrouchTraceName, false, CharacterOwner);
			FCollisionResponseParams ResponseParam;
			InitCollisionParams(CapsuleParams, ResponseParam);
			const bool bEncroached = GetWorld()->OverlapBlockingTestByChannel(UpdatedComponent->GetComponentLocation() - FVector(0.f, 0.f, ScaledHalfHeightAdjust), FQuat::Identity,
				UpdatedComponent->GetCollisionObjectType(), GetPawnCapsuleCollisionShape(SHRINK_None), CapsuleParams, ResponseParam);

			// If encroached, cancel
			if (bEncroached)
			{
				CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(OldUnscaledRadius, OldUnscaledHalfHeight);
				return;
			}
		}

		if (bCrouchMaintainsBaseLocation)
		{
			// Intentionally not using MoveUpdatedComponent, where a horizontal plane constraint would prevent the base of the capsule from staying at the same spot.
			UpdatedComponent->MoveComponent(FVector(0.f, 0.f, -ScaledHalfHeightAdjust), UpdatedComponent->GetComponentQuat(), true, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
		}

		CharacterMovementExInterce->SetIsCrouched(true);
		CharacterMovementExInterce->SetIsProne(false);
	}

	bForceNextFloorCheck = true;

	// OnStartCrouch takes the change from the Default size, not the current one (though they are usually the same).
	const float MeshAdjust = ScaledHalfHeightAdjust;
	ACharacter* DefaultCharacter = CharacterOwner->GetClass()->GetDefaultObject<ACharacter>();
	HalfHeightAdjust = (DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() - ClampedCrouchedHalfHeight);
	ScaledHalfHeightAdjust = HalfHeightAdjust * ComponentScale;

	AdjustProxyCapsuleSize();
	CharacterOwner->OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);

	// Don't smooth this change in mesh position
	if (bClientSimulation && CharacterOwner->GetLocalRole() == ROLE_SimulatedProxy)
	{
		FNetworkPredictionData_Client_Character* ClientData = GetPredictionData_Client_Character();
		if (ClientData && ClientData->MeshTranslationOffset.Z != 0.f)
		{
			ClientData->MeshTranslationOffset -= FVector(0.f, 0.f, MeshAdjust);
			ClientData->OriginalMeshTranslationOffset = ClientData->MeshTranslationOffset;
		}
	}
}

void UCharacterMovementComponentEx::UnCrouch(bool bClientSimulation)
{
	if (!HasValidData())
	{
		return;
	}

	ACharacter* DefaultCharacter = CharacterOwner->GetClass()->GetDefaultObject<ACharacter>();

	// See if collision is already at desired size.
	if (CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() == DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight())
	{
		if (!bClientSimulation)
		{
			CharacterOwner->bIsCrouched = false;
		}
		CharacterOwner->OnEndCrouch(0.f, 0.f);
		return;
	}

	const float CurrentCrouchedHalfHeight = CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	const float ComponentScale = CharacterOwner->GetCapsuleComponent()->GetShapeScale();
	const float OldUnscaledHalfHeight = CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	const float HalfHeightAdjust = DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() - OldUnscaledHalfHeight;
	const float ScaledHalfHeightAdjust = HalfHeightAdjust * ComponentScale;
	const FVector PawnLocation = UpdatedComponent->GetComponentLocation();

	// Grow to uncrouched size.
	check(CharacterOwner->GetCapsuleComponent());

	if (!bClientSimulation)
	{
		// Try to stay in place and see if the larger capsule fits. We use a slightly taller capsule to avoid penetration.
		const float SweepInflation = KINDA_SMALL_NUMBER * 10.f;
		FCollisionQueryParams CapsuleParams(CharacterMovementComponentExStatics::CrouchTraceName, false, CharacterOwner);
		FCollisionResponseParams ResponseParam;
		InitCollisionParams(CapsuleParams, ResponseParam);

		// Compensate for the difference between current capsule size and standing size
		const FCollisionShape StandingCapsuleShape = GetPawnCapsuleCollisionShape(SHRINK_HeightCustom, -SweepInflation - ScaledHalfHeightAdjust); // Shrink by negative amount, so actually grow it.
		const ECollisionChannel CollisionChannel = UpdatedComponent->GetCollisionObjectType();
		bool bEncroached = true;

		if (!bCrouchMaintainsBaseLocation)
		{
			// Expand in place
			bEncroached = GetWorld()->OverlapBlockingTestByChannel(PawnLocation, FQuat::Identity, CollisionChannel, StandingCapsuleShape, CapsuleParams, ResponseParam);

			if (bEncroached)
			{
				// Try adjusting capsule position to see if we can avoid encroachment.
				if (ScaledHalfHeightAdjust > 0.f)
				{
					// Shrink to a short capsule, sweep down to base to find where that would hit something, and then try to stand up from there.
					float PawnRadius, PawnHalfHeight;
					CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleSize(PawnRadius, PawnHalfHeight);
					const float ShrinkHalfHeight = PawnHalfHeight - PawnRadius;
					const float TraceDist = PawnHalfHeight - ShrinkHalfHeight;
					const FVector Down = FVector(0.f, 0.f, -TraceDist);

					FHitResult Hit(1.f);
					const FCollisionShape ShortCapsuleShape = GetPawnCapsuleCollisionShape(SHRINK_HeightCustom, ShrinkHalfHeight);
					const bool bBlockingHit = GetWorld()->SweepSingleByChannel(Hit, PawnLocation, PawnLocation + Down, FQuat::Identity, CollisionChannel, ShortCapsuleShape, CapsuleParams);
					if (Hit.bStartPenetrating)
					{
						bEncroached = true;
					}
					else
					{
						// Compute where the base of the sweep ended up, and see if we can stand there
						const float DistanceToBase = (Hit.Time * TraceDist) + ShortCapsuleShape.Capsule.HalfHeight;
						const FVector NewLoc = FVector(PawnLocation.X, PawnLocation.Y, PawnLocation.Z - DistanceToBase + PawnHalfHeight + SweepInflation + MIN_FLOOR_DIST / 2.f);
						bEncroached = GetWorld()->OverlapBlockingTestByChannel(NewLoc, FQuat::Identity, CollisionChannel, StandingCapsuleShape, CapsuleParams, ResponseParam);
						if (!bEncroached)
						{
							// Intentionally not using MoveUpdatedComponent, where a horizontal plane constraint would prevent the base of the capsule from staying at the same spot.
							UpdatedComponent->MoveComponent(NewLoc - PawnLocation, UpdatedComponent->GetComponentQuat(), false, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
						}
					}
				}
			}
		}
		else
		{
			// Expand while keeping base location the same.
			FVector StandingLocation = PawnLocation + FVector(0.f, 0.f, StandingCapsuleShape.GetCapsuleHalfHeight() - CurrentCrouchedHalfHeight);
			bEncroached = GetWorld()->OverlapBlockingTestByChannel(StandingLocation, FQuat::Identity, CollisionChannel, StandingCapsuleShape, CapsuleParams, ResponseParam);

			if (bEncroached)
			{
				if (IsMovingOnGround())
				{
					// Something might be just barely overhead, try moving down closer to the floor to avoid it.
					const float MinFloorDist = KINDA_SMALL_NUMBER * 10.f;
					if (CurrentFloor.bBlockingHit && CurrentFloor.FloorDist > MinFloorDist)
					{
						StandingLocation.Z -= CurrentFloor.FloorDist - MinFloorDist;
						bEncroached = GetWorld()->OverlapBlockingTestByChannel(StandingLocation, FQuat::Identity, CollisionChannel, StandingCapsuleShape, CapsuleParams, ResponseParam);
					}
				}
			}

			if (!bEncroached)
			{
				// Commit the change in location.
				UpdatedComponent->MoveComponent(StandingLocation - PawnLocation, UpdatedComponent->GetComponentQuat(), false, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
				bForceNextFloorCheck = true;
			}
		}

		// If still encroached then abort.
		if (bEncroached)
		{
			return;
		}

		CharacterOwner->bIsCrouched = false;
	}
	else
	{
		bShrinkProxyCapsule = true;
	}

	// Now call SetCapsuleSize() to cause touch/untouch events and actually grow the capsule
	CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleRadius(), DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight(), true);

	const float MeshAdjust = ScaledHalfHeightAdjust;
	AdjustProxyCapsuleSize();
	CharacterOwner->OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);

	// Don't smooth this change in mesh position
	if (bClientSimulation && CharacterOwner->GetLocalRole() == ROLE_SimulatedProxy)
	{
		FNetworkPredictionData_Client_Character* ClientData = GetPredictionData_Client_Character();
		if (ClientData && ClientData->MeshTranslationOffset.Z != 0.f)
		{
			ClientData->MeshTranslationOffset += FVector(0.f, 0.f, MeshAdjust);
			ClientData->OriginalMeshTranslationOffset = ClientData->MeshTranslationOffset;
		}
	}
}

bool UCharacterMovementComponentEx::CanStaticInCurrentState() const
{
	return true;
}

void UCharacterMovementComponentEx::Static(bool bClientSimulation)
{
	if (!HasValidData())
	{
		return;
	}

	CharacterMovementExInterce->SetIsStatic(true);
	CharacterMovementExInterce->OnStartStatic();
}

void UCharacterMovementComponentEx::UnStatic(bool bClientSimulation)
{
	if (!HasValidData())
	{
		return;
	}

	CharacterMovementExInterce->SetIsStatic(false);
	CharacterMovementExInterce->OnEndStatic();
}

bool UCharacterMovementComponentEx::CanSprintInCurrentState() const
{
	return true;
}

void UCharacterMovementComponentEx::Sprint(bool bClientSimulation)
{
	if (!HasValidData())
	{
		return;
	}

	CharacterMovementExInterce->SetIsSprint(true);
	CharacterMovementExInterce->OnStartSprint();
}

void UCharacterMovementComponentEx::UnSprint(bool bClientSimulation)
{
	if (!HasValidData())
	{
		return;
	}

	CharacterMovementExInterce->SetIsSprint(false);
	CharacterMovementExInterce->OnEndSprint();
}

bool UCharacterMovementComponentEx::CanAimInCurrentState() const
{
	return true;
}

void UCharacterMovementComponentEx::Aim(bool bClientSimulation)
{
	if (!HasValidData())
	{
		return;
	}

	CharacterMovementExInterce->SetIsAim(true);
	CharacterMovementExInterce->OnStartAim();
}

void UCharacterMovementComponentEx::UnAim(bool bClientSimulation)
{
	if (!HasValidData())
	{
		return;
	}

	CharacterMovementExInterce->SetIsAim(false);
	CharacterMovementExInterce->OnEndAim();
}

void UCharacterMovementComponentEx::UpdateFromCompressedFlags(uint8 Flags)
{
	if (!CharacterOwner)
	{
		return;
	}

	const bool bWasJumping = CharacterOwner->bPressedJump;

	CharacterOwner->bPressedJump = ((Flags & FSavedMove_Character::FLAG_JumpPressed) != 0);
	bWantsToCrouch = ((Flags & FSavedMove_Character::FLAG_WantsToCrouch) != 0);
	bWantsToProne = ((Flags & FLAG_WantsToProne) != 0);
	bWantsToStatic = ((Flags & FLAG_WantsToStatic) != 0);
	bWantsToSprint = ((Flags & FLAG_WantsToSprint) != 0);
	bWantsToAim = ((Flags & FLAG_WantsToAim) != 0);

	// Reset JumpKeyHoldTime when player presses Jump key on server as well.
	if (!bWasJumping && CharacterOwner->bPressedJump)
	{
		CharacterOwner->bWasJumping = false;
		CharacterOwner->JumpKeyHoldTime = 0.0f;
	}
}

bool UCharacterMovementComponentEx::ClientUpdatePositionAfterServerUpdate()
{
	SCOPE_CYCLE_COUNTER(STAT_CharacterMovementClientUpdatePositionAfterServerUpdate);
	if (!HasValidData())
	{
		return false;
	}

	FNetworkPredictionData_Client_Character* ClientData = GetPredictionData_Client_Character();
	check(ClientData);

	if (!ClientData->bUpdatePosition)
	{
		return false;
	}

	if (bIgnoreClientMovementErrorChecksAndCorrection)
	{
		//#if !UE_BUILD_SHIPPING
		//		if (CharacterMovementCVars::NetShowCorrections != 0)
		//		{
		//			UE_LOG(LogNetPlayerMovement, Warning, TEXT("*** Client: %s is set to ignore error checks and corrections with %d saved moves in queue."), *GetNameSafe(CharacterOwner), ClientData->SavedMoves.Num());
		//		}
		//#endif // !UE_BUILD_SHIPPING
		return false;
	}

	ClientData->bUpdatePosition = false;

	// Don't do any network position updates on things running PHYS_RigidBody
	if (CharacterOwner->GetRootComponent() && CharacterOwner->GetRootComponent()->IsSimulatingPhysics())
	{
		return false;
	}

	if (ClientData->SavedMoves.Num() == 0)
	{
		UE_LOG(LogNetPlayerMovement, VeryVerbose, TEXT("ClientUpdatePositionAfterServerUpdate No saved moves to replay"), ClientData->SavedMoves.Num());

		// With no saved moves to resimulate, the move the server updated us with is the last move we've done, no resimulation needed.
		CharacterOwner->bClientResimulateRootMotion = false;
		if (CharacterOwner->bClientResimulateRootMotionSources)
		{
			// With no resimulation, we just update our current root motion to what the server sent us
			UE_LOG(LogRootMotion, VeryVerbose, TEXT("CurrentRootMotion getting updated to ServerUpdate state: %s"), *CharacterOwner->GetName());
			CurrentRootMotion.UpdateStateFrom(CharacterOwner->SavedRootMotion);
			CharacterOwner->bClientResimulateRootMotionSources = false;
		}

		return false;
	}

	// Save important values that might get affected by the replay.
	const float SavedAnalogInputModifier = AnalogInputModifier;
	const FRootMotionMovementParams BackupRootMotionParams = RootMotionParams; // For animation root motion
	const FRootMotionSourceGroup BackupRootMotion = CurrentRootMotion;
	const bool bRealJump = CharacterOwner->bPressedJump;
	const bool bRealCrouch = bWantsToCrouch;
	const bool bRealProne = bWantsToProne;
	const bool bRealStatic = bWantsToStatic;
	const bool bRealSprint = bWantsToSprint;
	const bool bRealAim = bWantsToAim;
	const bool bRealForceMaxAccel = bForceMaxAccel;
	CharacterOwner->bClientWasFalling = (MovementMode == MOVE_Falling);
	CharacterOwner->bClientUpdating = true;
	bForceNextFloorCheck = true;

	// Replay moves that have not yet been acked.
	UE_LOG(LogNetPlayerMovement, VeryVerbose, TEXT("ClientUpdatePositionAfterServerUpdate Replaying %d Moves, starting at Timestamp %f"), ClientData->SavedMoves.Num(), ClientData->SavedMoves[0]->TimeStamp);
	for (int32 i = 0; i < ClientData->SavedMoves.Num(); i++)
	{
		const FSavedMovePtr& CurrentMove = ClientData->SavedMoves[i];
		CurrentMove->PrepMoveFor(CharacterOwner);
		MoveAutonomous(CurrentMove->TimeStamp, CurrentMove->DeltaTime, CurrentMove->GetCompressedFlags(), CurrentMove->Acceleration);
		CurrentMove->PostUpdate(CharacterOwner, FSavedMove_Character::PostUpdate_Replay);
	}

	if (ClientData->PendingMove.IsValid())
	{
		ClientData->PendingMove->bForceNoCombine = true;
	}

	// Restore saved values.
	AnalogInputModifier = SavedAnalogInputModifier;
	RootMotionParams = BackupRootMotionParams;
	CurrentRootMotion = BackupRootMotion;
	if (CharacterOwner->bClientResimulateRootMotionSources)
	{
		// If we were resimulating root motion sources, it's because we had mismatched state
		// with the server - we just resimulated our SavedMoves and now need to restore
		// CurrentRootMotion with the latest "good state"
		UE_LOG(LogRootMotion, VeryVerbose, TEXT("CurrentRootMotion getting updated after ServerUpdate replays: %s"), *CharacterOwner->GetName());
		CurrentRootMotion.UpdateStateFrom(CharacterOwner->SavedRootMotion);
		CharacterOwner->bClientResimulateRootMotionSources = false;
	}
	CharacterOwner->SavedRootMotion.Clear();
	CharacterOwner->bClientResimulateRootMotion = false;
	CharacterOwner->bClientUpdating = false;
	CharacterOwner->bPressedJump = bRealJump;
	bWantsToCrouch = bRealCrouch;
	bWantsToProne = bRealProne;
	bWantsToStatic = bRealStatic;
	bWantsToSprint = bRealSprint;
	bWantsToAim = bRealAim;
	bForceMaxAccel = bRealForceMaxAccel;
	bForceNextFloorCheck = true;

	return (ClientData->SavedMoves.Num() > 0);
}

bool UCharacterMovementComponentEx::IsProning() const
{
	return CharacterMovementExInterce && CharacterMovementExInterce->IsProning();
}

bool UCharacterMovementComponentEx::IsStaticing() const
{
	return CharacterMovementExInterce && CharacterMovementExInterce->IsStaticing();
}

bool UCharacterMovementComponentEx::IsAiming() const
{
	return CharacterMovementExInterce && CharacterMovementExInterce->IsAiming();
}

bool UCharacterMovementComponentEx::IsSprinting() const
{
	return CharacterMovementExInterce && CharacterMovementExInterce->IsSprinting();
}

FNetworkPredictionData_Client* UCharacterMovementComponentEx::GetPredictionData_Client() const
{
	if (ClientPredictionData == nullptr)
	{
		UCharacterMovementComponentEx* MutableThis = const_cast<UCharacterMovementComponentEx*>(this);
		MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_CharacterEx(*this);
	}

	return ClientPredictionData;
}

void FSavedMove_CharacterEx::Clear()
{
	bWantsToProne = false;
	bWantsToStatic = false;
	bWantsToSprint = false;
	bWantsToAim = false;

	FSavedMove_Character::Clear();
}

void FSavedMove_CharacterEx::SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character & ClientData)
{
	CharacterOwner = Character;
	DeltaTime = InDeltaTime;

	SetInitialPosition(Character);

	AccelMag = NewAccel.Size();
	AccelNormal = (AccelMag > SMALL_NUMBER ? NewAccel / AccelMag : FVector::ZeroVector);

	UCharacterMovementComponentEx* MovementComponent = Cast<UCharacterMovementComponentEx>(Character->GetCharacterMovement());
	if (MovementComponent == nullptr)
	{
		return;
	}

	// Round value, so that client and server match exactly (and so we can send with less bandwidth). This rounded value is copied back to the client in ReplicateMoveToServer.
	// This is done after the AccelMag and AccelNormal are computed above, because those are only used client-side for combining move logic and need to remain accurate.
	Acceleration = MovementComponent->RoundAcceleration(NewAccel);

	MaxSpeed = MovementComponent->GetMaxSpeed();

	// CheckJumpInput will increment JumpCurrentCount.
	// Therefore, for replicated moves we want it to set it at 1 less to properly
	// handle the change.
	JumpCurrentCount = Character->JumpCurrentCount > 0 ? Character->JumpCurrentCount - 1 : 0;
	bWantsToCrouch = MovementComponent->bWantsToCrouch;
	bWantsToProne = MovementComponent->bWantsToProne;
	bWantsToStatic = MovementComponent->bWantsToStatic;
	bWantsToSprint = MovementComponent->bWantsToSprint;
	bWantsToAim = MovementComponent->bWantsToAim;
	bForceMaxAccel = MovementComponent->bForceMaxAccel;
	StartPackedMovementMode = MovementComponent->PackNetworkMovementMode();
	//MovementMode = StartPackedMovementMode; // Deprecated, keep backwards compat until removed

	// Root motion source-containing moves should never be combined
	// Main discovered issue being a move without root motion combining with
	// a move with it will cause the DeltaTime for that next move to be larger than
	// intended (effectively root motion applies to movement that happened prior to its activation)
	if (MovementComponent->CurrentRootMotion.HasActiveRootMotionSources())
	{
		bForceNoCombine = true;
	}

	// Launch velocity gives instant and potentially huge change of velocity
	// Avoid combining move to prevent from reverting locations until server catches up
	const bool bHasLaunchVelocity = !MovementComponent->PendingLaunchVelocity.IsZero();
	if (bHasLaunchVelocity)
	{
		bForceNoCombine = true;
	}

	TimeStamp = ClientData.CurrentTimeStamp;
}

uint8 FSavedMove_CharacterEx::GetCompressedFlags() const
{
	uint8 Result = 0;

	if (bPressedJump)
	{
		Result |= FLAG_JumpPressed;
	}

	if (bWantsToCrouch)
	{
		Result |= FLAG_WantsToCrouch;
	}

	if (bWantsToProne)
	{
		Result |= FLAG_WantsToProne;
	}

	if (bWantsToStatic)
	{
		Result |= FLAG_WantsToStatic;
	}

	if (bWantsToSprint)
	{
		Result |= FLAG_WantsToSprint;
	}

	if (bWantsToAim)
	{
		Result |= FLAG_WantsToAim;
	}

	return Result;
}

FNetworkPredictionData_Client_CharacterEx::FNetworkPredictionData_Client_CharacterEx(const UCharacterMovementComponentEx& ClientMovement)
	: FNetworkPredictionData_Client_Character(ClientMovement)
{

}

FSavedMovePtr FNetworkPredictionData_Client_CharacterEx::AllocateNewMove()
{
	return FSavedMovePtr(new FSavedMove_CharacterEx());
}
