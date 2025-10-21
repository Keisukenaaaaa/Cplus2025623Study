// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MyAbilitySystemBlueprintLibrary.generated.h"

/**
 * 
 */

class UAttributeMenuWidgetController;
class UMyOverlayWidgetController;

UCLASS()
class CPLUS2025623STUDY_API UMyAbilitySystemBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintPure,Category="MyAbilitySystemLibrary|WidgetController")
	static UMyOverlayWidgetController* GetOverlayWidgetController(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure,Category="MyAbilitySystemLibrary|WidgetController")
	static UAttributeMenuWidgetController* GetUAttributeMenuWidgetController(const UObject* WorldContextObject);

	//初始化角色的属性
	UFUNCTION(BlueprintCallable, Category="MyAbilitySystemLibrary|CharacterClassDefaults")
	static void InitializeDefaultAttributes(const UObject* WorldContextObject, ECharacterClass CharacterClass, float Level, UAbilitySystemComponent* ASC);

	//初始化角色的技能
	UFUNCTION(BlueprintCallable, Category="MyAbilitySystemLibrary|CharacterClassDefaults")
	static void GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC);

	//获取角色配置数据
	UFUNCTION(BlueprintCallable, Category="MyAbilitySystemLibrary|CharacterClassDefaults")
	static UCharacterClassInfo* GetCharacterClassInfo(const UObject* WorldContextObject);

	//获取当前GE是否触发格挡
	UFUNCTION(BlueprintPure, Category="MyAbilitySystemLibrary|GameplayEffects")
	static bool IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle);

	//获取当前GE是否触发暴击
	UFUNCTION(BlueprintPure, Category="MyAbilitySystemLibrary|GameplayEffects")
	static bool IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle);

	//我们还要在函数库实现设置的两个函数，这里有个问题，就是没办法传入常量（前面加const）会出现问题，我们将函数编写完成以后编译在UE里面查看
	//设置需要传入两个值，一个是修改的Context的Handle，另一个则是bool值，用于设置的值，我们无法设置成静态函数，也就是
	// UFUNCTION(BlueprintCallable, Category="MyAbilitySystemLibrary|GameplayEffects")
	// static void SetIsBlockHit(FGameplayEffectContextHandle& EffectContextHandle, bool bInIsBlockedHit);
	//
	// UFUNCTION(BlueprintCallable, Category="MyAbilitySystemLibrary|GameplayEffects")
	// static void SetIsCriticalHit(FGameplayEffectContextHandle& EffectContextHandle, bool bInIsCriticalHit);

	//声明函数时，我们在前面添加UPARAM(ref)，即可实现在蓝图节点的左侧
	UFUNCTION(BlueprintCallable, Category="MyAbilitySystemLibrary|GameplayEffects")
	static void SetIsBlockHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInIsBlockedHit);

	UFUNCTION(BlueprintCallable, Category="MyAbilitySystemLibrary|GameplayEffects")
	static void SetIsCriticalHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInIsCriticalHit);



	



	
};
