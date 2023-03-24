// Copyright 2022 CQUnreal. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CharacterMovementComponentEx.generated.h"

/**
 * CharacterMovementComponentEx
 */
UCLASS()
class STUDIOGAME_API UCharacterMovementComponentEx : public UCharacterMovementComponent
{
	GENERATED_UCLASS_BODY()
	
	virtual void PostLoad() override;
	virtual void PerformMovement(float DeltaTime) override;
	virtual void SimulateMovement(float DeltaTime) override;
	virtual void SetUpdatedComponent(USceneComponent* NewUpdatedComponent) override;

	/** Return true if we have a valid CharacterOwner and UpdatedComponent. */
	virtual bool HasValidData() const;

	/** Set speed rate */
	virtual void SetSpeedRate(float NewSpeedRate);

	//BEGIN UMovementComponent Interface
	virtual float GetMaxSpeed() const override;

	/** 
	 * Updates Velocity and Acceleration based on the current state, applying the effects of friction and acceleration or deceleration. Does not apply gravity.
	 * This is used internally during movement updates. Normally you don't need to call this from outside code, but you might want to use it for custom movement modes.
	 *
	 * @param	DeltaTime						time elapsed since last frame.
	 * @param	Friction						coefficient of friction when not accelerating, or in the direction opposite acceleration.
	 * @param	bFluid							true if moving through a fluid, causing Friction to always be applied regardless of acceleration.
	 * @param	BrakingDeceleration				deceleration applied when not accelerating, or when exceeding max velocity.
	 */
	virtual void CalcVelocity(float DeltaTime, float Friction, bool bFluid, float BrakingDeceleration) override;

	/** Called after MovementMode has changed. Base implementation does special handling for starting certain modes, then notifies the CharacterOwner. */
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;

	/** Update the character state in PerformMovement right before doing the actual position change */
	virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;

	/** Update the character state in PerformMovement after the position change. Some rotation updates happen after this. */
	virtual void UpdateCharacterStateAfterMovement(float DeltaSeconds) override;

	/** @return true if the character is allowed to prone in the current state. By default it is allowed when walking or falling, if CanEverProne() is true. */
	virtual bool CanProneInCurrentState() const;

	/**
	* Checks if new capsule size fits (no encroachment), and call CharacterOwner->OnStartProne() if successful.
	* In general you should set bWantsToProne instead to have the prone persist during movement, or just use the prone functions on the owning Character.
	* @param	bClientSimulation	true when called when bIsProned is replicated to non owned clients, to update collision cylinder and offset.
	*/
	virtual void Prone(bool bClientSimulation = false);

	/**
	* Checks if default capsule size fits (no encroachment), and trigger OnEndProne() on the owner if successful.
	* @param	bClientSimulation	true when called when bIsProned is replicated to non owned clients, to update collision cylinder and offset.
	*/
	virtual void UnProne(bool bClientSimulation = false);

	/**
	 * Checks if new capsule size fits (no encroachment), and call CharacterOwner->OnStartCrouch() if successful.
	 * In general you should set bWantsToCrouch instead to have the crouch persist during movement, or just use the crouch functions on the owning Character.
	 * @param	bClientSimulation	true when called when bIsCrouched is replicated to non owned clients, to update collision cylinder and offset.
	 */
	virtual void Crouch(bool bClientSimulation = false);
	
	/**
	 * Checks if default capsule size fits (no encroachment), and trigger OnEndCrouch() on the owner if successful.
	 * @param	bClientSimulation	true when called when bIsCrouched is replicated to non owned clients, to update collision cylinder and offset.
	 */
	virtual void UnCrouch(bool bClientSimulation = false);

	/** @return true if the character is allowed to aim in the current state. By default it is allowed when walking or falling */
	virtual bool CanStaticInCurrentState() const;

	/**
	*  call CharacterOwner->OnStartSprint() if successful.
	* In general you should set bWantsToAim instead to have the prone persist during movement, or just use the prone functions on the owning Character.
	* @param	bClientSimulation	true when called when bIsAiming is replicated to non owned clients, to update collision cylinder and offset.
	*/
	virtual void Static(bool bClientSimulation = false);

	/**
	* trigger OnEndSprint() on the owner if successful.
	* @param	bClientSimulation	true when called when bIsAiming is replicated to non owned clients, to update collision cylinder and offset.
	*/
	virtual void UnStatic(bool bClientSimulation = false);

	/** @return true if the character is allowed to aim in the current state. By default it is allowed when walking or falling */
	virtual bool CanSprintInCurrentState() const;

	/**
	*  call CharacterOwner->OnStartSprint() if successful.
	* In general you should set bWantsToAim instead to have the prone persist during movement, or just use the prone functions on the owning Character.
	* @param	bClientSimulation	true when called when bIsAiming is replicated to non owned clients, to update collision cylinder and offset.
	*/
	virtual void Sprint(bool bClientSimulation = false);

	/**
	* trigger OnEndSprint() on the owner if successful.
	* @param	bClientSimulation	true when called when bIsAiming is replicated to non owned clients, to update collision cylinder and offset.
	*/
	virtual void UnSprint(bool bClientSimulation = false);

	/** @return true if the character is allowed to aim in the current state. By default it is allowed when walking or falling */
	virtual bool CanAimInCurrentState() const;

	/**
	*  call CharacterOwner->OnStartAim() if successful.
	* In general you should set bWantsToAim instead to have the prone persist during movement, or just use the prone functions on the owning Character.
	* @param	bClientSimulation	true when called when bIsAiming is replicated to non owned clients, to update collision cylinder and offset.
	*/
	virtual void Aim(bool bClientSimulation = false);

