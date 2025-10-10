// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AbilitySystemInterface.h"

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameFramework/Character.h"
#include "Interaction/CombatInterface.h"
#include "CharacterBase.generated.h"

class UAbilitySystemComponent;
class UAttributeSet;
class UGameplayEffect;

UCLASS(Abstract)
class CPLUS2025623STUDY_API ACharacterBase : public ACharacter, public IAbilitySystemInterface,public ICombatInterface
{
	GENERATED_BODY()

public:
	ACharacterBase();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() const { return AttributeSet; }

	

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Combat")
	FName WeaponTipSocketName;

	virtual  FVector GetCombatSocketLocation() override;

	UPROPERTY(EditAnywhere,Category="Combat")
	TObjectPtr<USkeletalMeshComponent>Weapon;

	UPROPERTY();
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY();
	TObjectPtr<UAttributeSet> AttributeSet;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Attributes")
	TSubclassOf<UGameplayEffect> DefaultPrimaryAttributes;

	//const写在哪里都可以吗 A:不行
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Attributes")
	TSubclassOf<UGameplayEffect> DefaultSecondaryAttributes;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Attributes")
	TSubclassOf<UGameplayEffect> DefaultVitalAttributes;
	
	void ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass,float Level) const;

	void InitializeDefaultAttributes() const;

	
	
	virtual void InitAbilityActorInfo();

	void AddCharacterAbilities();

	//重写战斗接口中的 受击函数 ;疑问这整篇代码中哪里将战斗接口赋予到角色积累的,忘记了;是上面那个public声明就可以做到了吗
	virtual UAnimMontage* GetHitReactMontage_Implementation() override;

	UPROPERTY(EditAnywhere, Category="Combat")
	TObjectPtr<UAnimMontage>HitReactMontage; 
	

private:
	UPROPERTY(EditAnywhere,Category="Attributes")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;
	

};
