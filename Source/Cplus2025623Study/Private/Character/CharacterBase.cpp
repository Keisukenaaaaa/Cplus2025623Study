// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CharacterBase.h"

// Sets default values
ACharacterBase::ACharacterBase()
{

	PrimaryActorTick.bCanEverTick = false;

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");//创建控件
	Weapon->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));//添加到父节点
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);//关闭骨骼网格体碰撞
}

void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
