// Fill out your copyright notice in the Description page of Project Settings.
//新增的火球朝向问题
#include "AbilitySystem/Abilities/ProjectileSpell.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

#include "Actor/Projectile.h"
#include "Interaction/CombatInterface.h"


void UProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                       const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                       const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo()))
	// {
	// 	FTransform SpawnTransform;
	// 	SpawnTransform.SetLocation(CombatInterface->GetCombatSocketLocation());
	// 	SpawnTransform.SetRotation(GetAvatarActorFromActorInfo()->GetActorQuat());
	// 	
	// 	//SpawnActorDeferred将异步创建实例，在实例创建完成时，相应的数据已经应用到了实例身上
	// 	AProjectile* Projectile = GetWorld()->SpawnActorDeferred<AProjectile>(
	// 		ProjectileClass,
	// 		SpawnTransform,
	// 		GetOwningActorFromActorInfo(),
	// 		Cast<APawn>(GetOwningActorFromActorInfo()),
	// 		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	//
	// TODO:给 Projectile添加一个GE 去实现伤害
	//
	// 	//确保变换设置被正确应用
	// 	Projectile->FinishSpawning(SpawnTransform);
	// }

}

void UProjectileSpell::SpawnProjectile(const FVector& ProjectileTargetLocation)
{
	const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority(); //判断此函数是否在服务器运行
	if (!bIsServer) return;

	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo()))
	{
		const FVector SocketLocation = CombatInterface->GetCombatSocketLocation();
		FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation(); //将方向转为旋转
		//Rotation.Pitch = 0.f; //设置Pitch为0，转向的朝向将平行于地面

		
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(CombatInterface->GetCombatSocketLocation());
		SpawnTransform.SetRotation(Rotation.Quaternion());

	
		//SpawnActorDeferred将异步创建实例，在实例创建完成时，相应的数据已经应用到了实例身上
		AProjectile* Projectile = GetWorld()->SpawnActorDeferred<AProjectile>(
			ProjectileClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			Cast<APawn>(GetOwningActorFromActorInfo()),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		//TODO:给 Projectile添加一个GE 去实现伤害
		//创建一个GE的实例，并设置给投射物
		const UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo());
		//10.16开始学GAS底层了讲了一堆听不懂的,到时候问下GPT
		FGameplayEffectContextHandle EffectContextHandle=SourceASC->MakeEffectContext();
		EffectContextHandle.SetAbility(this);//设置技能
		EffectContextHandle.AddSourceObject(Projectile); //设置GE的源
	
		//添加Actor列表 这个列表中的Actor怎么加的? 不是先加明中结果吗?还是这个是全地图所有的敌人表
		TArray<TWeakObjectPtr<AActor>> Actors;
		Actors.Add(Projectile);
		EffectContextHandle.AddActors(Actors);
		//添加命中结果
		FHitResult HitResult;
		HitResult.Location = ProjectileTargetLocation;
		EffectContextHandle.AddHitResult(HitResult);
		//添加技能触发位置
		EffectContextHandle.AddOrigin(ProjectileTargetLocation);
		//10.16 第一段
		
		const FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), SourceASC->MakeEffectContext());
		//设置技能伤害 SetByCaller获取 通过Tag
		const FMyGameplayTags GameplayTags = FMyGameplayTags::Get(); //获取标签单例
		for(auto& Pair : DamageTypes)
		{
			const float ScaledDamage = Pair.Value.GetValueAtLevel(GetAbilityLevel()); //根据等级获取技能伤害
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("火球术伤害：%f"), ScaledDamage));
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Pair.Key, ScaledDamage);
		}

	
		// const float ScaledDamage = Damage.GetValueAtLevel(GetAbilityLevel()); //根据等级获取技能伤害
		//const float ScaledDamage = Damage.GetValueAtLevel(GetAbilityLevel() + 19);
		
		
		//UAbilitySystemBlueprintLibrary::AssignSetByCallerMagnitude() //使用DataName设置 两种方式都行
		//UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Damage, ScaledDamage);
	
		Projectile->DamageEffectHandle = SpecHandle;

		//确保变换设置被正确应用
		Projectile->FinishSpawning(SpawnTransform);
	}

}


