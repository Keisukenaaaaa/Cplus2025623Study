// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CharacterBase.h"

#include "AbilitySystem/MyAbilitySystemComponentBase.h"
#include "AbilitySystem/MyAttributeSet.h"

// Sets default values
ACharacterBase::ACharacterBase()
{

	PrimaryActorTick.bCanEverTick = false;

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");//创建控件
	Weapon->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));//添加到父节点
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);//关闭骨骼网格体碰撞
}

UAbilitySystemComponent* ACharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACharacterBase::ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level) const
{
	check(IsValid(GetAbilitySystemComponent()));
	check(GameplayEffectClass);
	
	FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	ContextHandle.AddSourceObject(this);   //设置源对象,可以通过Spec.GetContext().GetSourceObject()去获取源对象;
	
	const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(GameplayEffectClass, Level, ContextHandle);
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), GetAbilitySystemComponent());
}

void ACharacterBase::InitializeDefaultAttributes() const
{
	ApplyEffectToSelf(DefaultPrimaryAttributes,1.f);
	ApplyEffectToSelf(DefaultSecondaryAttributes,1.f);
	ApplyEffectToSelf(DefaultVitalAttributes,1.f);
	
	
}

// void  ACharacterBase::InitializePrimaryAttributes() const
// {
// 	// check(IsValid(GetAbilitySystemComponent()));
// 	// check(DefaultPrimaryAttributes);
// 	//
// 	// const FGameplayEffectContextHandle ContextHandle=GetAbilitySystemComponent()->MakeEffectContext();
// 	// const FGameplayEffectSpecHandle SpecHandle=GetAbilitySystemComponent()->MakeOutgoingSpec(DefaultPrimaryAttributes,1.0f,ContextHandle);
// 	// GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(),GetAbilitySystemComponent());
//
// 	check(IsValid(GetAbilitySystemComponent()));
// 	check(DefaultPrimaryAttributes);
// 	
// 	const FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
// 	const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(DefaultPrimaryAttributes, 1.0f, ContextHandle);
// 	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), GetAbilitySystemComponent());
//
// }

void ACharacterBase::InitAbilityActorInfo()
{
}

void ACharacterBase::AddCharacterAbilities()
{
	UMyAbilitySystemComponentBase *ASC=Cast<UMyAbilitySystemComponentBase>(GetAbilitySystemComponent());
	
	if (!HasAuthority()) return;
	ASC->AddCharacterAbilities(StartupAbilities);
}

// Called every frame
