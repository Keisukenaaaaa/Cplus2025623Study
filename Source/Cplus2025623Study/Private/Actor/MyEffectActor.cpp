// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/MyEffectActor.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemStats.h"
#include "AbilitySystem/MyAbilitySystemComponentBase.h"
#include "AbilitySystem/MyAttributeSet.h"
#include "Character/PlayerCharacter.h"

#include "Components/SphereComponent.h"
#include "Player/MyPlayerState.h"


// Sets default values
AMyEffectActor::AMyEffectActor()
{
 
	PrimaryActorTick.bCanEverTick = false;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>("SceneRoot"));

	
}



void AMyEffectActor::BeginPlay()
{
	Super::BeginPlay();

	
	
}

void AMyEffectActor::ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass)
{
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (TargetASC == nullptr)return;
	check(GameplayEffectClass);

	
	//创建Effect的句柄 包含了实例化Effect 所需数据
	FGameplayEffectContextHandle EffectContextHandle = TargetASC->MakeEffectContext();
	//设置创建Effect的对象
	EffectContextHandle.AddSourceObject(this);
	//Effect的实例化后的句柄,可以通过此来寻找调用
	const FGameplayEffectSpecHandle EffectSpecHandle = TargetASC->MakeOutgoingSpec(GameplayEffectClass,ActorLevel,EffectContextHandle);
	//从句柄中获取到实例的地址,并被应用
	const FActiveGameplayEffectHandle ActiveGameplayEffectHandle=TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
	


	//从句柄中获取到实例的地址,并被应用
	const bool bIsInfinite =EffectSpecHandle.Data.Get()->Def.Get()->DurationPolicy == EGameplayEffectDurationType::Infinite;
	//在是无限时间效果和需要在结束时清除掉时,将效果句柄添加到map

	//FActiveGameplayEffectHandle ActiveHandle = TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
	if (bIsInfinite && InfinityEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
	{
		ActiveEffectHandles.Add(ActiveGameplayEffectHandle,TargetASC);
	}
}

void AMyEffectActor::OnOverlap(AActor* TargetActor)
{
	if (InstantEffectApplicationPolicy==EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor,InstantGameplayEffectClass);
	}
	if(DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, DurationGameplayEffectClass);
	}
	if (InfinityEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor,InfinityGameplayEffectClass);
	}

	
}


void AMyEffectActor::OnEndOverlap(AActor* TargetActor)
{
	if (InstantEffectApplicationPolicy==EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor,InstantGameplayEffectClass);
	}
	if(DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, DurationGameplayEffectClass);
	}
	if(InfinityEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, InfinityGameplayEffectClass);
	}

	
	//单独写的删除效果
	if (InfinityEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
	{
		UAbilitySystemComponent* TargetASC=UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
		if (!IsValid(TargetASC)) return;

		//创建存储需要一出的效果句柄 存储key ,用于遍历完成后remove效果
		TArray<FActiveGameplayEffectHandle> HandlesToRemove;
		//循环map内存的数据
		for (TTuple<FActiveGameplayEffectHandle, UAbilitySystemComponent*> HandlePair : ActiveEffectHandles)
		{
			if (TargetASC == HandlePair.Value)
			{
				//通过句柄将效果一出,注意,有可能有多层效果,不能一下把所有效果remove了;
				//此处尝试将 remove的层数变成可以蓝图里编辑的变量
				TargetASC->RemoveActiveGameplayEffect(HandlePair.Key,ActorRemoveTier);
				//添加到移除列表
				HandlesToRemove.Add(HandlePair.Key);
			}
		}

		for (auto& Handle : HandlesToRemove)
		{
			ActiveEffectHandles.FindAndRemoveChecked(Handle);
		}
	}

}



