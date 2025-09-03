// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/MyPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "EnhancedInputSubsystems.h"
#include "AbilitySystem/MyAbilitySystemBlueprintLibrary.h"
#include "Input/InputComponentBase.h"
#include "Interaction/EnemyInterface.h"


void AMyPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	CursorTrace();
}
void AMyPlayerController::CursorTrace()
{
	FHitResult CursorHit;
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
	if (!CursorHit.bBlockingHit)return;

	LastActor = ThisActor;
	ThisActor = Cast<IEnemyInterface>(CursorHit.GetActor());

	if (LastActor == nullptr) 
	{
		if (ThisActor != nullptr)
		{
			ThisActor ->HighlightActor();
		}
		else {};
	}
	else
	{
		if (ThisActor == nullptr)
		{
			LastActor->UnHighlightActor();
		}
		else {
			if (LastActor != ThisActor)
			{
				LastActor->UnHighlightActor();
				ThisActor->HighlightActor();
			}
			else {}

		}
	}
}

// void AMyPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)//该整段代码是不是完全可以在蓝图中实现?包括按钮的响应三种方式?本质不就是将按钮和三个逻辑绑定?
// {
// 	GEngine->AddOnScreenDebugMessage(1, 3.f, FColor::Red, *InputTag.ToString());
// }
//
// void AMyPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
// {
// 	GEngine->AddOnScreenDebugMessage(2, 3.f, FColor::Blue, *InputTag.ToString());
// }
//
// void AMyPlayerController::AbilityInputTagHold(FGameplayTag InputTag)
// {
// 	GEngine->AddOnScreenDebugMessage(3, 3.f, FColor::Yellow, *InputTag.ToString());
// }
void AMyPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	// GEngine->AddOnScreenDebugMessage(1, 3.f, FColor::Red, *InputTag.ToString());
}

void AMyPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if(GetASC() == nullptr) return;
	GetASC()->AbilityInputTagReleased(InputTag);
}

void AMyPlayerController::AbilityInputTagHold(FGameplayTag InputTag)
{
	if(GetASC() == nullptr) return;
	GetASC()->AbilityInputTagHold(InputTag);
}


UMyAbilitySystemComponentBase* AMyPlayerController::GetASC()
{
	if(MyAbilitySystemComponentBase == nullptr)
	{
		MyAbilitySystemComponentBase = Cast<UMyAbilitySystemComponentBase>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn()));
	}

	return MyAbilitySystemComponentBase;
}

AMyPlayerController::AMyPlayerController()
{
	bReplicates = true;//是否将数据传送服务器更新
}

void AMyPlayerController::BeginPlay()
{

	Super::BeginPlay();

	check(AuraContext);

	//从本地角色身上获取到它的子系统
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (Subsystem)
	{
	
	Subsystem->AddMappingContext(AuraContext, 0);//可以存在多个操作映射，根据优先级触发
		}

	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	
	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);//将鼠标锁定在视口内
	InputModeData.SetHideCursorDuringCapture(false);//鼠标被捕获时是否隐藏
	SetInputMode(InputModeData);



}

void AMyPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UInputComponentBase* EnhancedInputComponent = CastChecked<UInputComponentBase>(InputComponent);//此处为什么从EnhancedInputComponent切换到UInputComponentBase?这里面不是没有移动的逻辑吗 基本没事了= =他继承自EnhancedInputComponent

	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMyPlayerController::Move);

	EnhancedInputComponent->BindAbilityAction(InputConfig,this,&ThisClass::AbilityInputTagPressed,&ThisClass::AbilityInputTagReleased,&ThisClass::AbilityInputTagHold);
}

void AMyPlayerController::Move(const FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if (APawn* ControlledPawn = GetPawn<APawn>()) 
	{
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
	}
}


