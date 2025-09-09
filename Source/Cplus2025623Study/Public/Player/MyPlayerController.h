// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "AbilitySystem/MyAbilitySystemComponentBase.h"
#include "GameFramework/PlayerController.h"
#include "MyPlayerController.generated.h"

class USplineComponent;
class UInputConfig;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class IEnemyInterface;
class UMyAbilitySystemComponentBase;
/**
 * 
 */
UCLASS()
class CPLUS2025623STUDY_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()
	

public:
	AMyPlayerController();
	virtual void PlayerTick(float DeltaTime) override;
	
protected:
	virtual void BeginPlay() override;//游戏开始时触发
	virtual void SetupInputComponent() override;//再生成输入组件时触发

private:
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext>AuraContext;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction>MoveAction;
	
	void Move(const FInputActionValue& InputActionValue);
	
	void CursorTrace();
	IEnemyInterface* LastActor;
	IEnemyInterface* ThisActor;
	FHitResult CursorHit;

	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHold(FGameplayTag InputTag);

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputConfig> InputConfig;
	
	UPROPERTY()
	TObjectPtr<UMyAbilitySystemComponentBase> MyAbilitySystemComponentBase;

	UMyAbilitySystemComponentBase* GetASC();

	FVector CachedDestination = FVector::ZeroVector;//储存鼠标点击的位置
	float FollowTime = 0.f;//用于查看按住了鼠标多久
	bool bAutoRunning=false;//当前是否为自动移动(自动寻路)
	bool bTargeting = false;//当前鼠标是否选中敌人

	UPROPERTY(EditDefaultsOnly)
	float ShortPressThreshold=0.3f;//-----定义鼠标悬停多长时间算点击时间------

	UPROPERTY(EditDefaultsOnly)
	float AutoRunAcceptanceRadius=50.f;//当角色和目标距离在此半径内时,寻路停止

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USplineComponent> Spline;//自动寻路所需线条

	
	void AutoRun();
};
