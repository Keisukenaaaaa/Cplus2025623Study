// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/EnemyCharacterBase.h"
#include <Cplus2025623Study/Cplus2025623Study.h>

#include "MyGameplayTags.h"
#include "AbilitySystem/MyAbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/MyAbilitySystemComponentBase.h"
#include "AbilitySystem/MyAttributeSet.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UI/Widget/MyUserWidget.h"


AEnemyCharacterBase::AEnemyCharacterBase()
{
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	
	AbilitySystemComponent = CreateDefaultSubobject<UMyAbilitySystemComponentBase>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true); //设置组件用于在网络上复制
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UMyAttributeSet>("AttributeSet");

	HealthBar = CreateDefaultSubobject<UWidgetComponent>("HealthBar");
	HealthBar->SetupAttachment(GetRootComponent()); //将血条附件到根节点上
}

void AEnemyCharacterBase::HighlightActor()
{
	
	GetMesh()->SetRenderCustomDepth(true);
	GetMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
	Weapon->SetRenderCustomDepth(true);
	Weapon->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);

}

void AEnemyCharacterBase::UnHighlightActor()
{
	GetMesh()->SetRenderCustomDepth(false);
	Weapon->SetRenderCustomDepth(false);
}

int32 AEnemyCharacterBase::GetPlayerLevel()
{
	return Level;
}

void AEnemyCharacterBase::HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount)//如果数量大于0,择期移动速度为0
{
	bHitReacting = NewCount > 0;
	GetCharacterMovement()->MaxWalkSpeed = bHitReacting ? 0.f : BaseWalkSpeed;
}

void AEnemyCharacterBase::Die()
{
	SetLifeSpan(LifeSpan);
	Super::Die();//为什么此处需要super?是继承父级的原本的Die吗?
}

/////带研读线
void AEnemyCharacterBase::BeginPlay()//这段代码在9.17更新后需要重点研读
{
	Super::BeginPlay();

	//设置角色的初始移动速度
	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;

	//初始化角色ASC
	InitAbilityActorInfo();

	//初始化角色的技能--添加HasAuthority来让他复制到客户端
	if (HasAuthority())
	{
		UMyAbilitySystemBlueprintLibrary::GiveStartupAbilities(this, AbilitySystemComponent);
	}
	
	if(UMyUserWidget* UserWidget = Cast<UMyUserWidget>(HealthBar->GetUserWidgetObject()))
	{
		UserWidget->SetWidgetController(this);
	}
	if(const UMyAttributeSet* AS = Cast<UMyAttributeSet>(AttributeSet))
	{
		//监听血量变化
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AS->GetHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnHealthChanged.Broadcast(Data.NewValue);
			}
		);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AS->GetMaxHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxHealthChanged.Broadcast(Data.NewValue);
			}
		);

		//在AEnemyBase::BeginPlay()中，我们设置对监听函数的回调
		AbilitySystemComponent->RegisterGameplayTagEvent(FMyGameplayTags::Get().Effects_HitReact,EGameplayTagEventType::NewOrRemoved).AddUObject(
			this,
			&ThisClass::HitReactTagChanged);
		//初始化血量
		OnHealthChanged.Broadcast(AS->GetHealth());
		OnMaxHealthChanged.Broadcast(AS->GetMaxHealth());
	}

	
}

void AEnemyCharacterBase::InitAbilityActorInfo()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	Cast<UMyAbilitySystemComponentBase>(AbilitySystemComponent)->AbilityActorInfoSet();

	//通过GE初始角色的属性
	if (HasAuthority())
	{
		InitializeDefaultAttributes();
	}
	//打印生命值查看属性
	UE_LOG(LogTemp, Warning, TEXT("%s 的生命值为 %f"), *this->GetName(), Cast<UMyAttributeSet>(AttributeSet)->GetHealth())
}

void AEnemyCharacterBase::InitializeDefaultAttributes() const
{
	UMyAbilitySystemBlueprintLibrary::InitializeDefaultAttributes(this, CharacterClass, Level, AbilitySystemComponent);
}
