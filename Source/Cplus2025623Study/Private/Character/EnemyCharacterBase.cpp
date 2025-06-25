// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/EnemyCharacterBase.h"

void AEnemyCharacterBase::HighlightActor()
{
	bHighlighted = true;
}

void AEnemyCharacterBase::UnHighlightActor()
{
	bHighlighted = false;
}
