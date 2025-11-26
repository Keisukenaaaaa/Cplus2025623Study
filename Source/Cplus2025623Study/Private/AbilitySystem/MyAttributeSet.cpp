// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/MyAttributeSet.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayEffectExtension.h"
#include "MyGameplayTags.h"
#include "AbilitySystem/MyAbilitySystemBlueprintLibrary.h"
#include "GameFramework/Character.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/GameplayStatics.h"
#include"Net/UnrealNetwork.h"
#include "Player/MyPlayerController.h"

void UMyAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	if(Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
	
	if(Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMana());
	}
}





void UMyAttributeSet::SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Props)
{
	//source 效果的所有者 Target 效果应用的目标
	Props.EffectContextHandle=Data.EffectSpec.GetContext();
	Props.SourceASC=Props.EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();

	//获取效果所有者的相关对象
	if(IsValid(Props.SourceASC) && Props.SourceASC->AbilityActorInfo.IsValid() && Props.SourceASC->AbilityActorInfo->AvatarActor.IsValid())
	{
		Props.SourceAvatarActor = Props.SourceASC->AbilityActorInfo->AvatarActor.Get(); //获取Actor
		Props.SourceController = Props.SourceASC->AbilityActorInfo->PlayerController.Get(); //获取PlayerController
		if(Props.SourceController == nullptr && Props.SourceAvatarActor != nullptr)
		{
			if(const APawn* Pawn = Cast<APawn>(Props.SourceAvatarActor))
			{
				Props.SourceController = Pawn->GetController();
			}
		}
	
		if(Props.SourceController)
		{
			Props.SourceCharacter = Cast<ACharacter>(Props.SourceController->GetPawn());
		}
	}
	
	if(Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		Props.TargetAvatarActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		Props.TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		Props.TargetCharacter = Cast<ACharacter>(Props.TargetAvatarActor);
		Props.TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Props.TargetAvatarActor);
	}
}

void UMyAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	FEffectProperties Props;
	SetEffectProperties(Data,Props);
	
	if(Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(  FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
		UE_LOG(LogTemp, Warning, TEXT("%s 的生命值发生了修改，当前生命值：%f"), *Props.TargetAvatarActor->GetName(), GetHealth());//测试用
	}

	if(Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		SetMana ( FMath::Clamp(GetMana(), 0.f, GetMaxMana()));
	}
	//自己写代码的时候把这个提前了然后把生命赋予注释了,导致完全没有初始生命
	if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		const float LocalInComingDamage=GetIncomingDamage();
		
		SetIncomingDamage(0.f);
		
		if (LocalInComingDamage>0.f)
		{
			const float NewHealth=GetHealth()-LocalInComingDamage;
			SetHealth(  FMath::Clamp(NewHealth, 0.f, GetMaxHealth()));
			const bool bFatal= NewHealth <=0.f;//如果血量小于0 角色将会死亡
			//测试Tag--hitReact所用 完全不懂
			// if(!bFatal)
			// {
			// 	FGameplayTagContainer TagContainer;
			// 	TagContainer.AddTag(FMyGameplayTags::Get().Effects_HitReact);
			// 	Props.TargetASC->TryActivateAbilitiesByTag(TagContainer); //根据tag标签激活技能
			// }
			if (bFatal)
			{
				//调用死亡函数
				ICombatInterface* CombatInterface=Cast<ICombatInterface>(Props.TargetAvatarActor);
				if (CombatInterface)
				{
					CombatInterface->Die();
				}
			}
			//如果没死 激活受击技能
			else
			{
				//原来这就是激活受击技能的代码
				FGameplayTagContainer TagContainer;
				TagContainer.AddTag(FMyGameplayTags::Get().Effects_HitReact);
				//应用受击前取消前次受击,从而尝试新的受击激活
				Props.TargetASC->CancelAbilities(&TagContainer);
				Props.TargetASC->TryActivateAbilitiesByTag(TagContainer); //根据tag标签激活技能
			}

			//获取格挡和暴击
			const bool IsBlockedHit = UMyAbilitySystemBlueprintLibrary::IsBlockedHit(Props.EffectContextHandle);
			const bool IsCriticalHit = UMyAbilitySystemBlueprintLibrary::IsCriticalHit(Props.EffectContextHandle);
		
			ShowFloatingText(Props,LocalInComingDamage,IsBlockedHit,IsCriticalHit);
			//这些个Props是什么 前面是定义了,但是是哪来的有什么用
		}
	}
}
void UMyAttributeSet::ShowFloatingText(const FEffectProperties& Props, const float Damage, bool IsBlockedHit, bool IsCriticalHit)
{
	//调用显示伤害数字
	if(Props.SourceCharacter != Props.TargetCharacter)
	{
		// //然后获取到目标的PlayerController，调用函数即可
		// if(AMyPlayerController* PC = Cast<AMyPlayerController>(UGameplayStatics::GetPlayerController(Props.SourceCharacter, 0)))
		// {
		// 	PC->ShowDamageNumber(Damage, Props.TargetCharacter,IsBlockedHit,IsCriticalHit); //调用显示伤害数字
		// }改为下文可以同步到客户端版无法直接使用静态函数去获取，我们可以直接从SourceCharacter上面获取

		if(AMyPlayerController* PC = Cast<AMyPlayerController>(Props.SourceCharacter->Controller))
		{
			PC->ShowDamageNumber(Damage,Props.TargetCharacter,IsBlockedHit,IsCriticalHit);//调用显示伤害数字
		}
	}
}





