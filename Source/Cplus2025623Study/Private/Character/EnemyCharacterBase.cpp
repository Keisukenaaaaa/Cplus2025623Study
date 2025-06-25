// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/EnemyCharacterBase.h"
#include <Cplus2025623Study/Cplus2025623Study.h>


AEnemyCharacterBase::AEnemyCharacterBase()
{
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
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
