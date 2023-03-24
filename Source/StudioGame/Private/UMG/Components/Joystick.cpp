// Copyright 2022 CQUnreal. All Rights Reserved.

#include "Components/Joystick.h"

#define LOCTEXT_NAMESPACE "UMG"

UJoystick::UJoystick(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, ColorAndOpacity(FLinearColor::White)
	, InputMode(EJoystickInputMode::Joystick)
	, bFixed(true)
	, XAxisRate(1.f)
	, YAxisRate(1.f)
{

}

void UJoystick::SynchronizeProperties()
{
	Super::SynchronizeProperties();
}

void UJoystick::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	MyJoystick.Reset();
}

void UJoystick::SetStyle(const FJoystickStyle& InStyle)
{
	WidgetStyle = InStyle;
	if (MyJoystick.IsValid())
	{
		MyJoystick->SetJoystickStyle(&WidgetStyle);
	}
}


void UJoystick::SetColorAndOpacity(const FLinearColor& InColorAndOpacity)
{
	ColorAndOpacity = InColorAndOpacity;
	if (MyJoystick.IsValid())
	{
		MyJoystick->SetColorAndOpacity(ColorAndOpacity);
	}
}

void UJoystick::SetXAxisRate(const float InValue)
{
	XAxisRate = InValue;
	if (MyJoystick.IsValid())
	{
		MyJoystick->SetXAxisRate(InValue);
	}
}


void UJoystick::SetYAxisRate(const float InValue)
{
	YAxisRate = InValue;
	if (MyJoystick.IsValid())
	{
		MyJoystick->SetYAxisRate(InValue);
	}
}


void UJoystick::SetFixed(const bool InValue)
{
	bFixed = InValue;
	if (MyJoystick.IsValid())
	{
		MyJoystick->SetFixed(InValue);
	}
}

FVector2D UJoystick::GetMovedOffset() const
{
	if (MyJoystick.IsValid())
	{
		return MyJoystick->GetMovedOffset();
	}

	return FVector2D::ZeroVector;
}

TSharedRef<SWidget> UJoystick::RebuildWidget()
{
	MyJoystick = SNew(SJoystick)
		.PCOwner(GetOwningPlayer())
		.JoystickStyle(&WidgetStyle)
		.ActionKey(ActionKey)
		.XAxis(XAxis)
		.YAxis(YAxis)
		.InputMode(InputMode)
		.bFixed(bFixed)
		.XAxisRate(XAxisRate)
		.YAxisRate(YAxisRate)
		.ColorAndOpacity(ColorAndOpacity);

	return MyJoystick.ToSharedRef();
}

#if WITH_EDITOR

const FText UJoystick::GetPaletteCategory()
{
	return LOCTEXT("Common", "Common");
}

#endif

/////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE

