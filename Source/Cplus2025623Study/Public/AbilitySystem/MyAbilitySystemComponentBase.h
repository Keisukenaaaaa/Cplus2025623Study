// Fill out your copyright notice in the Description page of Project Settings.



#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "MyAbilitySystemComponentBase.generated.h"


DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags,const FGameplayTagContainer& /*AssetTag*/)

/**
 * 
 */
UCLASS()
class CPLUS2025623STUDY_API UMyAbilitySystemComponentBase : public UAbilitySystemComponent
{
	GENERATED_BODY()


	public:
	void AbilityActorInfoSet();

	FEffectAssetTags EffectAssetTags;

	protected:
	void EffectApplied(UAbilitySystemComponent* AbilitySystemComponent,const FGameplayEffectSpec& EffectSpec,FActiveGameplayEffectHandle ActiveGameplayEffectHandle);
};
