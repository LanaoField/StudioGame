// Copyright 2023 CQUnreal. All Rights Reserved.

#pragma once
#include "Components/ScrollBoxSlot.h"
#include "Slate/Widgets/SScrollBoxEx.h"
#include "ScrollBoxSlotEx.generated.h"

UCLASS()
class STUDIOGAME_API UScrollBoxSlotEx : public UScrollBoxSlot
{
	GENERATED_UCLASS_BODY()

#if ENGINE_MAJOR_VERSION >= 5
public:
	/** Builds the underlying FSlot for the Slate layout panel. */
	void BuildSlot(TSharedRef<SScrollBoxEx> ScrollBox);

	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

private:

	/** A raw pointer to the slot to allow us to adjust the size, padding...etc at runtime. */
	SScrollBoxEx::FSlot* Slot;
#endif
};
