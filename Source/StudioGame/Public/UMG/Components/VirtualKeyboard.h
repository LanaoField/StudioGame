// Copyright 2018 CQUnreal, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "VirtualKeyboard.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnKeyboardInputEvent, const FString&, Key);

/**
* 虚拟按键
*/
UCLASS(Meta = ( DisplayName = "VirtualKey"))
class STUDIOGAME_API UVirtualKey : public UButton
{
	GENERATED_UCLASS_BODY()
	
	DECLARE_DYNAMIC_DELEGATE_RetVal_OneParam(bool, FOnInputed, const FString&, Key);

public:
	UPROPERTY(EditAnywhere,  Category = VirtualKeyboard)
	FString Key;

	UPROPERTY(EditAnywhere, Category = VirtualKeyboard)
	FString Shift_Key;

	UPROPERTY(EditAnywhere, Category = VirtualKeyboard)
	bool bIsShift;

	UPROPERTY(VisibleAnywhere, Category = VirtualKeyboard)
	bool OnInputedEvent;

	/** A bindable delegate for bIsShift */
	UPROPERTY()
	FGetBool bIsShiftDelegate;

	UFUNCTION(BlueprintCallable, Category = VirtualKeyboard)
	void SetIsShift(bool NewIsShift);

	/** Called when the button is inputed */
	UPROPERTY(BlueprintAssignable, Category = "Button|Event")
	FOnKeyboardInputEvent OnInputed;
	
	/** A bindable delegate for OnInputed */
	UPROPERTY()
	FOnInputed OnInputedEventDelegate;

	//~ Begin UWidget Interface
	virtual void SynchronizeProperties() override;
	//~ End UWidget Interface

	UFUNCTION()
	virtual void HandleInputed();

private:
	PROPERTY_BINDING_IMPLEMENTATION(bool, bIsShift);
};

/**
 * 虚拟键盘
 */
UCLASS(Meta = (DisplayName = "VirtualKeyboard"))
class STUDIOGAME_API UVirtualKeyboard : public UUserWidget
{
	GENERATED_UCLASS_BODY()

	virtual void BeginDestroy() override;

	UFUNCTION(BlueprintGetter, Category = VirtualKeyboard)
	virtual bool IsShift() const;

	UFUNCTION(BlueprintGetter, Category = VirtualKeyboard)
	virtual bool OnInputed(const FString& Key);

	UFUNCTION(BlueprintGetter, Category = VirtualKeyboard)
	virtual bool OnDelete(const FString& Key);

	UFUNCTION(BlueprintGetter, Category = VirtualKeyboard)
	virtual bool OnShift(const FString& Key);

	UFUNCTION(BlueprintGetter, Category = VirtualKeyboard)
	virtual bool OnDetermine(const FString& Key);	

	UFUNCTION(BlueprintCallable, Category = VirtualKeyboard)
	virtual void Register(class UWidget* InTextWidget);

	UFUNCTION(BlueprintCallable, Category = VirtualKeyboard)
	virtual void Unregister();

protected:
	UPROPERTY(BlueprintReadOnly, Category = VirtualKeyboard)
	bool bIsShift;

	UPROPERTY()
	class UWidget* TextWidget;
};