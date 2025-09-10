// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/ProjectileSpell.h"

#include "Actor/Projectile.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/KismetSystemLibrary.h"

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
	// 	//TODO:给 Projectile添加一个GE 去实现伤害
	//
	// 	//确保变换设置被正确应用
	// 	Projectile->FinishSpawning(SpawnTransform);
	// }

}

void UProjectileSpell::SpawnProjectile()
{
	const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority(); //判断此函数是否在服务器运行
	if (!bIsServer) return;

	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo()))
	{
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(CombatInterface->GetCombatSocketLocation());
		SpawnTransform.SetRotation(GetAvatarActorFromActorInfo()->GetActorQuat());
		
		//SpawnActorDeferred将异步创建实例，在实例创建完成时，相应的数据已经应用到了实例身上
		AProjectile* Projectile = GetWorld()->SpawnActorDeferred<AProjectile>(
			ProjectileClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			Cast<APawn>(GetOwningActorFromActorInfo()),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		//TODO:给 Projectile添加一个GE 去实现伤害

		//确保变换设置被正确应用
		Projectile->FinishSpawning(SpawnTransform);
	}

}
