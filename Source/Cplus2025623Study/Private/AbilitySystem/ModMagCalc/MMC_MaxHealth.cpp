// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ModMagCalc/MMC_MaxHealth.h"

#include "AbilitySystem/MyAttributeSet.h"
#include "Interaction/CombatInterface.h"

UMMC_MaxHealth::UMMC_MaxHealth()
{
	VigorDef.AttributeToCapture=UMyAttributeSet::GetVigorAttribute();//设置需要获取的属性对象;
	VigorDef.AttributeSource=EGameplayEffectAttributeCaptureSource::Target;//设置拾取对象为GE的应用目标
	VigorDef.bSnapshot=false;

	RelevantAttributesToCapture.Add(VigorDef);//添加捕获到的数值,只有添加到列表,才回去获取
}

float UMMC_MaxHealth::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	//从source 和 target中获取tag
	const FGameplayTagContainer* SourceTags=Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags=Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;//明天问元宝
	EvaluateParameters.SourceTags=SourceTags;
	EvaluateParameters.TargetTags=TargetTags;

	float Vigor=0.f;
	GetCapturedAttributeMagnitude(VigorDef,Spec,EvaluateParameters,Vigor);
	Vigor=FMath::Max<float>(Vigor,0.f);//将Vigor的影响控制在最小为0

	//获取等级
	ICombatInterface* CombatInterface=Cast<ICombatInterface>(Spec.GetContext().GetSourceObject());
	const int32 Level=CombatInterface->GetPlayerLevel();

	return 80.f+Vigor*2.5f+Level*10.f;
}
