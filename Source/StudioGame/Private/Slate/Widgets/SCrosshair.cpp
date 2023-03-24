// Copyright 2022 CQUnreal. All Rights Reserved.

#include "Widgets/SCrosshair.h"


void SCrosshair::Construct(const FArguments& InArgs)
{
	SetCanTick(false);

	bVisible = true;

	Spread = InArgs._Spread;

	ColorAndOpacity = InArgs._ColorAndOpacity;

	SetCrosshairStyle(InArgs._CrosshairStyle);
}

int32 SCrosshair::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	if (CrosshairBrushs == nullptr)
	{
		return LayerId;
	}

	const bool bIsEnabled = ShouldBeEnabled(bParentEnabled);
	const ESlateDrawEffect DrawEffects = bIsEnabled ? ESlateDrawEffect::None : ESlateDrawEffect::DisabledEffect;

	FPaintGeometry DefaultPaintGeometry = AllottedGeometry.ToPaintGeometry();

	const float DrawScale = DefaultPaintGeometry.DrawScale;
	const FVector2D& DrawPosition = FVector2D(DefaultPaintGeometry.DrawPosition);
	const FVector2D& DrawCenter = DefaultPaintGeometry.GetLocalSize() * 0.5f * DrawScale;

	const float SpreadWidth = Spread.Get(0.f);
	const float SpreadRadius = FMath::Sqrt(SpreadWidth * SpreadWidth + SpreadWidth * SpreadWidth) * 0.5f;

	// Center
	const FSlateBrush* CenterBrush = CrosshairBrushs->Find(ECrosshairDirection::Center);
	if ((CenterBrush != nullptr) && (CenterBrush->DrawAs != ESlateBrushDrawType::NoDrawType))
	{
		const FLinearColor FinalColorAndOpacity(InWidgetStyle.GetColorAndOpacityTint() * ColorAndOpacity.Get().GetColor(InWidgetStyle) * CenterBrush->GetTint(InWidgetStyle));

		FPaintGeometry PaintGeometry(DrawPosition + DrawCenter - CenterBrush->ImageSize * 0.5f * DrawScale, CenterBrush->ImageSize * DrawScale, DrawScale);

		FSlateDrawElement::MakeBox(OutDrawElements, LayerId, PaintGeometry, CenterBrush, DrawEffects, FinalColorAndOpacity);
	}

	// Top
	const FSlateBrush* TopBrush = CrosshairBrushs->Find(ECrosshairDirection::Top);
	if ((TopBrush != nullptr) && (TopBrush->DrawAs != ESlateBrushDrawType::NoDrawType))
	{
		const FLinearColor FinalColorAndOpacity(InWidgetStyle.GetColorAndOpacityTint() * ColorAndOpacity.Get().GetColor(InWidgetStyle) * TopBrush->GetTint(InWidgetStyle));

		FPaintGeometry PaintGeometry(DrawPosition + DrawCenter - FVector2D(TopBrush->ImageSize.X * 0.5f, TopBrush->ImageSize.Y) * DrawScale + FVector2D(0.f, -SpreadWidth) * DrawScale, TopBrush->ImageSize * DrawScale, DrawScale);

		FSlateDrawElement::MakeBox(OutDrawElements, LayerId, PaintGeometry, TopBrush, DrawEffects, FinalColorAndOpacity);
	}

	// Bottom
	const FSlateBrush* BottomBrush = CrosshairBrushs->Find(ECrosshairDirection::Bottom);
	if ((BottomBrush != nullptr) && (BottomBrush->DrawAs != ESlateBrushDrawType::NoDrawType))
	{
		const FLinearColor FinalColorAndOpacity(InWidgetStyle.GetColorAndOpacityTint() * ColorAndOpacity.Get().GetColor(InWidgetStyle) * BottomBrush->GetTint(InWidgetStyle));

		FPaintGeometry PaintGeometry(DrawPosition + DrawCenter - FVector2D(BottomBrush->ImageSize.X * 0.5f, 0.f) * DrawScale + FVector2D(0.f, SpreadWidth) * DrawScale, BottomBrush->ImageSize * DrawScale, DrawScale);

		FSlateDrawElement::MakeBox(OutDrawElements, LayerId, PaintGeometry, BottomBrush, DrawEffects, FinalColorAndOpacity);
	}

	// Left
	const FSlateBrush* LeftBrush = CrosshairBrushs->Find(ECrosshairDirection::Left);
	if ((LeftBrush != nullptr) && (LeftBrush->DrawAs != ESlateBrushDrawType::NoDrawType))
	{
		const FLinearColor FinalColorAndOpacity(InWidgetStyle.GetColorAndOpacityTint() * ColorAndOpacity.Get().GetColor(InWidgetStyle) * LeftBrush->GetTint(InWidgetStyle));

		FPaintGeometry PaintGeometry(DrawPosition + DrawCenter - FVector2D(LeftBrush->ImageSize.X, LeftBrush->ImageSize.Y * 0.5f) * DrawScale + FVector2D(-SpreadWidth, 0.f) * DrawScale, LeftBrush->ImageSize * DrawScale, DrawScale);

		FSlateDrawElement::MakeBox(OutDrawElements, LayerId, PaintGeometry, LeftBrush, DrawEffects, FinalColorAndOpacity);
	}

	// Right
	const FSlateBrush* RightBrush = CrosshairBrushs->Find(ECrosshairDirection::Right);
	if ((RightBrush != nullptr) && (RightBrush->DrawAs != ESlateBrushDrawType::NoDrawType))
	{
		const FLinearColor FinalColorAndOpacity(InWidgetStyle.GetColorAndOpacityTint() * ColorAndOpacity.Get().GetColor(InWidgetStyle) * RightBrush->GetTint(InWidgetStyle));

		FPaintGeometry PaintGeometry(DrawPosition + DrawCenter - FVector2D(0.f, RightBrush->ImageSize.Y * 0.5f) * DrawScale + FVector2D(SpreadWidth, 0.f) * DrawScale, RightBrush->ImageSize * DrawScale, DrawScale);

		FSlateDrawElement::MakeBox(OutDrawElements, LayerId, PaintGeometry, RightBrush, DrawEffects, FinalColorAndOpacity);
	}

	// TopRight
	const FSlateBrush* TopRightBrush = CrosshairBrushs->Find(ECrosshairDirection::TopRight);
	if ((TopRightBrush != nullptr) && (TopRightBrush->DrawAs != ESlateBrushDrawType::NoDrawType))
	{
		const FLinearColor FinalColorAndOpacity(InWidgetStyle.GetColorAndOpacityTint() * ColorAndOpacity.Get().GetColor(InWidgetStyle) * TopRightBrush->GetTint(InWidgetStyle));

		FPaintGeometry PaintGeometry(DrawPosition + DrawCenter - FVector2D(0, TopRightBrush->ImageSize.Y) * DrawScale + FVector2D(SpreadRadius, -SpreadRadius) * DrawScale, TopRightBrush->ImageSize * DrawScale, DrawScale);

		FSlateDrawElement::MakeBox(OutDrawElements, LayerId, PaintGeometry, TopRightBrush, DrawEffects, FinalColorAndOpacity);
	}

	// TopLeft
	const FSlateBrush* TopLeftBrush = CrosshairBrushs->Find(ECrosshairDirection::TopLeft);
	if ((TopLeftBrush != nullptr) && (TopLeftBrush->DrawAs != ESlateBrushDrawType::NoDrawType))
	{
		const FLinearColor FinalColorAndOpacity(InWidgetStyle.GetColorAndOpacityTint() * ColorAndOpacity.Get().GetColor(InWidgetStyle) * TopLeftBrush->GetTint(InWidgetStyle));

		FPaintGeometry PaintGeometry(DrawPosition + DrawCenter - FVector2D(TopLeftBrush->ImageSize.X, TopLeftBrush->ImageSize.Y) * DrawScale + FVector2D(-SpreadRadius, -SpreadRadius) * DrawScale, TopLeftBrush->ImageSize * DrawScale, DrawScale);

		FSlateDrawElement::MakeBox(OutDrawElements, LayerId, PaintGeometry, TopLeftBrush, DrawEffects, FinalColorAndOpacity);
	}

	// BottomRight
	const FSlateBrush* BottomRightBrush = CrosshairBrushs->Find(ECrosshairDirection::BottomRight);
	if ((BottomRightBrush != nullptr) && (BottomRightBrush->DrawAs != ESlateBrushDrawType::NoDrawType))
	{
		const FLinearColor FinalColorAndOpacity(InWidgetStyle.GetColorAndOpacityTint() * ColorAndOpacity.Get().GetColor(InWidgetStyle) * BottomRightBrush->GetTint(InWidgetStyle));

		FPaintGeometry PaintGeometry(DrawPosition + DrawCenter /*- FVector2D(0, 0) * DrawScale */ + FVector2D(SpreadRadius, SpreadRadius) * DrawScale, BottomRightBrush->ImageSize * DrawScale, DrawScale);

		FSlateDrawElement::MakeBox(OutDrawElements, LayerId, PaintGeometry, BottomRightBrush, DrawEffects, FinalColorAndOpacity);
	}

	// BottomLeft
	const FSlateBrush* BottomLeftBrush = CrosshairBrushs->Find(ECrosshairDirection::BottomLeft);
	if ((BottomLeftBrush != nullptr) && (BottomLeftBrush->DrawAs != ESlateBrushDrawType::NoDrawType))
	{
		const FLinearColor FinalColorAndOpacity(InWidgetStyle.GetColorAndOpacityTint() * ColorAndOpacity.Get().GetColor(InWidgetStyle) * BottomLeftBrush->GetTint(InWidgetStyle));

		FPaintGeometry PaintGeometry(DrawPosition + DrawCenter - FVector2D(BottomLeftBrush->ImageSize.X, 0) * DrawScale + FVector2D(-SpreadRadius, SpreadRadius) * DrawScale, BottomLeftBrush->ImageSize * DrawScale, DrawScale);

		FSlateDrawElement::MakeBox(OutDrawElements, LayerId, PaintGeometry, BottomLeftBrush, DrawEffects, FinalColorAndOpacity);
	}

	return LayerId;
}

FVector2D SCrosshair::ComputeDesiredSize(float LayoutScaleMultiplier) const
{
	return FVector2D(100, 100);
}

void SCrosshair::SetCrosshairStyle(const FCrosshairStyle* CrosshairStyle)
{
	Style = CrosshairStyle;

	CrosshairBrushs = &CrosshairStyle->CrosshairBrushs;
}

void SCrosshair::SetColorAndOpacity(FLinearColor InColorAndOpacity)
{
	ColorAndOpacity = InColorAndOpacity;
}

void SCrosshair::SetSpread(const float& InSpread)
{
	Spread = InSpread;
}