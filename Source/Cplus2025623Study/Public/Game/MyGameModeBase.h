// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "GameFramework/GameModeBase.h"
#include "MyGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class CPLUS2025623STUDY_API AMyGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category="Character Class Defaults")
	TObjectPtr<UCharacterClassInfo> CharacterClassInfo;

};
