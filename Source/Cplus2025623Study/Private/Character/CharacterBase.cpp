// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CharacterBase.h"

#include "AbilitySystem/MyAbilitySystemComponentBase.h"
#include "AbilitySystem/MyAttributeSet.h"
#include "Components/CapsuleComponent.h"
#include "Cplus2025623Study/Cplus2025623Study.h"

// Sets default values
ACharacterBase::ACharacterBase()
{

	PrimaryActorTick.bCanEverTick = false;

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");//创建控件
	Weapon->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));//添加到父节点
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);//关闭骨骼网格体碰撞

	//设置角色不会和相机碰撞
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_PROJECTILE, ECR_Overlap); //设置碰撞体可以和发射物产生重叠
	GetCapsuleComponent()->SetGenerateOverlapEvents(true); //设置碰撞体可以生成重叠事件
	
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

}

UAbilitySystemComponent* ACharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

FVector ACharacterBase::GetCombatSocketLocation()
{
	check(Weapon);
	return Weapon->GetSocketLocation(WeaponTipSocketName);
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
	
	if (!HasAuthority()) return;//查询是否拥有网络权限,应用技能需要添加给服务器
	ASC->AddCharacterAbilities(StartupAbilities);
}

UAnimMontage* ACharacterBase::GetHitReactMontage_Implementation()
{
	return HitReactMontage;
}

void ACharacterBase::Die()
{	//将武器从角色上分离
	Weapon->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld,true));
	MulticastHandleDeath();
}

void ACharacterBase::Dissolve()
{
	TArray<UMaterialInstanceDynamic*> MatArray;
	//设置角色溶解
	if(IsValid(DissolveMaterialInstance))
	{
		UMaterialInstanceDynamic* DynamicMatInst = UMaterialInstanceDynamic::Create(DissolveMaterialInstance, this);
		GetMesh()->SetMaterial(0, DynamicMatInst);
		MatArray.Add(DynamicMatInst);
	}

	//设置武器溶解
	if(IsValid(WeaponDissolveMaterialInstance))
	{
		UMaterialInstanceDynamic* DynamicMatInst = UMaterialInstanceDynamic::Create(WeaponDissolveMaterialInstance, this);
		Weapon->SetMaterial(0, DynamicMatInst);
		MatArray.Add(DynamicMatInst);
	}

	//调用时间轴渐变溶解
	StartDissolveTimeline(MatArray);

}

void ACharacterBase::MulticastHandleDeath_Implementation()
{
	//开启武器物理效果
	Weapon->SetSimulatePhysics(true); //开启模拟物理效果
	Weapon->SetEnableGravity(true); //开启重力效果
	Weapon->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly); //开启物理碰撞通道

	//开启角色物理效果
	GetMesh()->SetSimulatePhysics(true); //开启模拟物理效果
	GetMesh()->SetEnableGravity(true); //开启重力效果
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly); //开启物理碰撞通道
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block); //开启角色与静态物体产生碰撞

	//关闭角色碰撞体碰撞通道，避免其对武器和角色模拟物理效果产生影响 此处的碰撞体应该是碰撞检测所需的
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//设置角色溶解
	Dissolve();
	
}

// Called every frame
