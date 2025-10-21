// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "DamageTextComponent.generated.h"

/**
 * 
 */
UCLASS()
class CPLUS2025623STUDY_API UDamageTextComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:
//我们只需要增加一个函数，设置伤害的数值，可以在蓝图内实现此函数，并且可以在蓝图内调用
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetDamageText(float Damage,bool IsBlockedHit, bool IsCriticalHit); //设置伤害文字
};
