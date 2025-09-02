// Fill out your copyright notice in the Description page of Project Settings.


#include "Input/InputConfig.h"


const UInputAction* UInputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const//这里的bool是怎么来的 怎么用的???
{
	for (const FInputActionStruct& Action : AbilityInputActions)
	{
		if (Action.InputAction&&Action.InputTag==InputTag)
		{
			return Action.InputAction;
		}
	}
	if (bLogNotFound)
	{
		UE_LOG(LogTemp, Error, TEXT("无法从InputConfig[%s]中找到InputTag[%s]对应的技能InputAction"), *GetNameSafe(this), *InputTag.ToString());
		//什么是getnamesafe?
	}
	return nullptr;
}
