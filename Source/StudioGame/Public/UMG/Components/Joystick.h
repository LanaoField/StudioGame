// Copyright 2022 CQUnreal. All Rights Reserved.

#pragma once

#include "StudioGameTypes.h"
#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "Widgets/Input/SJoystick.h"
#include "Joystick.generated.h"


/**
 * UJoystick
 */
UCLASS()
class STUDIOGAME_API UJoystick : public UWidget
{
	GENERATED_UCLASS_BODY()

public:
	//~ Begin UWidget Interface
	virtual void SynchronizeProperties() override;
	//~ End UWidget Interface

	//~ Begin UVisual Interface
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	//~ End UVisual Interface

public:
	/** The joystick style used at runtime */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance", meta = (DisplayName = "Style"))
	FJoystickStyle WidgetStyle;

	/** The color multiplier for the joystick content */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance", meta = (sRGB = "true"))
	FLinearColor ColorAndOpacity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	FKey ActionKey;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	FKey XAxis;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	FKey YAxis;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	EJoystickInputMode InputMode;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	bool bFixed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	float XAxisRate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	float YAxisRate;

public:
	/** Sets the joystick style */
	UFUNCTION(BlueprintCallable, Category="Joystick|Appearance")
	void SetStyle(const FJoystickStyle& InStyle);

	/** Sets the joystick style */
	UFUNCTION(BlueprintCallable, Category = "Joystick|Appearance")
	void SetColorAndOpacity(const FLinearColor& InColorAndOpacity);

	UFUNCTION(BlueprintCallable, Category = "Joystick|Appearance")
	void SetXAxisRate(const float InValue);

	UFUNCTION(BlueprintCallable, Category = "Joystick|Appearance")
	void SetYAxisRate(const float InValue);

	UFUNCTION(BlueprintCallable, Category = "Joystick|Appearance")
	void SetFixed(const bool InValue);

	UFUNCTION(BlueprintCallable, Category = "Joystick|Appearance")
	FVector2D GetMovedOffset() const;

protected:
	//~ Begin UWidget Interface
	virtual TSharedRef<SWidget> RebuildWidget() override;

#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override;
	virtual TSharedRef<SWidget> RebuildDesignWidget(TSharedRef<SWidget> Content) override { return Content; }
#endif
	//~ End UWidget Interface

protected:
	/** Cached pointer to the underlying slate button owned by this UWidget */
	TSharedPtr<class SJoystick> MyJoystick;
};
