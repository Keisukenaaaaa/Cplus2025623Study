// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ModMagCalc/MMC_MaxMana.h"

#include "AbilitySystem/MyAttributeSet.h"
#include "Interaction/CombatInterface.h"

UMMC_MaxMana::UMMC_MaxMana()
{
	IntelligenceDef.AttributeToCapture=UMyAttributeSet::GetIntelligenceAttribute();//设置需要获取的属性对象;
	IntelligenceDef.AttributeSource=EGameplayEffectAttributeCaptureSource::Target;//设置拾取对象为GE的应用目标
	IntelligenceDef.bSnapshot=false;

	RelevantAttributesToCapture.Add(IntelligenceDef);//添加捕获到的数值,只有添加到列表,才回去获取
}

float UMMC_MaxMana::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	//从source 和 target中获取tag
	const FGameplayTagContainer* SourceTags=Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags=Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;//明天问元宝
	EvaluateParameters.SourceTags=SourceTags;
	EvaluateParameters.TargetTags=TargetTags;

	float Intelligence=0.f;
	GetCapturedAttributeMagnitude(IntelligenceDef,Spec,EvaluateParameters,Intelligence);
	Intelligence=FMath::Max<float>(Intelligence,0.f);//将Intelligence的影响控制在最小为0

	//获取等级
	ICombatInterface* CombatInterface=Cast<ICombatInterface>(Spec.GetContext().GetSourceObject());
	const int32 Level=CombatInterface->GetPlayerLevel();

	return 50.f+Intelligence*2.5f+Level*15.f;
}