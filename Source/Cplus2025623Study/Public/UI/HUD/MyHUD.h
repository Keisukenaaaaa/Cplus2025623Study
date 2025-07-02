// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/HUD.h"
#include "UI/Widget/MyUserWidget.h"

#include "MyHUD.generated.h"


class UMyOverlayWidgetController;
class UMyUserWidget;//为什么此处要声明对应的class 为什么有时候声明class 有时候用include
struct FWidgetControllerParams;
/**
 * 
 */
UCLASS()
class CPLUS2025623STUDY_API AMyHUD : public AHUD
{
	GENERATED_BODY()
public:

	UPROPERTY()
	TObjectPtr<UMyUserWidget> OverlayWidget;

	UMyOverlayWidgetController* GetOverlayWidgetController(const FWidgetControllerParams& WCParams);

	void InitOverlay(APlayerController*PC,APlayerState*PS,UAbilitySystemComponent*ASC,UAttributeSet*AS);
	
protected:

	
private:
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UMyUserWidget>	OverlayWidgetClass;


	UPROPERTY()
	TObjectPtr<UMyOverlayWidgetController> OverlayWidgetController;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UMyOverlayWidgetController>	OverlayWidgetControllerClass;
};
