// Copyright 2022 CQUnreal. All Rights Reserved.

#pragma once

#include "StudioGameTypes.h"
#include "Components/Widget.h"
#include "Slate/Widgets/SCrosshair.h"
#include "Crosshair.generated.h"


/**
 * UCrosshair
 */
UCLASS()
class STUDIOGAME_API UCrosshair : public UWidget
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
	/** The Crosshair style used at runtime */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance", meta = (DisplayName = "Style"))
	FCrosshairStyle WidgetStyle;

	/** The color multiplier for the crosshair content */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance", meta = (sRGB = "true"))
	FLinearColor ColorAndOpacity;

	/** The value for draw the crosshair content */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance", meta = (sRGB = "true"))
	float Spread;

public:
	/** Sets the Crosshair style */
	UFUNCTION(BlueprintCallable, Category="Crosshair|Appearance")
	void SetStyle(const FCrosshairStyle& InStyle);

	/** Sets the color and opacity */
	UFUNCTION(BlueprintCallable, Category = "Crosshair|Appearance")
	void SetColorAndOpacity(FLinearColor InColorAndOpacity);

	/** Sets the Crosshair style */
	UFUNCTION(BlueprintCallable, Category = "Crosshair|Appearance")
	void SetSpread(const float& InSpread);

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
	TSharedPtr<class SCrosshair> MyCrosshair;
};
