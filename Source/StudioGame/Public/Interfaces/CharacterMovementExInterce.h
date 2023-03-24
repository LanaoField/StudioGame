// Copyright 2022 CQUnreal. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CharacterMovementExInterce.generated.h"


UINTERFACE(MinimalAPI)
class UCharacterMovementExInterce : public UInterface
{
	GENERATED_BODY()
};

/**
 * 移动组件扩展接口
 */
class STUDIOGAME_API ICharacterMovementExInterce
{
	GENERATED_BODY()

public:
	virtual class ACharacter* GetCharacterOwner();
	virtual class UCharacterMovementComponentEx* GetCharacterMovementComponentEx();
	virtual void SetIsCrouched(bool bIsCrouched);

	UFUNCTION()
	virtual bool IsStaticing() const = 0;

	virtual void SetIsStatic(bool bIsStatic) = 0;

	/** @return true if this character is currently able to sprint (and is not currently staticing) */
	UFUNCTION()
	virtual bool CanStatic();

	UFUNCTION()
	virtual void Static(bool bClientSimulation = false);

	UFUNCTION()
	virtual void UnStatic(bool bClientSimulation = false);

	/** 
	 * Called when character stops static. Called on non-owned Characters through bIsStatic replication.
	 */
	virtual void OnEndStatic();

	/** 
	 * Event when character stops static.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = CharacterMovementExInterce, meta=(DisplayName="OnEndStatic", ScriptName="OnEndStatic"))
	void K2_OnEndStatic();

	/**
	 * Called when character static. Called on non-owned Characters through bIsStatic replication.
	 */
	virtual void OnStartStatic();

	/**
	 * Event when character static.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = CharacterMovementExInterce, meta=(DisplayName="OnStartStatic", ScriptName="OnStartStatic"))
	void K2_OnStartStatic();

	UFUNCTION()
	virtual bool IsSprinting() const = 0;

	virtual void SetIsSprint(bool bIsSprinting) = 0;

	/** @return true if this character is currently able to sprint (and is not currently sprinting) */
	UFUNCTION()
	virtual bool CanSprint();

	UFUNCTION()
	virtual void Sprint(bool bClientSimulation = false);

	UFUNCTION()
	virtual void UnSprint(bool bClientSimulation = false);

	/** 
	 * Called when character stops sprinting. Called on non-owned Characters through bIsSprint replication.
	 */
	virtual void OnEndSprint();

	/** 
	 * Event when character stops sprinting.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = CharacterMovementExInterce, meta=(DisplayName="OnEndSprint", ScriptName="OnEndSprint"))
	void K2_OnEndSprint();

	/**
	 * Called when character aim. Called on non-owned Characters through bIsSprint replication.
	 */
	virtual void OnStartSprint();

	/**
	 * Event when character sprint.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = CharacterMovementExInterce, meta=(DisplayName="OnStartSprint", ScriptName="OnStartSprint"))
	void K2_OnStartSprint();

	UFUNCTION()
	virtual bool IsProning() const = 0;

	virtual void SetIsProne(bool bIsProning) = 0;
	
	virtual void SetBaseTranslationOffset(const FVector& NewBaseTranslationOffset) = 0;

	/** @return true if this character is currently able to prone (and is not currently proned) */
	UFUNCTION()
	virtual bool CanProne();

	UFUNCTION()
	virtual void Prone(bool bClientSimulation = false);

	UFUNCTION()
	virtual void UnProne(bool bClientSimulation = false);

	/** 
	 * Called when character stops proning. Called on non-owned Characters through bIsPorned replication.
	 * @param	HalfHeightAdjust		difference between default collision half-height, and actual proned capsule half-height.
	 * @param	ScaledHalfHeightAdjust	difference after component scale is taken in to account.
	 */
	virtual void OnEndProne(float HalfHeightAdjust, float ScaledHalfHeightAdjust);

	/** 
	 * Event when character stops proning.
	 * @param	HalfHeightAdjust		difference between default collision half-height, and actual proned capsule half-height.
	 * @param	ScaledHalfHeightAdjust	difference after component scale is taken in to account.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = CharacterMovementExInterce, meta=(DisplayName="OnEndProne", ScriptName="OnEndProne"))
	void K2_OnEndProne(float HalfHeightAdjust, float ScaledHalfHeightAdjust);

	/**
	 * Called when character prones. Called on non-owned Characters through bIsProned replication.
	 * @param	HalfHeightAdjust		difference between default collision half-height, and actual proned capsule half-height.
	 * @param	ScaledHalfHeightAdjust	difference after component scale is taken in to account.
	 */
	virtual void OnStartProne(float HalfHeightAdjust, float ScaledHalfHeightAdjust);

	/**
	 * Event when character prones.
	 * @param	HalfHeightAdjust		difference between default collision half-height, and actual proned capsule half-height.
	 * @param	ScaledHalfHeightAdjust	difference after component scale is taken in to account.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = CharacterMovementExInterce, meta=(DisplayName="OnStartProne", ScriptName="OnStartProne"))
	void K2_OnStartProne(float HalfHeightAdjust, float ScaledHalfHeightAdjust);

	UFUNCTION()
	virtual bool IsAiming() const = 0;
	
	virtual void SetIsAim(bool bIsAiming) = 0;

	/** @return true if this character is currently able to aim (and is not currently aiming) */
	UFUNCTION()
	virtual bool CanAim();

	UFUNCTION()
	virtual void Aim(bool bClientSimulation = false);

	UFUNCTION()
	virtual void UnAim(bool bClientSimulation = false);

	/** 
	 * Called when character stops aiming. Called on non-owned Characters through bIsAiming replication.
	 */
	virtual void OnEndAim();

	/** 
	 * Event when character stops aiming.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = CharacterMovementExInterce, meta=(DisplayName="OnEndAim", ScriptName="OnEndAim"))
	void K2_OnEndAim();

	/**
	 * Called when character aim. Called on non-owned Characters through bIsAiming replication.
	 */
	virtual void OnStartAim();

	/**
	 * Event when character aim.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = CharacterMovementExInterce, meta=(DisplayName="OnStartAim", ScriptName="OnStartAim"))
	void K2_OnStartAim();
};
