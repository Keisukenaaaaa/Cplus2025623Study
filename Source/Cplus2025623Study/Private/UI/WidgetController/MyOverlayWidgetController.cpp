// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/MyOverlayWidgetController.h"

#include "AbilitySystem/MyAttributeSet.h"

void UMyOverlayWidgetController::BroadcastInitialValues()
{
	 const UMyAttributeSet* MyAttributeSet=CastChecked<UMyAttributeSet>(AttributeSet);//此处的const什么意思

	OnHealthChanged.Broadcast(MyAttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(MyAttributeSet->GetMaxHealth());
}
