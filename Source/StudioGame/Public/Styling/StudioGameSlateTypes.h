// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"
#include "UObject/Class.h"
#include "Styling/SlateColor.h"
#include "Fonts/CompositeFont.h"
#include "Fonts/SlateFontInfo.h"
#include "Layout/Margin.h"
#include "Sound/SlateSound.h"
#include "Styling/SlateBrush.h"
#include "Styling/SlateWidgetStyle.h"
#include "Styling/StudioGameCoreStyle.h"
#include "StudioGameSlateTypes.generated.h"

/**
 * Represents the appearance of an SDoubleSlider
 */
USTRUCT(BlueprintType)
struct STUDIOGAME_API FDoubleSliderStyle : public FSlateWidgetStyle
{
	GENERATED_USTRUCT_BODY()

	FDoubleSliderStyle();

	virtual ~FDoubleSliderStyle() {}

	virtual void GetResources( TArray< const FSlateBrush* >& OutBrushes ) const override;

	static const FName TypeName;
	virtual const FName GetTypeName() const override { return TypeName; };

	static const FDoubleSliderStyle& GetDefault();

	/** Image to use when the slider bar is in its normal state */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Appearance)
	FSlateBrush NormalBarImage;
	FDoubleSliderStyle& SetNormalBarImage(const FSlateBrush& InNormalBarImage){ NormalBarImage = InNormalBarImage; return *this; }

	/** Image to use when the slider bar is in its hovered state */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Appearance)
	FSlateBrush HoveredBarImage;
	FDoubleSliderStyle& SetHoveredBarImage(const FSlateBrush& InHoveredBarImage){ HoveredBarImage = InHoveredBarImage; return *this; }

	/** Image to use when the slider bar is in its disabled state */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Appearance)
	FSlateBrush DisabledBarImage;
	FDoubleSliderStyle& SetDisabledBarImage(const FSlateBrush& InDisabledBarImage){ DisabledBarImage = InDisabledBarImage; return *this; }

	/** Image to use when the slider thumb is in its normal state */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Appearance)
	FSlateBrush NormalThumbImage;
	FDoubleSliderStyle& SetNormalThumbImage( const FSlateBrush& InNormalThumbImage ){ NormalThumbImage = InNormalThumbImage; return *this; }

	/** Image to use when the slider thumb is in its hovered state */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Appearance)
	FSlateBrush HoveredThumbImage;
	FDoubleSliderStyle& SetHoveredThumbImage( const FSlateBrush& InHoveredThumbImage ){ HoveredThumbImage = InHoveredThumbImage; return *this; }

	/** Image to use when the slider thumb is in its disabled state */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Appearance)
	FSlateBrush DisabledThumbImage;
	FDoubleSliderStyle& SetDisabledThumbImage( const FSlateBrush& InDisabledThumbImage ){ DisabledThumbImage = InDisabledThumbImage; return *this; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Appearance)
	float BarThickness;
	FDoubleSliderStyle& SetBarThickness(float InBarThickness) { BarThickness = InBarThickness; return *this; }

	/**
	 * Unlinks all colors in this style.
	 * @see FSlateColor::Unlink
	 */
	void UnlinkColors()
	{
		NormalBarImage.UnlinkColors();
		HoveredBarImage.UnlinkColors();
		DisabledBarImage.UnlinkColors();
		NormalThumbImage.UnlinkColors();
		HoveredThumbImage.UnlinkColors();
		DisabledThumbImage.UnlinkColors();
	}
};

UENUM(BlueprintType)
enum class EJoystickInputMode : uint8
{
	Joystick,

	Slide
};

/**
 * Represents the appearance of an SJoystick
 */
USTRUCT(BlueprintType)
struct STUDIOGAME_API FJoystickStyle : public FSlateWidgetStyle
{
	GENERATED_USTRUCT_BODY()

public:
	FJoystickStyle()
		: Center(FVector2D::ZeroVector)
		, VisualSize(FVector2D::ZeroVector)
		, ThumbSize(FVector2D::ZeroVector)
	{

	}

	virtual void GetResources(TArray< const FSlateBrush* >& OutBrushes) const override;

	static const FName TypeName;
	virtual const FName GetTypeName() const override { return TypeName; };

	static const FJoystickStyle& GetDefault();

	UPROPERTY(EditAnywhere, Category = VirtualJoystick, meta = (ToolTip = "The center point of the control (if <= 1.0, it's relative to screen, > 1.0 is absolute)"))
	FVector2D Center;
	FJoystickStyle& SetCenter(const FVector2D& InCenter) { Center = InCenter; return *this; }

	UPROPERTY(EditAnywhere, Category = VirtualJoystick, meta = (ToolTip = "The size of the control (if <= 1.0, it's relative to screen, > 1.0 is absolute)"))
	FVector2D VisualSize;
	FJoystickStyle& SetVisualSize(const FVector2D& InVisualSize) { VisualSize = InVisualSize; return *this; }

	UPROPERTY(EditAnywhere, Category = VirtualJoystick, meta = (ToolTip = "For sticks, the size of the thumb (if <= 1.0, it's relative to screen, > 1.0 is absolute)"))
	FVector2D ThumbSize;
	FJoystickStyle& SetThumbSize(const FVector2D& InThumbSize) { ThumbSize = InThumbSize; return *this; }

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Appearance)
	FSlateBrush VisualBrush;
	FJoystickStyle& SetVisualBrush(const FSlateBrush& InVisualBrush) { VisualBrush = InVisualBrush; return *this; }

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Appearance)
	FSlateBrush ThumbBrush;
	FJoystickStyle& SetThumbBrush(const FSlateBrush& InThumbBrush) { ThumbBrush = InThumbBrush; return *this; }
};

UENUM(BlueprintType)
enum class ECrosshairDirection : uint8
{
	None,
	Center,
	Top,
	Bottom,
	Left,
	Right,
	TopLeft,
	TopRight,
	BottomLeft,
	BottomRight
};

/**
 * Represents the appearance of an SCrosshair
 */
USTRUCT(BlueprintType)
struct STUDIOGAME_API FCrosshairStyle : public FSlateWidgetStyle
{
	GENERATED_USTRUCT_BODY()

public:
	virtual void GetResources(TArray< const FSlateBrush* >& OutBrushes) const override;

	static const FName TypeName;
	virtual const FName GetTypeName() const override { return TypeName; };

	static const FCrosshairStyle& GetDefault();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Appearance)
	TMap<ECrosshairDirection, FSlateBrush> CrosshairBrushs;
	FCrosshairStyle& SetVisualBrush(const TMap<ECrosshairDirection, FSlateBrush>& InCrosshairBrushs) { CrosshairBrushs = InCrosshairBrushs; return *this; }
};
