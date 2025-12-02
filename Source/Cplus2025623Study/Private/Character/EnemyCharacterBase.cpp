

#include "Character/EnemyCharacterBase.h"
#include <Cplus2025623Study/Cplus2025623Study.h>

#include "MyGameplayTags.h"
#include "AbilitySystem/MyAbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/MyAbilitySystemComponentBase.h"
#include "AbilitySystem/MyAttributeSet.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/RPGAIController.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UI/Widget/MyUserWidget.h"


AEnemyCharacterBase::AEnemyCharacterBase()
{
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	
	AbilitySystemComponent = CreateDefaultSubobject<UMyAbilitySystemComponentBase>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true); //????????????????????????????????????
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UMyAttributeSet>("AttributeSet");

	HealthBar = CreateDefaultSubobject<UWidgetComponent>("HealthBar");
	HealthBar->SetupAttachment(GetRootComponent()); //??????????????????????????????

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = ARPGAIController::StaticClass();

// Enemy uses movement direction to orient
    bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->bUseControllerDesiredRotation = true;
	}
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

void AEnemyCharacterBase::HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount)//??????????????????0,?????????????????????
{
	bHitReacting = NewCount > 0;
	GetCharacterMovement()->MaxWalkSpeed = bHitReacting ? 0.f : BaseWalkSpeed;
	if (RPGAIController)
	{
		if (UBlackboardComponent* BlackboardComp = RPGAIController->GetBlackboardComponent())
		{
			BlackboardComp->SetValueAsBool("HitReacting", bHitReacting);
		}
	}
}

void AEnemyCharacterBase::Die()
{
	SetLifeSpan(LifeSpan);
	Super::Die();//?????????????????????super????????????????????????????Die???
}

/////????????????
void AEnemyCharacterBase::BeginPlay()//???????????????.17???????????????????????????
{
	Super::BeginPlay();

	//?????????????????????????????????
	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;

	//???????????????ASC
	InitAbilityActorInfo();

	//????????????????????????
	UMyAbilitySystemBlueprintLibrary::GiveStartupAbilities(this, AbilitySystemComponent);
	
	if(UMyUserWidget* UserWidget = Cast<UMyUserWidget>(HealthBar->GetUserWidgetObject()))
	{
		UserWidget->SetWidgetController(this);
	}
	if(const UMyAttributeSet* AS = Cast<UMyAttributeSet>(AttributeSet))
	{
		//??????????????????
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

		//???AEnemyBase::BeginPlay()??????????????????????????????????????????
		AbilitySystemComponent->RegisterGameplayTagEvent(FMyGameplayTags::Get().Effects_HitReact,EGameplayTagEventType::NewOrRemoved).AddUObject(
			this,
			&ThisClass::HitReactTagChanged);
		//???????????????
		OnHealthChanged.Broadcast(AS->GetHealth());
		OnMaxHealthChanged.Broadcast(AS->GetMaxHealth());
	}

	
}

void AEnemyCharacterBase::InitAbilityActorInfo()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	Cast<UMyAbilitySystemComponentBase>(AbilitySystemComponent)->AbilityActorInfoSet();

	//??????GE?????????????????????
	InitializeDefaultAttributes();

	//???????????????????????????
	UE_LOG(LogTemp, Warning, TEXT("%s ??????????????? %f"), *this->GetName(), Cast<UMyAttributeSet>(AttributeSet)->GetHealth())
}

void AEnemyCharacterBase::InitializeDefaultAttributes() const
{
	UMyAbilitySystemBlueprintLibrary::InitializeDefaultAttributes(this, CharacterClass, Level, AbilitySystemComponent);
}

void AEnemyCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// 只在服务器执行 AI 初始化
	if (!HasAuthority())
	{
		return;
	}

	RPGAIController = Cast<ARPGAIController>(NewController);
	if (!RPGAIController)
	{
		return;
	}

	// 行为树必须存在
	if (!BehaviorTree)
	{
		return;
	}

	// 黑板资源必须存在
	if (!BehaviorTree->BlackboardAsset)
	{
		return;
	}

	// 初始化黑板（由 UseBlackboard 自动创建 BlackboardComponent）
	UBlackboardComponent* BlackboardComp = nullptr;
	RPGAIController->UseBlackboard(BehaviorTree->BlackboardAsset, BlackboardComp);

	// 行为树必须成功启动
	if (!RPGAIController->RunBehaviorTree(BehaviorTree))
	{
		return;
	}

	// 根据教程，此处只做最基本的黑板初始化
	if (BlackboardComp)
	{
		BlackboardComp->SetValueAsBool("RangedAttacker", bRangedAttacker);
		BlackboardComp->SetValueAsBool("HitReacting", bHitReacting);
	}
}





