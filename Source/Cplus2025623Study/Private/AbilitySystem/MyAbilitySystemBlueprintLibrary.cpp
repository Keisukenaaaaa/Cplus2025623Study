// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/MyAbilitySystemBlueprintLibrary.h"


#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Game/MyGameModeBase.h"
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

void UMyAbilitySystemBlueprintLibrary::InitializeDefaultAttributes(const UObject* WorldContextObject,
	ECharacterClass CharacterClass, float Level, UAbilitySystemComponent* ASC)
{
	//获取到当前关卡的GameMode实例
	const AMyGameModeBase* GameMode = Cast<AMyGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if(GameMode == nullptr) return;
	//从实例获取到关卡角色的配置
	UCharacterClassInfo* ClassInfo = GameMode->CharacterClassInfo;
	//获取到默认的基础角色数据
	const FCharacterClassDefaultInfo ClassDefaultInfo = ClassInfo->GetClassDefaultInfo(CharacterClass);

	//应用基础属性  这里面每句话什么意思
	FGameplayEffectContextHandle PrimaryContextHandle = ASC->MakeEffectContext();
	PrimaryContextHandle.AddSourceObject(WorldContextObject);
	const FGameplayEffectSpecHandle PrimarySpecHandle = ASC->MakeOutgoingSpec(ClassDefaultInfo.PrimaryAttributes, Level, PrimaryContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*PrimarySpecHandle.Data.Get());

	//设置次级属性
	FGameplayEffectContextHandle SecondaryContextHandle = ASC->MakeEffectContext();
	SecondaryContextHandle.AddSourceObject(WorldContextObject);
	const FGameplayEffectSpecHandle SecondarySpecHandle = ASC->MakeOutgoingSpec(ClassInfo->SecondaryAttributes, Level, SecondaryContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*SecondarySpecHandle.Data.Get());

	//填充血量和蓝量
	FGameplayEffectContextHandle VitalContextHandle = ASC->MakeEffectContext();
	VitalContextHandle.AddSourceObject(WorldContextObject);
	const FGameplayEffectSpecHandle VitalSpecHandle = ASC->MakeOutgoingSpec(ClassInfo->VitalAttributes, Level, VitalContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*VitalSpecHandle.Data.Get());


}
