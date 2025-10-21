// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "MyGameplayTags.h"
#include "RPGAbilityTypes.h"
#include "AbilitySystem/MyAbilitySystemBlueprintLibrary.h"
#include "Game/MyGameModeBase.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/GameplayStatics.h"

//这里结构体不加F是因为它是内部结构体，不需要外部获取，也不需要在蓝图中使用
struct SDamageStatics 
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration);
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitResistance);
	
	
	SDamageStatics()
	{
		//参数：1.属性集 2.属性名 3.目标还是自身 4.是否设置快照（true为创建时获取，false为应用时获取）
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMyAttributeSet, Armor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMyAttributeSet, ArmorPenetration, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMyAttributeSet, BlockChance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMyAttributeSet, CriticalHitChance, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMyAttributeSet, CriticalHitDamage, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMyAttributeSet, CriticalHitResistance, Target, false);
		
		
	}

};

static const SDamageStatics& DamageStatics()
{
	static SDamageStatics DStatics;
	return DStatics;
}


UExecCalc_Damage::UExecCalc_Damage()
{
	//最后一步，就是需要将属性添加到捕获列表中，我们在构造函数中添加即可。
	RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
	RelevantAttributesToCapture.Add(DamageStatics().BlockChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArmorPenetrationDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitResistanceDef);

}


