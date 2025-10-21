// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemGlobals.h"
#include "RPGAbilitySystemGlobals.generated.h"

/**
 * 
 */
UCLASS()
class CPLUS2025623STUDY_API URPGAbilitySystemGlobals : public UAbilitySystemGlobals
{
	GENERATED_BODY()

	//为什么此处的句式不用添加public这类限定
	virtual FGameplayEffectContext* AllocGameplayEffectContext() const override;
	
};
