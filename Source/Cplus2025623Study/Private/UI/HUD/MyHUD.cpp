// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/MyHUD.h"

#include "Blueprint/UserWidget.h"
#include "UI/WidgetController/MyOverlayWidgetController.h"

UMyOverlayWidgetController* AMyHUD::GetOverlayWidgetController(const FWidgetControllerParams& WCParams)
{
	if (OverlayWidgetController==nullptr)
	{
		OverlayWidgetController=NewObject<UMyOverlayWidgetController>(this,OverlayWidgetControllerClass);
		OverlayWidgetController->SetWidgetControllerParams(WCParams);

		return OverlayWidgetController;
	}
	return OverlayWidgetController;
}

void AMyHUD::InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS)//这一整段什么意思
{

	checkf(OverlayWidgetClass,TEXT("Overlay Widget Class 没有弄好,去把BPHUD填进去"));
	checkf(OverlayWidgetControllerClass,TEXT("Overlay Widget Controller Class 没有弄好,去把BPHUD填进去"));
	
	
	UUserWidget* Widget = CreateWidget<UUserWidget>(GetWorld(),OverlayWidgetClass);
	OverlayWidget=Cast<UMyUserWidget>(Widget);

	const FWidgetControllerParams WidgetControllerParams(PC,PS,ASC,AS);
	UMyOverlayWidgetController* WidgetController=GetOverlayWidgetController(WidgetControllerParams);

	OverlayWidget->SetWidgetController(WidgetController);//什么是成员变量
	WidgetController->BroadcastInitialValues();

	
	Widget->AddToViewport();
}

