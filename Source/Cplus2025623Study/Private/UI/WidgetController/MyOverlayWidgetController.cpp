// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/MyOverlayWidgetController.h"

#include "AbilitySystem/MyAttributeSet.h"

void UMyOverlayWidgetController::BroadcastInitialValues()
{
	 const UMyAttributeSet* MyAttributeSet=CastChecked<UMyAttributeSet>(AttributeSet);//此处的const什么意思

	OnHealthChanged.Broadcast(MyAttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(MyAttributeSet->GetMaxHealth());
	OnManaChanged.Broadcast(MyAttributeSet->GetMana());
	OnMaxManaChanged.Broadcast(MyAttributeSet->GetMaxMana());

	
}

void UMyOverlayWidgetController::BindCallbacksToDependencies()
{
	const UMyAttributeSet* MyAttributeSetBase = CastChecked<UMyAttributeSet>(AttributeSet);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		MyAttributeSetBase->GetHealthAttribute()).AddUObject(this, &UMyOverlayWidgetController::HealthChanged);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		MyAttributeSetBase->GetMaxHealthAttribute()).AddUObject(this, &UMyOverlayWidgetController::MaxHealthChanged);
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		MyAttributeSetBase->GetManaAttribute()).AddUObject(this, &UMyOverlayWidgetController::ManaChanged);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		MyAttributeSetBase->GetMaxManaAttribute()).AddUObject(this, &UMyOverlayWidgetController::MaxManaChanged);
	
                        

}

void UMyOverlayWidgetController::HealthChanged(const FOnAttributeChangeData& Data) const 
{
	OnHealthChanged.Broadcast(Data.NewValue);
}

void UMyOverlayWidgetController::MaxHealthChanged(const FOnAttributeChangeData& Data) const
{
	OnMaxHealthChanged.Broadcast(Data.NewValue);
}

void UMyOverlayWidgetController::ManaChanged(const FOnAttributeChangeData& Data) const
{
	OnManaChanged.Broadcast(Data.NewValue);
}

void UMyOverlayWidgetController::MaxManaChanged(const FOnAttributeChangeData& Data) const
{
	OnMaxManaChanged.Broadcast(Data.NewValue);
}
