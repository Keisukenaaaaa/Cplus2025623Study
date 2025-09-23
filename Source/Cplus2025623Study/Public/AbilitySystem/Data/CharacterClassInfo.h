// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayEffect.h"
#include "CharacterClassInfo.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class ECharacterClass:uint8
{
	Elementalist,//法师
	Warrior,//战士
	Ranger//游侠
};

USTRUCT()
struct FCharacterClassDefaultInfo
{
	GENERATED_BODY()//为什么结构体重具有该代码

	UPROPERTY(EditDefaultsOnly,Category="Class Defaults")
	TSubclassOf<UGameplayEffect> PrimaryAttributes;
};
UCLASS()
class CPLUS2025623STUDY_API UCharacterClassInfo : public UDataAsset
{
	GENERATED_BODY()

	
	UPROPERTY(EditDefaultsOnly, Category="Class Defaults")
	TMap<ECharacterClass, FCharacterClassDefaultInfo> CharacterClassInformation;
	
	UPROPERTY(EditDefaultsOnly, Category="Common Class Defaults")
	TSubclassOf<UGameplayEffect> SecondaryAttributes;
	
	UPROPERTY(EditDefaultsOnly, Category="Common Class Defaults")
	TSubclassOf<UGameplayEffect> VitalAttributes;

	//通过枚举获取对应的初始化类
	FCharacterClassDefaultInfo GetClassDefaultInfo(ECharacterClass CharacterClass);

};
