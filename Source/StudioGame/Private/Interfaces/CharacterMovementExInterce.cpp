// Copyright 2022 CQUnreal. All Rights Reserved.

#include "Interfaces/CharacterMovementExInterce.h"
#include "Components/CharacterMovementComponentEx.h"
#include "GameFramework/Character.h"


ACharacter* ICharacterMovementExInterce::GetCharacterOwner()
{
	return Cast<ACharacter>(this);
}

UCharacterMovementComponentEx* ICharacterMovementExInterce::GetCharacterMovementComponentEx()
{
	ACharacter* CharacterOwner = Cast<ACharacter>(this);
	return CharacterOwner ? Cast<UCharacterMovementComponentEx>(CharacterOwner->GetCharacterMovement()) : nullptr;
}

void ICharacterMovementExInterce::SetIsCrouched(bool bIsCrouched)
{
	ACharacter* CharacterOwner = Cast<ACharacter>(this);
	if (CharacterOwner)
	{
		CharacterOwner->bIsCrouched = bIsCrouched;
	}
}

bool ICharacterMovementExInterce::CanStatic()
{
	return true;
}

void ICharacterMovementExInterce::Static(bool bClientSimulation)
{
	UCharacterMovementComponentEx* MovementComponent = GetCharacterMovementComponentEx();
	if (MovementComponent)
	{
		if (CanStatic())
		{
			MovementComponent->bWantsToStatic = true;
			MovementComponent->bWantsToSprint = false;
		}
	}
}

void ICharacterMovementExInterce::UnStatic(bool bClientSimulation)
{
	UCharacterMovementComponentEx* MovementComponent = GetCharacterMovementComponentEx();
	if (MovementComponent)
	{
		if (CanStatic())
		{
			MovementComponent->bWantsToStatic = false;
		}
	}
}

void ICharacterMovementExInterce::OnEndStatic()
{
	Execute_K2_OnEndStatic(Cast<UObject>(this));
}

void ICharacterMovementExInterce::OnStartStatic()
{
	Execute_K2_OnStartStatic(Cast<UObject>(this));
}

bool ICharacterMovementExInterce::CanSprint()
{
	return true;
}

void ICharacterMovementExInterce::Sprint(bool bClientSimulation)
{
	UCharacterMovementComponentEx* MovementComponent = GetCharacterMovementComponentEx();
	if (MovementComponent)
	{
		if (CanSprint())
		{
			MovementComponent->bWantsToSprint = true;
			MovementComponent->bWantsToStatic = false;
			MovementComponent->bWantsToAim = false;
		}
	}
}

void ICharacterMovementExInterce::UnSprint(bool bClientSimulation)
{
	UCharacterMovementComponentEx* MovementComponent = GetCharacterMovementComponentEx();
	if (MovementComponent)
	{
		if (CanSprint())
		{
			MovementComponent->bWantsToSprint = false;
		}
	}
}

void ICharacterMovementExInterce::OnEndSprint()
{
	Execute_K2_OnEndSprint(Cast<UObject>(this));
}

void ICharacterMovementExInterce::OnStartSprint()
{
	Execute_K2_OnStartSprint(Cast<UObject>(this));
}

bool ICharacterMovementExInterce::CanProne()
{
	ACharacter* CharacterOwner = GetCharacterOwner();
	return !IsProning() && CharacterOwner && CharacterOwner->GetRootComponent() && !CharacterOwner->GetRootComponent()->IsSimulatingPhysics();
}

void ICharacterMovementExInterce::Prone(bool bClientSimulation)
{
	UCharacterMovementComponentEx* MovementComponent = GetCharacterMovementComponentEx();
	if (MovementComponent)
	{
		if (CanProne())
		{
			MovementComponent->bWantsToProne = true;
			MovementComponent->bWantsToCrouch = false;
		}
	}
}

void ICharacterMovementExInterce::UnProne(bool bClientSimulation)
{
	UCharacterMovementComponentEx* MovementComponent = GetCharacterMovementComponentEx();
	if (MovementComponent)
	{
		MovementComponent->bWantsToProne = false;
	}
}

