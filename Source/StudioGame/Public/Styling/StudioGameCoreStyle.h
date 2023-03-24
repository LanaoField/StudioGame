// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Styling/ISlateStyle.h"


struct FSlateDynamicImageBrush;

/**
 * StudioGame Core slate style
 */
class STUDIOGAME_API FStudioGameCoreStyle
{
public:

	static TSharedRef<class ISlateStyle> Create();

	/** 
	* @return the Application Style 
	*
	* NOTE: Until the Editor can be fully updated, calling FStarshipCoreStyle::Get() will
	* return the AppStyle instead of the style defined in this class.  
	*
	* Using the AppStyle is preferred in most cases as it allows the style to be changed 
	* and restyled more easily.
	*
	* In cases requiring explicit use of the CoreStyle where a Slate Widget should not take on
	* the appearance of the rest of the application, use FStarshipCoreStyle::GetCoreStyle().
	*
	*/
	static const ISlateStyle& Get( )
	{
		if (!Instance.IsValid())
		{
			ResetToDefault();
		}

		return *(Instance.Get());
	}

	static void ResetToDefault();

	static bool IsInitialized() { return Instance.IsValid(); }

private:
	class FStyle;

	static void SetStyle(const TSharedRef<class ISlateStyle>& NewStyle);

private:

	/** Singleton instances of this style. */
	static TSharedPtr< class ISlateStyle > Instance;

};