	/**
	* trigger OnEndAim() on the owner if successful.
	* @param	bClientSimulation	true when called when bIsAiming is replicated to non owned clients, to update collision cylinder and offset.
	*/
	virtual void UnAim(bool bClientSimulation = false);

protected:
	/** Unpack compressed flags from a saved move and set state accordingly. See FSavedMove_Character. */
	virtual void UpdateFromCompressedFlags(uint8 Flags) override;

	/** If bUpdatePosition is true, then replay any unacked moves. Returns whether any moves were actually replayed. */
	virtual bool ClientUpdatePositionAfterServerUpdate() override;

	/** Get prediction data for a client game. Should not be used if not running as a client. Allocates the data on demand and can be overridden to allocate a custom override if desired. Result must be a FNetworkPredictionData_Client_Character. */
	virtual class FNetworkPredictionData_Client* GetPredictionData_Client() const override;
	
	//END UMovementComponent Interface

public:
	virtual bool IsProning() const;
	virtual bool IsStaticing() const;
	virtual bool IsSprinting() const;
	virtual bool IsAiming() const;

	/** Stop to walk starting rate */
	UPROPERTY(Category = "Character Movement: Walking", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float StartingSpeedRate;
	
	/** The maximum ground speed when walking and Proned. */
	UPROPERTY(Category = "Character Movement: Walking", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float MaxWalkSpeedProned;

	/** The maximum ground speed when walking and Static. */
	UPROPERTY(Category = "Character Movement: Walking", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float MaxWalkSpeedStaticing;

	/** The maximum ground speed when sprinting. Also determines maximum lateral speed when falling. */
	UPROPERTY(Category = "Character Movement: Walking", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float MaxWalkSpeedSprinting;

	/** The angle of direction limitation in sprinting. */
	UPROPERTY(Category = "Character Movement: Walking", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float SprintAstrictAngle;

	/** The maximum ground speed rate when walking and aiming. */
	UPROPERTY(Category = "Character Movement: Walking", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", ClampMax = "1.0", UIMin = "0", UIMax = "1.0"))
	float AimingDecelerationRate;

	/** If true, try to prone (or keep proning) on next update. If false, try to stop proning on next update. */
	UPROPERTY(Category = "Character Movement (General Settings)", VisibleInstanceOnly, BlueprintReadOnly)
	uint8 bWantsToProne : 1;

	/** If true, try to static (or keep staticing) on next update. If false, try to stop staticing on next update. */
	UPROPERTY(Category = "Character Movement (General Settings)", VisibleInstanceOnly, BlueprintReadOnly)
	uint8 bWantsToStatic : 1;

	/** If true, try to aim (or keep sprinting) on next update. If false, try to stop sprinting on next update. */
	UPROPERTY(Category = "Character Movement (General Settings)", VisibleInstanceOnly, BlueprintReadOnly)
	uint8 bWantsToSprint : 1;

	/** If true, try to aim (or keep aiming) on next update. If false, try to stop aiming on next update. */
	UPROPERTY(Category = "Character Movement (General Settings)", VisibleInstanceOnly, BlueprintReadOnly)
	uint8 bWantsToAim : 1;

	/** Collision half-height when proning (component scale is applied separately) */
	UPROPERTY(Category = "Character Movement (General Settings)", EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0", UIMin = "0"))
	float PronedHalfHeight;

	UPROPERTY()
	float SpeedRate = 1.f;

	/**
	 * If true, proning should keep the base of the capsule in place by lowering the center of the shrunken capsule. If false, the base of the capsule moves up and the center stays in place.
	 * The same behavior applies when the character unprones: if true, the base is kept in the same location and the center moves up. If false, the capsule grows and only moves up if the base impacts something.
	 * By default this variable is set when the movement mode changes: set to true when walking and false otherwise. Feel free to override the behavior when the movement mode changes.
	 */
	UPROPERTY(Category = "Character Movement (General Settings)", VisibleInstanceOnly, BlueprintReadWrite, AdvancedDisplay)
	uint32 bProneMaintainsBaseLocation : 1;

	/** The animation is in the state switching state. */
	UPROPERTY(Transient)
	uint32 bIsSwitchingState : 1;

protected:
	/** Cached pointer to the movement owner for this Component */
	UPROPERTY()
	TScriptInterface<class ICharacterMovementExInterce> CharacterMovementExInterce;
};

#define FLAG_WantsToProne FSavedMove_Character::FLAG_Custom_0
#define FLAG_WantsToAim FSavedMove_Character::FLAG_Custom_1
#define FLAG_WantsToStatic FSavedMove_Character::FLAG_Custom_2
#define FLAG_WantsToSprint FSavedMove_Character::FLAG_Custom_3


class STUDIOGAME_API FSavedMove_CharacterEx : public FSavedMove_Character
{
public:
	uint32 bWantsToProne : 1;
	uint32 bWantsToStatic : 1;
	uint32 bWantsToSprint : 1;
	uint32 bWantsToAim : 1;

	/** Clear saved move properties, so it can be re-used. */
	virtual void Clear() override;

	/** Called to set up this saved move (when initially created) to make a predictive correction. */
	virtual void SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character & ClientData);

	/** @returns a byte containing encoded special movement information (jumping, crouching, etc.)	 */
	virtual uint8 GetCompressedFlags() const override;
};

class STUDIOGAME_API FNetworkPredictionData_Client_CharacterEx : public FNetworkPredictionData_Client_Character
{
public:
	FNetworkPredictionData_Client_CharacterEx(const UCharacterMovementComponentEx& ClientMovement);

	/** Allocate a new saved move. Subclasses should override this if they want to use a custom move class. */
	virtual FSavedMovePtr AllocateNewMove() override;
};