void ICharacterMovementExInterce::OnEndProne(float HeightAdjust, float ScaledHeightAdjust)
{
	ACharacter* CharacterOwner = GetCharacterOwner();
	if (!CharacterOwner)
	{
		return;
	}

	CharacterOwner->RecalculateBaseEyeHeight();

	const ACharacter* DefaultChar = CharacterOwner->GetClass()->GetDefaultObject<ACharacter>();
	if (CharacterOwner->GetMesh() && DefaultChar->GetMesh())
	{
		FVector NewLocation = CharacterOwner->GetMesh()->GetRelativeLocation();
		NewLocation.Z = DefaultChar->GetMesh()->GetRelativeLocation().Z;

		CharacterOwner->GetMesh()->SetRelativeLocation(NewLocation);

		FVector NewBaseTranslationOffset = CharacterOwner->GetBaseTranslationOffset();
		NewBaseTranslationOffset.Z = NewLocation.Z;

		SetBaseTranslationOffset(NewBaseTranslationOffset);
	}
	else
	{
		FVector NewBaseTranslationOffset = CharacterOwner->GetBaseTranslationOffset();
		NewBaseTranslationOffset.Z = DefaultChar->GetBaseTranslationOffset().Z;

		SetBaseTranslationOffset(NewBaseTranslationOffset);
	}

	Execute_K2_OnEndProne(Cast<UObject>(this), HeightAdjust, ScaledHeightAdjust);
}

void ICharacterMovementExInterce::OnStartProne(float HeightAdjust, float ScaledHeightAdjust)
{
	ACharacter* CharacterOwner = GetCharacterOwner();
	if (!CharacterOwner)
	{
		return;
	}

	CharacterOwner->RecalculateBaseEyeHeight();

	const ACharacter* DefaultChar = CharacterOwner->GetClass()->GetDefaultObject<ACharacter>();
	if (CharacterOwner->GetMesh() && DefaultChar->GetMesh())
	{
		FVector NewLocation = CharacterOwner->GetMesh()->GetRelativeLocation();
		NewLocation.Z = DefaultChar->GetMesh()->GetRelativeLocation().Z + HeightAdjust;

		CharacterOwner->GetMesh()->SetRelativeLocation(NewLocation, false, nullptr, ETeleportType::TeleportPhysics);
		
		FVector NewBaseTranslationOffset = CharacterOwner->GetBaseTranslationOffset();
		NewBaseTranslationOffset.Z = NewLocation.Z;

		SetBaseTranslationOffset(NewBaseTranslationOffset);
	}
	else
	{
		FVector NewBaseTranslationOffset = CharacterOwner->GetBaseTranslationOffset();
		NewBaseTranslationOffset.Z += HeightAdjust;

		SetBaseTranslationOffset(NewBaseTranslationOffset);
	}

	Execute_K2_OnStartProne(Cast<UObject>(this), HeightAdjust, ScaledHeightAdjust);
}

bool ICharacterMovementExInterce::CanAim()
{
	ACharacter* CharacterOwner = GetCharacterOwner();
	return !IsAiming() && CharacterOwner && CharacterOwner->GetRootComponent() && !CharacterOwner->GetRootComponent()->IsSimulatingPhysics();
}

void ICharacterMovementExInterce::Aim(bool bClientSimulation)
{
	UCharacterMovementComponentEx* MovementComponent = GetCharacterMovementComponentEx();
	if (MovementComponent)
	{
		if (CanAim())
		{
			MovementComponent->bWantsToAim = true;
			MovementComponent->bWantsToSprint = false;
		}
	}
}

void ICharacterMovementExInterce::UnAim(bool bClientSimulation)
{
	UCharacterMovementComponentEx* MovementComponent = GetCharacterMovementComponentEx();
	if (MovementComponent)
	{
		MovementComponent->bWantsToAim = false;
	}
}

void ICharacterMovementExInterce::OnEndAim()
{
	Execute_K2_OnEndAim(Cast<UObject>(this));
}

void ICharacterMovementExInterce::OnStartAim()
{
	Execute_K2_OnStartAim(Cast<UObject>(this));
}