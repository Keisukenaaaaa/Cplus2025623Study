// Fill out your copyright notice in the Description page of Project Settings.
//新增的火球朝向问题

#include "AbilitySystem/Abilities/ProjectileSpell.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Actor/Projectile.h"
#include "Interaction/CombatInterface.h"
#include "GameFramework/PlayerController.h"
#include "Components/SkeletalMeshComponent.h"

void UProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                       const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                       const FGameplayEventData* TriggerEventData)
{
	AActor* Avatar = GetAvatarActorFromActorInfo();
	UE_LOG(LogTemp, Warning, TEXT("ProjectileSpell::ActivateAbility Avatar=%s HasAuthority=%d Role=%d"),
		*GetNameSafe(Avatar),
		HasAuthority(&CurrentActivationInfo),
		Avatar ? static_cast<int32>(Avatar->GetLocalRole()) : -1);

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	FVector TargetLocation = FVector::ZeroVector;
	if (TriggerEventData && TriggerEventData->TargetData.Num() > 0)
	{
		TargetLocation = UAbilitySystemBlueprintLibrary::GetTargetDataEndPoint(TriggerEventData->TargetData, 0);
	}
	else
	{
		if (const APlayerController* PC = Cast<APlayerController>(ActorInfo ? ActorInfo->PlayerController.Get() : nullptr))
		{
			FHitResult Hit;
			PC->GetHitResultUnderCursor(ECC_Visibility, false, Hit);
			if (Hit.bBlockingHit)
			{
				TargetLocation = Hit.ImpactPoint;
			}
		}
	}

	if (TargetLocation.IsNearlyZero() && Avatar)
	{
		TargetLocation = Avatar->GetActorLocation() + Avatar->GetActorForwardVector() * 800.f;
	}

}

void UProjectileSpell::SpawnProjectile(const FVector& ProjectileTargetLocation)
{
	if (!GetAvatarActorFromActorInfo()->HasAuthority()) return;

	UE_LOG(LogTemp, Warning, TEXT("SpawnProjectile on %s HasAuthority=%d Role=%d"),
		*GetNameSafe(GetAvatarActorFromActorInfo()),
		HasAuthority(&CurrentActivationInfo),
		GetAvatarActorFromActorInfo() ? static_cast<int32>(GetAvatarActorFromActorInfo()->GetLocalRole()) : -1);

	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo()))
	{
		const FVector SocketLocation = CombatInterface->GetCombatSocketLocation();
		FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();
		Rotation.Pitch = 0.f;

		FTransform SpawnTransform;
		SpawnTransform.SetLocation(SocketLocation);
		SpawnTransform.SetRotation(Rotation.Quaternion());

		AProjectile* Projectile = GetWorld()->SpawnActorDeferred<AProjectile>(
			ProjectileClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			Cast<APawn>(GetOwningActorFromActorInfo()),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		const UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo());
		FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
		EffectContextHandle.SetAbility(this);
		EffectContextHandle.AddSourceObject(Projectile);

		TArray<TWeakObjectPtr<AActor>> Actors;
		Actors.Add(Projectile);
		EffectContextHandle.AddActors(Actors);

		FHitResult HitResult;
		HitResult.Location = ProjectileTargetLocation;
		EffectContextHandle.AddHitResult(HitResult);
		EffectContextHandle.AddOrigin(ProjectileTargetLocation);

		const FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), SourceASC->MakeEffectContext());
		for(auto& Pair : DamageTypes)
		{
			const float ScaledDamage = Pair.Value.GetValueAtLevel(GetAbilityLevel());
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("火球术伤害：%f"), ScaledDamage));
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Pair.Key, ScaledDamage);
		}

		Projectile->DamageEffectHandle = SpecHandle;
		Projectile->FinishSpawning(SpawnTransform);
	}
}
