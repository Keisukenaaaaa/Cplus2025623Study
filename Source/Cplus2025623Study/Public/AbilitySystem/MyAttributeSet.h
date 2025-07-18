// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "MyAttributeSet.generated.h"

	#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

USTRUCT()
struct FEffectProperties
{
	GENERATED_BODY()

	FEffectProperties(){}

	FGameplayEffectContextHandle EffectContextHandle;

	UPROPERTY()
	UAbilitySystemComponent* SourceASC=nullptr;

	UPROPERTY()
	AActor* SourceAvatarActor=nullptr;
	
	UPROPERTY()
	AController* SourceController=nullptr;

	UPROPERTY()
	ACharacter* SourceCharacter=nullptr;

	UPROPERTY()
	UAbilitySystemComponent* TargetASC=nullptr;

	UPROPERTY()
	AActor* TargetAvatarActor=nullptr;

	UPROPERTY()
	AController* TargetController=nullptr;

	UPROPERTY()
	ACharacter* TargetCharacter=nullptr;
};




UCLASS()
class CPLUS2025623STUDY_API UMyAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	UMyAttributeSet();
	//为什么可以直接以脚本名为方法建立函数 
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;//啥意思
	
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	
	
	

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Vital Attributes")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UMyAttributeSet,Health);


	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Vital Attributes")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UMyAttributeSet,MaxHealth);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Mana, Category = "Vital Attributes")
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(UMyAttributeSet,Mana);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxMana, Category = "Vital Attributes")
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(UMyAttributeSet,MaxMana);
	
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth) const; //此处的两个const分别代表什么

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const; //此处的两个const分别代表什么
	UFUNCTION()
	void OnRep_Mana(const FGameplayAttributeData& OldMana) const; //此处的两个const分别代表什么

	UFUNCTION()
	void OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const; //此处的两个const分别代表什么
	
private:
	
	static void SetEffectProperties(const FGameplayEffectModCallbackData& Data,FEffectProperties& Props);

};
