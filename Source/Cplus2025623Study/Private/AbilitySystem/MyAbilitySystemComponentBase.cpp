// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/MyAbilitySystemComponentBase.h"

#include "MyGameplayTags.h"
#include "AbilitySystem/Abilities/MyGameplayAbility.h"

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
		if (const UMyGameplayAbility* AbilityBase=Cast<UMyGameplayAbility>(AbilitySpec.Ability))
		{
			AbilitySpec.DynamicAbilityTags.AddTag(AbilityBase->StartupInputTag);
			GiveAbility(AbilitySpec);//只应用不激活?
		}
	//DynamicAbilityTags是一个FGameplayTagContainer类型，它也是可以复制到服务器的? 为什么这两段怎么能写成这样的?括号之间啥跟啥啊??
		//GiveAbilityAndActivateOnce(AbilitySpec);//应用并激活一次
		
	}
}

void UMyAbilitySystemComponentBase::AbilityInputTagHold(const FGameplayTag& InputTag)
{
	if(!InputTag.IsValid()) return;
	//判定InputTag是否可用
	for(auto AbilitySpec : GetActivatableAbilities())
		//遍历所有已经应用的技能并返回一个可以激活的列表
	{
		if(AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
			//对tag进行比对看看spec上的tag和传入的tag是否一致
		{
			AbilitySpecInputPressed(AbilitySpec);
			//在技能悬停事件里面，我们首先调用告知GameplayAbility，此技能被触发按下事件，
			if(!AbilitySpec.IsActive())
				//防止技能重复被激活，判断当前技能是否处于激活状态
			{
				TryActivateAbility(AbilitySpec.Handle);
			}
		}
	}
	//整段代码中的spec是哪来的??
}

void UMyAbilitySystemComponentBase::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if(!InputTag.IsValid()) return;
	for(auto AbilitySpec : GetActivatableAbilities())
	{
		if(AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			AbilitySpecInputReleased(AbilitySpec);
		}
	
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
