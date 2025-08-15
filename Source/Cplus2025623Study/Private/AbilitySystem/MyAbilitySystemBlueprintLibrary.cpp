// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/MyAbilitySystemBlueprintLibrary.h"


#include "Kismet/GameplayStatics.h"
#include "Player/MyPlayerState.h"
#include "UI/HUD/MyHUD.h"

UMyOverlayWidgetController* UMyAbilitySystemBlueprintLibrary::GetOverlayWidgetController(
	const UObject* WorldContextObject)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		if (AMyHUD*HUD=Cast<AMyHUD>(PC->GetHUD()))
		{
			AMyPlayerState*PS=PC->GetPlayerState<AMyPlayerState>();//此处可能有问题 为什么这些地方填写对应的 数据类型 有的是自定义 有的是官方的
			UAbilitySystemComponent* ASC=PS->GetAbilitySystemComponent();
			UAttributeSet*AS = PS->GetAttributeSet();
			const FWidgetControllerParams widgetcontrollerparams(PC,PS,ASC,AS);
			return HUD->GetOverlayWidgetController(widgetcontrollerparams);
			
		}
	}
	return nullptr;
}

UAttributeMenuWidgetController* UMyAbilitySystemBlueprintLibrary::GetUAttributeMenuWidgetController(
	const UObject* WorldContextObject)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		if (AMyHUD*HUD=Cast<AMyHUD>(PC->GetHUD()))
		{
			AMyPlayerState*PS=PC->GetPlayerState<AMyPlayerState>();//此处可能有问题
			UAbilitySystemComponent* ASC=PS->GetAbilitySystemComponent();
			UAttributeSet*AS = PS->GetAttributeSet();
			const FWidgetControllerParams widgetcontrollerparams(PC,PS,ASC,AS);
			return HUD->GetAttributeMenuWidgetController(widgetcontrollerparams);
			
		}
	}
	return nullptr;
}
