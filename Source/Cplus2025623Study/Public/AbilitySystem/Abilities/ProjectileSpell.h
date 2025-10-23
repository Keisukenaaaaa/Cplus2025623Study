// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyGameplayTags.h"
#include "RPGDamageGameplayAbility.h"
#include "AbilitySystem/Abilities/MyGameplayAbility.h"
//#include "Actor/Projectile.h"
#include "ProjectileSpell.generated.h"


class AProjectile;
/**
 * 
 */
UCLASS()
class CPLUS2025623STUDY_API UProjectileSpell : public URPGDamageGameplayAbility
{
	GENERATED_BODY()

protected:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TSubclassOf<AProjectile> ProjectileClass;

	

	UFUNCTION(BlueprintCallable,Category="Projectile")
	void SpawnProjectile(const FVector& ProjectileTargetLocation);


	
};
