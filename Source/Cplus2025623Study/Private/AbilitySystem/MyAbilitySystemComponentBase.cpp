// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/MyAbilitySystemComponentBase.h"

#include "MyGameplayTags.h"

void UMyAbilitySystemComponentBase::AbilityActorInfoSet()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UMyAbilitySystemComponentBase::EffectApplied);

	// const FMyGameplayTags& GameplayTags = FMyGameplayTags::Get();
	// GEngine->AddOnScreenDebugMessage
	// (-1,
	// 	10.f,
	// 	FColor::Orange,
	// 	FString::Printf(TEXT("Tag: %s"),*GameplayTags.Attributes_Secondary_Armor.ToString())
	// 	);调试用
}

void UMyAbilitySystemComponentBase::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities)
{
	for (TSubclassOf<UGameplayAbility>AbilityClass : StartupAbilities)
	{
		FGameplayAbilitySpec AbilitySpec=FGameplayAbilitySpec(AbilityClass,1);
//GiveAbility(AbilitySpec)只应用不激活?
		GiveAbilityAndActivateOnce(AbilitySpec);//应用并激活一次
		
	}
}

void UMyAbilitySystemComponentBase::EffectApplied(UAbilitySystemComponent* AbilitySystemComponent,
                                                  const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveGameplayEffectHandle)
{
	 //GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Blue, FString("Effect Applied!"));
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer);

	EffectAssetTags.Broadcast(TagContainer);
	
}
