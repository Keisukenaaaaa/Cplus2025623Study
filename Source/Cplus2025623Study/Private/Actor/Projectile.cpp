#include "Actor/Projectile.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "Cplus2025623Study/Cplus2025623Study.h"

AProjectile::AProjectile()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;

    Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
    SetRootComponent(Sphere);
    Sphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    Sphere->SetCollisionObjectType(ECC_PROJECTILE);
    Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
    Sphere->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
    Sphere->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
    Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

    Sphere->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnSphereOverlap);

    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
    ProjectileMovement->InitialSpeed = 550.f;
    ProjectileMovement->MaxSpeed = 550.f;
    ProjectileMovement->ProjectileGravityScale = 0.f;
    ProjectileMovement->bRotationFollowsVelocity = true;

    SetReplicateMovement(true);
}

void AProjectile::BeginPlay()
{
    Super::BeginPlay();

    SetLifeSpan(LifeSpan);

    AActor* OwnerActor = GetOwner();
    if (OwnerActor)
    {
        Sphere->IgnoreActorWhenMoving(OwnerActor, true);
    }
    APawn* InstigatorPawn = GetInstigator();
    if (InstigatorPawn)
    {
        Sphere->IgnoreActorWhenMoving(InstigatorPawn, true);
    }

    LoopingSoundComponent = UGameplayStatics::SpawnSoundAttached(LoopingSound, GetRootComponent());
}

void AProjectile::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor == nullptr || OtherActor == GetOwner() || OtherActor == GetInstigator())
    {
        return;
    }

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
        //如果对actor没有权威性，将bHit设置为true，证明当前已经播放了击中特效  什么是权威?
        bHit = true;
    }
}

void AProjectile::Destroyed()
{
    //如果没有权威性，并且bHit没有修改为true，证明当前没有触发Overlap事件，在销毁前播放击中特效
    if(!bHit && !HasAuthority())
    {
        //为什么此处就在销毁前?
        PlayImpact();
    }
    Super::Destroyed();
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