UMyAttributeSet::UMyAttributeSet()
{
	const FMyGameplayTags& GameplayTags = FMyGameplayTags::Get();

	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Strength,GetStrengthAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Intelligence,GetIntelligenceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Vigor,GetVigorAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Resilience,GetResilienceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_Armor,GetArmorAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_ArmorPenetration,GetArmorPenetrationAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_BlockChance,GetBlockChanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_CriticalHitDamage,GetCriticalHitDamageAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_CriticalHitChance,GetCriticalHitChanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_CriticalHitResistance,GetCriticalHitResistanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_HealthRegeneration,GetHealthRegenerationAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_ManaRegeneration,GetManaRegenerationAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_MaxHealth,GetMaxHealthAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_MaxMana,GetMaxManaAttribute);
	/*
	 * Resistance Attribute
	 */
	TagsToAttributes.Add(GameplayTags.Attributes_Resistance_Fire, GetFireResistanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Resistance_Lightning, GetLightningResistanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Resistance_Arcane, GetArcaneResistanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Resistance_Physical, GetPhysicalResistanceAttribute);

}

void UMyAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);//这个super的对象怎么确定的
	//主要属性
	DOREPLIFETIME_CONDITION_NOTIFY(UMyAttributeSet, Strength, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMyAttributeSet, Intelligence, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMyAttributeSet, Resilience, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMyAttributeSet, Vigor, COND_None, REPNOTIFY_Always);
	//次级属性

	DOREPLIFETIME_CONDITION_NOTIFY(UMyAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UMyAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMyAttributeSet, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMyAttributeSet, ArmorPenetration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMyAttributeSet, BlockChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMyAttributeSet, CriticalHitChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMyAttributeSet, CriticalHitDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMyAttributeSet, CriticalHitResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMyAttributeSet, HealthRegeneration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMyAttributeSet, ManaRegeneration, COND_None, REPNOTIFY_Always);
	//vital attribute
	DOREPLIFETIME_CONDITION_NOTIFY(UMyAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMyAttributeSet, Mana, COND_None, REPNOTIFY_Always);
	//抗性属性
	DOREPLIFETIME_CONDITION_NOTIFY(UMyAttributeSet, FireResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMyAttributeSet, LightningResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMyAttributeSet, ArcaneResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMyAttributeSet, PhysicalResistance, COND_None, REPNOTIFY_Always);
	
}

void UMyAttributeSet::OnRep_Strength(const FGameplayAttributeData& OldStrength) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMyAttributeSet, Strength, OldStrength);
}

void UMyAttributeSet::OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMyAttributeSet, Intelligence, OldIntelligence);
}

void UMyAttributeSet::OnRep_Resilience(const FGameplayAttributeData& OldResilience) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMyAttributeSet, Resilience, OldResilience);
}

void UMyAttributeSet::OnRep_Vigor(const FGameplayAttributeData& OldVigor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMyAttributeSet, Vigor, OldVigor);
}

void UMyAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMyAttributeSet, Health, OldHealth);
}

void UMyAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMyAttributeSet, MaxHealth, OldMaxHealth);
}

void UMyAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMyAttributeSet, Mana, OldMana);
}


void UMyAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMyAttributeSet, MaxMana, OldMaxMana);
}

void UMyAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldArmor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMyAttributeSet, Armor, OldArmor);
}

void UMyAttributeSet::OnRep_ArmorPenetration(const FGameplayAttributeData& OldArmorPenetration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMyAttributeSet, ArmorPenetration, OldArmorPenetration);
}

void UMyAttributeSet::OnRep_BlockChance(const FGameplayAttributeData& OldBlockChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMyAttributeSet, BlockChance, OldBlockChance);
}

void UMyAttributeSet::OnRep_CriticalHitChance(const FGameplayAttributeData& OldCriticalHitChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMyAttributeSet, CriticalHitChance, OldCriticalHitChance);
}

void UMyAttributeSet::OnRep_CriticalHitDamage(const FGameplayAttributeData& OldCriticalHitDamage) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMyAttributeSet, CriticalHitDamage, OldCriticalHitDamage);
}

void UMyAttributeSet::OnRep_CriticalHitResistance(const FGameplayAttributeData& OldCriticalHitResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMyAttributeSet, CriticalHitResistance, OldCriticalHitResistance);
}

void UMyAttributeSet::OnRep_HealthRegeneration(const FGameplayAttributeData& OldHealthRegeneration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMyAttributeSet, HealthRegeneration, OldHealthRegeneration);
}

void UMyAttributeSet::OnRep_ManaRegeneration(const FGameplayAttributeData& OldManaRegeneration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMyAttributeSet, ManaRegeneration, OldManaRegeneration);
}
void UMyAttributeSet::OnRep_FireResistance(const FGameplayAttributeData& OldFireResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMyAttributeSet, FireResistance, OldFireResistance);
}

void UMyAttributeSet::OnRep_LightningResistance(const FGameplayAttributeData& OldLightningResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMyAttributeSet, LightningResistance, OldLightningResistance);
}

void UMyAttributeSet::OnRep_ArcaneResistance(const FGameplayAttributeData& OldArcaneResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMyAttributeSet, ArcaneResistance, OldArcaneResistance);
}

void UMyAttributeSet::OnRep_PhysicalResistance(const FGameplayAttributeData& OldPhysicalResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMyAttributeSet, PhysicalResistance, OldPhysicalResistance);
}


