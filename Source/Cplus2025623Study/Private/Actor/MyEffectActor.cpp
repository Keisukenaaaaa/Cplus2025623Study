// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/MyEffectActor.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/MyAttributeSet.h"
#include "Character/PlayerCharacter.h"

#include "Components/SphereComponent.h"
#include "Player/MyPlayerState.h"


// Sets default values
AMyEffectActor::AMyEffectActor()
{
 
	PrimaryActorTick.bCanEverTick = false;

	Mesh=CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	SetRootComponent(Mesh);
	
	Sphere=CreateDefaultSubobject<USphereComponent>("Sphere");
	Sphere->SetupAttachment(GetRootComponent());
}

void AMyEffectActor::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//暂时用一种hack方法,以后需要改变他
	// if (IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(OtherActor))
	// {
	// 	const UMyAttributeSet* PlayerAttributeSet =  Cast<UMyAttributeSet>(ASCInterface->GetAbilitySystemComponent()->GetAttributeSet(UMyAttributeSet::StaticClass()));
	// 	UMyAttributeSet* MuteableAuraAttributeSet=const_cast<UMyAttributeSet*>(PlayerAttributeSet);
	// 	MuteableAuraAttributeSet ->SetHealth(PlayerAttributeSet->GetHealth()+25.f);
	// 	Destroy();
	// }
	if(APlayerCharacter* TargetCharacter=Cast<APlayerCharacter>(OtherActor))
	{
		AMyPlayerState* TargetPlayerState=TargetCharacter->GetPlayerState<AMyPlayerState>();
		UMyAttributeSet* AttributeSet =Cast<UMyAttributeSet>(TargetPlayerState->GetAttributeSet());
		
		AttributeSet->SetHealth(AttributeSet->GetHealth()+25.f);
		//Destroy();
	}
	if (IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(OtherActor)) {}

}
// void AMyEffectActor::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
// 	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
// {
// 	//TODO: 为了测试数值修改功能，启用了常量转变量功能。
// 	if(IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(OtherActor))
// 	{
// 		//根据类从ASC里面获取到对应的AS实例
// 		const UMyAttributeSet* AttributeSet = Cast<UMyAttributeSet>(ASCInterface->GetAbilitySystemComponent()->GetAttributeSet(UMyAttributeSet::StaticClass()));
// 		UMyAttributeSet* MutableAttributeSet = const_cast<UMyAttributeSet*>(AttributeSet); //将常量转为变量
// 		MutableAttributeSet->SetHealth(AttributeSet->GetHealth() + 25.f);
// 		Destroy(); // 销毁自身
// 	}
// }


void AMyEffectActor::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

void AMyEffectActor::BeginPlay()
{
	Super::BeginPlay();

	Sphere->OnComponentBeginOverlap.AddDynamic(this,&AMyEffectActor::OnOverlap);
	Sphere->OnComponentEndOverlap.AddDynamic(this,&AMyEffectActor::EndOverlap);
	
}



