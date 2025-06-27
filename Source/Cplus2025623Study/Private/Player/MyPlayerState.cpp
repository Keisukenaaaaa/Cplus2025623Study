// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/MyPlayerState.h"

#include "AbilitySystem/MyAbilitySystemComponentBase.h"
#include "AbilitySystem/MyAttributeSet.h"

AMyPlayerState::AMyPlayerState()
{
	

	AbilitySystemComponent = CreateDefaultSubobject<UMyAbilitySystemComponentBase>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true); //设置组件用于在网络上复制

	AttributeSet = CreateDefaultSubobject<UMyAttributeSet>("AttributeSet");
	NetUpdateFrequency = 100.f;
}

UAbilitySystemComponent* AMyPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
