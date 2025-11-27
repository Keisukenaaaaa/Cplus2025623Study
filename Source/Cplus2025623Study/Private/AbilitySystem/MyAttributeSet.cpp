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
	//source 鏁堟灉鐨勬墍鏈夎€?Target 鏁堟灉搴旂敤鐨勭洰鏍?
	Props.EffectContextHandle=Data.EffectSpec.GetContext();
	Props.SourceASC=Props.EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();

	//鑾峰彇鏁堟灉鎵€鏈夎€呯殑鐩稿叧瀵硅薄
	if(IsValid(Props.SourceASC) && Props.SourceASC->AbilityActorInfo.IsValid() && Props.SourceASC->AbilityActorInfo->AvatarActor.IsValid())
	{
		Props.SourceAvatarActor = Props.SourceASC->AbilityActorInfo->AvatarActor.Get(); //鑾峰彇Actor
		Props.SourceController = Props.SourceASC->AbilityActorInfo->PlayerController.Get(); //鑾峰彇PlayerController
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
		UE_LOG(LogTemp, Warning, TEXT("%s 鐨勭敓鍛藉€煎彂鐢熶簡淇敼锛屽綋鍓嶇敓鍛藉€硷細%f"), *Props.TargetAvatarActor->GetName(), GetHealth());//娴嬭瘯鐢?
	}

	if(Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		SetMana ( FMath::Clamp(GetMana(), 0.f, GetMaxMana()));
	}
	//鑷繁鍐欎唬鐮佺殑鏃跺€欐妸杩欎釜鎻愬墠浜嗙劧鍚庢妸鐢熷懡璧嬩簣娉ㄩ噴浜?瀵艰嚧瀹屽叏娌℃湁鍒濆鐢熷懡
	if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		const float LocalInComingDamage=GetIncomingDamage();
		
		SetIncomingDamage(0.f);
		
		if (LocalInComingDamage>0.f)
		{
			const float NewHealth=GetHealth()-LocalInComingDamage;
			SetHealth(  FMath::Clamp(NewHealth, 0.f, GetMaxHealth()));
			const bool bFatal= NewHealth <=0.f;//濡傛灉琛€閲忓皬浜? 瑙掕壊灏嗕細姝讳骸
			//娴嬭瘯Tag--hitReact鎵€鐢?瀹屽叏涓嶆噦
			// if(!bFatal)
			// {
			// 	FGameplayTagContainer TagContainer;
			// 	TagContainer.AddTag(FMyGameplayTags::Get().Effects_HitReact);
			// 	Props.TargetASC->TryActivateAbilitiesByTag(TagContainer); //鏍规嵁tag鏍囩婵€娲绘妧鑳?
			// }
			if (bFatal)
			{
				//璋冪敤姝讳骸鍑芥暟
				ICombatInterface* CombatInterface=Cast<ICombatInterface>(Props.TargetAvatarActor);
				if (CombatInterface)
				{
					CombatInterface->Die();
				}
			}
			//濡傛灉娌℃ 婵€娲诲彈鍑绘妧鑳?
			else
			{
				//鍘熸潵杩欏氨鏄縺娲诲彈鍑绘妧鑳界殑浠ｇ爜
				FGameplayTagContainer TagContainer;
				TagContainer.AddTag(FMyGameplayTags::Get().Effects_HitReact);
				//搴旂敤鍙楀嚮鍓嶅彇娑堝墠娆″彈鍑?浠庤€屽皾璇曟柊鐨勫彈鍑绘縺娲?
				Props.TargetASC->CancelAbilities(&TagContainer);
				Props.TargetASC->TryActivateAbilitiesByTag(TagContainer); //鏍规嵁tag鏍囩婵€娲绘妧鑳?
			}

			//鑾峰彇鏍兼尅鍜屾毚鍑?
			const bool IsBlockedHit = UMyAbilitySystemBlueprintLibrary::IsBlockedHit(Props.EffectContextHandle);
			const bool IsCriticalHit = UMyAbilitySystemBlueprintLibrary::IsCriticalHit(Props.EffectContextHandle);
		
			ShowFloatingText(Props,LocalInComingDamage,IsBlockedHit,IsCriticalHit);
			//杩欎簺涓狿rops鏄粈涔?鍓嶉潰鏄畾涔変簡,浣嗘槸鏄摢鏉ョ殑鏈変粈涔堢敤
		}
	}
}
void UMyAttributeSet::ShowFloatingText(const FEffectProperties& Props, const float Damage, bool IsBlockedHit, bool IsCriticalHit)
{
	//调用显示伤害数字
	if(Props.SourceCharacter != Props.TargetCharacter)
	{
		//优先尝试在源角色的本地控制器上显示
		if(AMyPlayerController* PC = Cast<AMyPlayerController>(Props.SourceCharacter->GetController()))
		{
			PC->ShowDamageNumber(Damage, Props.TargetCharacter,IsBlockedHit,IsCriticalHit);
			return;
		}
		//否则尝试使用目标角色的控制器（例如被本地客户端拥有时）
		if(AMyPlayerController* PC = Cast<AMyPlayerController>(Props.TargetCharacter->GetController()))
		{
			PC->ShowDamageNumber(Damage, Props.TargetCharacter,IsBlockedHit,IsCriticalHit);
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
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);//杩欎釜super鐨勫璞℃€庝箞纭畾鐨?
	//涓昏灞炴€?
	DOREPLIFETIME_CONDITION_NOTIFY(UMyAttributeSet, Strength, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMyAttributeSet, Intelligence, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMyAttributeSet, Resilience, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMyAttributeSet, Vigor, COND_None, REPNOTIFY_Always);
	//娆＄骇灞炴€?

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
	//鎶楁€у睘鎬?
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



