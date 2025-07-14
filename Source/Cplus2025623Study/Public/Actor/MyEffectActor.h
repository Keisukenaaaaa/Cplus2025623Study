// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyEffectActor.generated.h"

class UGamplayEffect;

UCLASS()
class CPLUS2025623STUDY_API AMyEffectActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AMyEffectActor();
	
	

protected:

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable) 
	void ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass);//给与目标添加GameplayEffect

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Applied Effects")
	TSubclassOf<UGameplayEffect> InstantGameplayEffectClass;

	

	

	
public:	



};
