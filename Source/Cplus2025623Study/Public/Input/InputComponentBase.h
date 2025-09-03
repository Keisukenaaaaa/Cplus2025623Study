// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "InputConfig.h"
#include "InputComponentBase.generated.h"

/**
 * 
 */
UCLASS()
class CPLUS2025623STUDY_API UInputComponentBase : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:
	//整个代码这么设计的意义是?
	template<class UserClass,typename PressedFuncType,typename ReleasedFuncType,typename HoldFuncType>
	void BindAbilityAction(const UInputConfig* InputConfig,UserClass* Object,PressedFuncType PressedFunc,ReleasedFuncType ReleasedFunc,HoldFuncType HoldFunc);
	
};

//这个template是什么

//bindAction干嘛?为什么要在头文件里写方法
template<class UserClass,typename PressedFuncType,typename ReleasedFuncType,typename HoldFuncType>
	void UInputComponentBase ::BindAbilityAction(const UInputConfig* InputConfig,UserClass* Object,PressedFuncType PressedFunc,ReleasedFuncType ReleasedFunc,HoldFuncType HoldFunc)
{
	check(InputConfig);

	for (const FInputActionStruct& Action:InputConfig->AbilityInputActions)
	{
		if (Action.InputAction && Action.InputTag.IsValid())
		{
			if (PressedFunc)
			{
				BindAction(Action.InputAction,ETriggerEvent::Started,Object,PressedFunc,Action.InputTag);
			}
			if (HoldFunc)
			{
				BindAction(Action.InputAction,ETriggerEvent::Triggered,Object,HoldFunc,Action.InputTag);
			}
			if (ReleasedFunc)
			{
				BindAction(Action.InputAction,ETriggerEvent::Completed,Object,ReleasedFunc,Action.InputTag);
			}
		}
	}
}