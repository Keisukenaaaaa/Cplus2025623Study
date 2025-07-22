// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/MyOverlayWidgetController.h"

#include "AbilitySystem/MyAbilitySystemComponentBase.h"
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
	
	//ADDLambda 绑定匿名函数

	Cast<UMyAbilitySystemComponentBase>(AbilitySystemComponent)->EffectAssetTags.AddLambda
	([this](const FGameplayTagContainer& AssetTags)
	{
		for (const FGameplayTag& Tag : AssetTags)
		{
			//将tag广播给Widget Controller
			const FString Msg = FString::Printf(TEXT("GE Tag in Widget Controller: %s"), *Tag.ToString()); //获取Asset Tag
			GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Cyan, Msg); //打印到屏幕上 -1 不会被覆盖

			FUIWidgetRow* Row=GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable,Tag);
			
			// FGameplayTag MessageTag=FGameplayTag::RequestGameplayTag(FName("Message"));
			// if (Tag.MatchesTag(MessageTag))
			// {
			// 	FUIWidgetRow* Row=GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable,Tag);
			// 	MessageWidgetRowDelegate.B
			// }
			
		}
	}
		
	);

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