//模板的内容，以后制作相关的，我们可以直接复制使用
void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	//获取ASC
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	//获取AvatarActor
	AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;

	//获取到战斗接口
	ICombatInterface* SourceCombatInterface = Cast<ICombatInterface>(SourceAvatar);
	ICombatInterface* TargetCombatInterface = Cast<ICombatInterface>(TargetAvatar);

	//获取挂载此类的GE实例
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	//设置评估参数
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	//获取到角色配置数据
	const UCharacterClassInfo* CharacterClassInfo = UMyAbilitySystemBlueprintLibrary::GetCharacterClassInfo(SourceAvatar);

	// //获取护甲
	// float Armor = 0.f;
	// ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvaluationParameters, Armor);
	// Armor = FMath::Max(0.f, Armor);
	//
	// //输出计算结果
	// const FGameplayModifierEvaluatedData EvaluatedData(DamageStatics().ArmorProperty, EGameplayModOp::Additive, Armor);
	// OutExecutionOutput.AddOutputModifier(EvaluatedData);

	//从Set by Caller 获取Damage的伤害值
	float Damage = Spec.GetSetByCallerMagnitude(FMyGameplayTags::Get().Damage);
	

	
	//--------------------处理格挡路--------------------
	//获取格挡率，如果触发格挡，伤害减少一半  先获取到伤害值,然后计算各种减伤 但他是什么时候输出的呢?所以我把输出函数先后移一下
	float TargetBlockChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BlockChanceDef, EvaluationParameters, TargetBlockChance);
	TargetBlockChance = FMath::Max(0.f, TargetBlockChance);
	
	//处理格挡触发
	const bool bBlocked = FMath::RandRange(1, 100) < TargetBlockChance;

	//获取GE的上下文句柄
	FGameplayEffectContextHandle EffectContextHandle = Spec.GetContext();
	//设置格挡
	UMyAbilitySystemBlueprintLibrary::SetIsBlockHit(EffectContextHandle, bBlocked);

	//切换集成到蓝图库里了
	// //我们通过Spec的函数GetContext获取句柄，并通过句柄的Get获取到Context
	// FGameplayEffectContext* EffectContext = Spec.GetContext().Get();
	//
	// //将FGameplayEffectContext转换成我们创建的自定义类型，然后在转换这里一定要用static_cast，不然会报错。	static_cast是强制类型转换操作符
	// FRPGGameplayEffectContext* RPGEffectContext = static_cast<FRPGGameplayEffectContext*>(EffectContext);
	//
	// //获取到自定义类型的context的上下文后，我们可以通过调用函数设置格挡
	// RPGEffectContext->SetIsBlockedHit(bBlocked);
	
	if(bBlocked) Damage *= 0.5f;


	//--------------------处理目标护甲和源的护甲穿透影响伤害--------------------
	//获取目标护甲值
	float TargetArmor = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvaluationParameters, TargetArmor);
	TargetArmor = FMath::Max(0.f, TargetArmor);
	//获取源护甲穿透
	float SourceArmorPenetration = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorPenetrationDef, EvaluationParameters, SourceArmorPenetration);
	SourceArmorPenetration = FMath::Max(0.f, SourceArmorPenetration);

	//获取到数据表内的护甲穿透系数
	const FRealCurve* ArmorPenetrationCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("ArmorPenetration"), FString());
	const float ArmorPenetrationCoefficient = ArmorPenetrationCurve->Eval(SourceCombatInterface->GetPlayerLevel());
	//护甲穿透将按照比例忽略目标的护甲值，护甲穿透的值可以根据某个方程计算出实际护甲穿透率（可以根据等级，等级越高，每一点护甲穿透值的比例越低）
	const float EffectiveArmor = TargetArmor * (100.f - SourceArmorPenetration * ArmorPenetrationCoefficient) / 100.f;
	//获取到数据表内的护甲系数
	const FRealCurve* EffectiveArmorCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("EffectiveArmor"), FString());
	const float EffectiveArmorCoefficient = EffectiveArmorCurve->Eval(TargetCombatInterface->GetPlayerLevel());

	//忽略后的护甲值将以一定比例影响伤害
	Damage *= (100.f - EffectiveArmor * EffectiveArmorCoefficient) / 100.f;
	
	//--------------------处理暴击伤害--------------------
	//获取目标暴击率
	float SourceCriticalHitChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitChanceDef, EvaluationParameters, SourceCriticalHitChance);
	SourceCriticalHitChance = FMath::Max(0.f, SourceCriticalHitChance);
	//获取暴击伤害CriticalHitDamageDef
	float SourceCriticalHitDamage = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitDamageDef, EvaluationParameters, SourceCriticalHitDamage);
	SourceCriticalHitDamage = FMath::Max(0.f, SourceCriticalHitDamage);
	//获取目标暴击防御CriticalHitResistanceDef
	float TargetCriticalHitResistance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitResistanceDef, EvaluationParameters, TargetCriticalHitResistance);
	TargetCriticalHitResistance = FMath::Max(0.f, TargetCriticalHitResistance);

	//获取到数据表内的暴击抵抗系数
	const FRealCurve* CriticalHitResistanceCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("CriticalHitResistance"), FString());
	const float CriticalHitResistanceCoefficient = CriticalHitResistanceCurve->Eval(SourceCombatInterface->GetPlayerLevel());

	//计算当前是否暴击
	const float EffectiveCriticalHitChance = SourceCriticalHitChance - TargetCriticalHitResistance * CriticalHitResistanceCoefficient;
	const bool bCriticalHit = FMath::RandRange(1, 100) < EffectiveCriticalHitChance;

	//设置暴击
	UMyAbilitySystemBlueprintLibrary::SetIsCriticalHit(EffectContextHandle, bCriticalHit);
	
	//触发暴击 伤害乘以暴击伤害率
	if(bCriticalHit) Damage = Damage * 2.f + SourceCriticalHitDamage;




	//输出计算结果
	const FGameplayModifierEvaluatedData EvaluatedData(UMyAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);


	// 调试信息：基础信息
    if (GEngine)
    {
        FString DebugString = FString::Printf(TEXT("=== 伤害计算调试信息 ==="));
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, DebugString);
        
        DebugString = FString::Printf(TEXT("源角色: %s, 目标角色: %s"), 
            SourceAvatar ? *SourceAvatar->GetName() : TEXT("Null"),
            TargetAvatar ? *TargetAvatar->GetName() : TEXT("Null"));
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, DebugString);
        
        // 显示等级信息
        if (SourceCombatInterface && TargetCombatInterface)
        {
            DebugString = FString::Printf(TEXT("源等级: %d, 目标等级: %d"), 
                SourceCombatInterface->GetPlayerLevel(), 
                TargetCombatInterface->GetPlayerLevel());
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, DebugString);
        }
        
        // 显示防御属性
        DebugString = FString::Printf(TEXT("目标护甲: %.2f, 目标格挡率: %.2f%%"), 
            TargetArmor, TargetBlockChance);
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, DebugString);
        
        DebugString = FString::Printf(TEXT("源护甲穿透: %.2f"), SourceArmorPenetration);
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, DebugString);
        
        // 显示计算过程中的值
        DebugString = FString::Printf(TEXT("护甲穿透系数: %.4f, 有效护甲系数: %.4f"), 
            ArmorPenetrationCoefficient, EffectiveArmorCoefficient);
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, DebugString);
        
        DebugString = FString::Printf(TEXT("有效护甲: %.2f"), EffectiveArmor);
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, DebugString);
        
        // 显示格挡状态
        DebugString = FString::Printf(TEXT("格挡触发: %s"), bBlocked ? TEXT("是") : TEXT("否"));
        GEngine->AddOnScreenDebugMessage(-1, 5.f, bBlocked ? FColor::Red : FColor::Green, DebugString);
    	// 显示暴击相关属性
    	DebugString = FString::Printf(TEXT("暴击属性 - 源暴击率: %.2f%%, 源暴击伤害: %.2f, 目标暴击抵抗: %.2f"), 
			SourceCriticalHitChance, SourceCriticalHitDamage, TargetCriticalHitResistance);
    	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, DebugString);

    	// 显示暴击计算系数和有效值
    	DebugString = FString::Printf(TEXT("暴击系数 - 抵抗系数: %.4f, 有效暴击率: %.2f%%"), 
			CriticalHitResistanceCoefficient, EffectiveCriticalHitChance);
    	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Magenta, DebugString);

    	// 显示暴击触发状态
    	DebugString = FString::Printf(TEXT("暴击触发: %s"), bCriticalHit ? TEXT("是") : TEXT("否"));
    	GEngine->AddOnScreenDebugMessage(-1, 5.f, bCriticalHit ? FColor::Red : FColor::Blue, DebugString);

    	if (bCriticalHit)
    	{
    		DebugString = FString::Printf(TEXT("暴击伤害加成: +%.2f"), SourceCriticalHitDamage);
    		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, DebugString);
    	}
        
        // 最终伤害
        DebugString = FString::Printf(TEXT("基础伤害: %.2f, 最终伤害: %.2f"), 
            Spec.GetSetByCallerMagnitude(FMyGameplayTags::Get().Damage), Damage);
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, DebugString);
    }


	
}
