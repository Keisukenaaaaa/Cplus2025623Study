// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/MyAbilitySystemComponentBase.h"

void UMyAbilitySystemComponentBase::AbilityActorInfoSet()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UMyAbilitySystemComponentBase::EffectApplied);
}

void UMyAbilitySystemComponentBase::EffectApplied(UAbilitySystemComponent* AbilitySystemComponent,
                                                  const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveGameplayEffectHandle)
{
	 GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Blue, FString("Effect Applied!"));
	// FGameplayTagContainer TagContainer;
	// EffectSpec.GetAllAssetTags(TagContainer);
	// for (const FGameplayTag& Tag : TagContainer)
	// {
	// 	const FString Msg = FString::Printf(TEXT("GE Tag: %s"),*Tag.ToString());
	// 	GEngine->AddOnScreenDebugMessage(-1,8.f,FColor::Green,Msg);
	// 	
	// }
	
}
