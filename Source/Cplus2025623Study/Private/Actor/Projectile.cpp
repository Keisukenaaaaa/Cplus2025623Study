// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Projectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "Cplus2025623Study/Cplus2025623Study.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;//服务器负责计算并更新Actor的状态，然后通过网络将这些更新复制到所有连接的客户端上

	//初始化碰撞体
	Sphere=CreateDefaultSubobject<USphereComponent>("Sphere");
	SetRootComponent(Sphere);
	Sphere->SetCollisionObjectType(ECC_PROJECTILE);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly); //设置其只用作查询使用
	Sphere->SetCollisionResponseToChannels(ECR_Ignore); //设置其忽略所有碰撞检测
	Sphere->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap); //设置其与世界动态物体产生重叠事件
	Sphere->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap); //设置其与世界静态物体产生重叠事件
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); //设置其与Pawn类型物体产生重叠事件

	//创建发射组件
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
	ProjectileMovement->InitialSpeed = 550.f; //设置初始速度
	ProjectileMovement->MaxSpeed = 550.f; //设置最大速度
	ProjectileMovement->ProjectileGravityScale = 0.f; //设置重力影响因子，0为不受影响


}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	//设置此物品的生存时间
	SetLifeSpan(LifeSpan);
	
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnSphereOverlap);

	//添加一个音效，并附加到根组件上面，在技能移动时，声音也会跟随移动
	LoopingSoundComponent = UGameplayStatics::SpawnSoundAttached(LoopingSound, GetRootComponent());
	
}

void AProjectile::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (GetInstigator()==OtherActor) return;
	PlayImpact();
	
	if(HasAuthority())
	{
		//为目标应用GE
		if(UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			TargetASC->ApplyGameplayEffectSpecToSelf(*DamageEffectHandle.Data.Get());
		}
		

		Destroy();
	}
	else
	{
		//如果对actor没有权威性，将bHit设置为true，证明当前已经播放了击中特效  什么是权威性
		bHit = true;
	}
}

void AProjectile::Destroyed()
{
	//如果没有权威性，并且bHit没有修改为true，证明当前没有触发Overlap事件，在销毁前播放击中特效
	if(!bHit && !HasAuthority())
	{
		//为什么此处就是 销毁前了
		PlayImpact();
	}
	Super::Destroyed();//这里的摧毁的逻辑在哪?
}

void AProjectile::PlayImpact() const
{
	//播放音效
	UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation(), FRotator::ZeroRotator);
	//播放粒子特效
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation());

	//将音乐停止后会自动销毁
	if(LoopingSoundComponent) LoopingSoundComponent->Stop();
	
}




