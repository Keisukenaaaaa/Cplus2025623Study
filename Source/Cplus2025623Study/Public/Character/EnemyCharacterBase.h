// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/CharacterBase.h"
#include "Interaction/EnemyInterface.h"
#include "UI/WidgetController/MyOverlayWidgetController.h"
#include "EnemyCharacterBase.generated.h"

class UWidgetComponent;
/**
 * 
 */
UCLASS()
class CPLUS2025623STUDY_API AEnemyCharacterBase : public ACharacterBase, public IEnemyInterface
{
	GENERATED_BODY()

public:
	AEnemyCharacterBase();

	/** Enemy Inferface */
	virtual void HighlightActor() override;
	virtual void UnHighlightActor() override;
	/** end Enemy Inferface */

	/** Combat Inferface */
	virtual int32 GetPlayerLevel() override;
	/** end Combat Inferface */

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnMaxHealthChanged;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Widget", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UWidgetComponent> HealthBar;


	
protected:
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;

	

	
//private: 此处有报错 说无法让BlueprintReadonly在 private中使用	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character Class Defaults")
	int32 Level=1;
};
