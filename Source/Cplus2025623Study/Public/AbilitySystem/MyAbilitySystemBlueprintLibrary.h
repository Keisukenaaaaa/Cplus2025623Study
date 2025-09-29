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


	
};
