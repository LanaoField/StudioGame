#include "UMG/Components/ScrollBoxSlotEx.h"
#include "Components/Widget.h"

UScrollBoxSlotEx::UScrollBoxSlotEx(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

#if ENGINE_MAJOR_VERSION >= 5
void UScrollBoxSlotEx::BuildSlot(TSharedRef<SScrollBoxEx> ScrollBox)
{
	PRAGMA_DISABLE_DEPRECATION_WARNINGS
		ScrollBox->AddSlot()
		.Padding(Padding)
		.HAlign(HorizontalAlignment)
		.VAlign(VerticalAlignment)
		.Expose(Slot)
		[
			Content == nullptr ? SNullWidget::NullWidget : Content->TakeWidget()
		];
	PRAGMA_ENABLE_DEPRECATION_WARNINGS
}

void UScrollBoxSlotEx::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	Slot = nullptr;
}
#endif