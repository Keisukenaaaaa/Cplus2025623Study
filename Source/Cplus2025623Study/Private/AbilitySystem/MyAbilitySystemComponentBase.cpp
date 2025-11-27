// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/MyAbilitySystemComponentBase.h"

#include "AbilitySystem/Abilities/MyGameplayAbility.h"
#include "MyGameplayTags.h"

void UMyAbilitySystemComponentBase::AbilityActorInfoSet()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UMyAbilitySystemComponentBase::ClientEffectApplied);
}

void UMyAbilitySystemComponentBase::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities)
{
	for (TSubclassOf<UGameplayAbility> AbilityClass : StartupAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		if (const UMyGameplayAbility* AbilityBase = Cast<UMyGameplayAbility>(AbilitySpec.Ability))
		{
			AbilitySpec.DynamicAbilityTags.AddTag(AbilityBase->StartupInputTag);
			GiveAbility(AbilitySpec);
		}
	}
}

void UMyAbilitySystemComponentBase::AbilityInputTagHold(const FGameplayTag& InputTag)
{
	UE_LOG(LogTemp, Warning, TEXT("ASC [%s] Hold InputTag=%s Role=%d IsLocal=%d"),
		*GetNameSafe(GetOwner()),
		*InputTag.ToString(),
		GetOwner() ? static_cast<int32>(GetOwner()->GetLocalRole()) : -1,
		(GetOwner() && GetOwner()->GetLocalRole() == ROLE_AutonomousProxy) ? 1 : 0);

	if (!InputTag.IsValid()) return;
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			AbilitySpecInputPressed(AbilitySpec);
			if (!AbilitySpec.IsActive())
			{
				TryActivateAbility(AbilitySpec.Handle);
			}
			else
			{
				InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, AbilitySpec.Handle, AbilitySpec.ActivationInfo.GetActivationPredictionKey());
			}
		}
	}
}

void UMyAbilitySystemComponentBase::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	UE_LOG(LogTemp, Warning, TEXT("ASC [%s] Pressed InputTag=%s Role=%d IsLocal=%d"),
		*GetNameSafe(GetOwner()),
		*InputTag.ToString(),
		GetOwner() ? static_cast<int32>(GetOwner()->GetLocalRole()) : -1,
		(GetOwner() && GetOwner()->GetLocalRole() == ROLE_AutonomousProxy) ? 1 : 0);

	if (!InputTag.IsValid()) return;
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			AbilitySpecInputPressed(AbilitySpec);
			if (AbilitySpec.IsActive())
			{
				InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, AbilitySpec.Handle, AbilitySpec.ActivationInfo.GetActivationPredictionKey());
			}
		}
	}
}

void UMyAbilitySystemComponentBase::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	UE_LOG(LogTemp, Warning, TEXT("ASC [%s] Released InputTag=%s Role=%d IsLocal=%d"),
		*GetNameSafe(GetOwner()),
		*InputTag.ToString(),
		GetOwner() ? static_cast<int32>(GetOwner()->GetLocalRole()) : -1,
		(GetOwner() && GetOwner()->GetLocalRole() == ROLE_AutonomousProxy) ? 1 : 0);

	if (!InputTag.IsValid()) return;
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			AbilitySpecInputReleased(AbilitySpec);
			if (AbilitySpec.IsActive())
			{
				InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, AbilitySpec.Handle, AbilitySpec.ActivationInfo.GetActivationPredictionKey());
			}
		}
	}
}

void UMyAbilitySystemComponentBase::ClientEffectApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent,
	const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveGameplayEffectHandle) const
{
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer);
	EffectAssetTags.Broadcast(TagContainer);
}

void UMyAbilitySystemComponentBase::EffectApplied(UAbilitySystemComponent* AbilitySystemComponent,
	const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveGameplayEffectHandle)
{
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer);
	EffectAssetTags.Broadcast(TagContainer);
}
