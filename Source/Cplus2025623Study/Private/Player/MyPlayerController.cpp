// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/MyPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "EnhancedInputSubsystems.h"
#include "MyGameplayTags.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "AbilitySystem/MyAbilitySystemBlueprintLibrary.h"
#include "Components/SplineComponent.h"
#include "Input/InputComponentBase.h"
#include "Interaction/EnemyInterface.h"


AMyPlayerController::AMyPlayerController()  	//此处是构造函数,为什么构造函数不用写在最前面?它是在对象创建时调用的?我调整过位置 问题换成为什么不放最前面也能正常运行
{
	bReplicates = true;//是否将数据传送服务器更新

	LastActor=nullptr;
	ThisActor=nullptr;
	
	Spline =CreateDefaultSubobject<USplineComponent>("Spline"); //在构造函数中将spline初始化
}

void AMyPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
//鼠标位置射线追踪
	CursorTrace();
//自动寻路
	AutoRun();
}

void AMyPlayerController::AutoRun()
{
	if(!bAutoRunning) return;
	if(APawn* ControlledPawn = GetPawn())
	{
		//找到距离样条最近的位置
		const FVector LocationOnSpline = Spline->FindLocationClosestToWorldLocation(ControlledPawn->GetActorLocation(), ESplineCoordinateSpace::World);
		//获取这个位置在样条上的方向
		const FVector Direction = Spline->FindDirectionClosestToWorldLocation(LocationOnSpline, ESplineCoordinateSpace::World);
		ControlledPawn->AddMovementInput(Direction);

		const float DistanceToDestination = (LocationOnSpline - CachedDestination).Length();
		if(DistanceToDestination <= AutoRunAcceptanceRadius)
		{
			bAutoRunning = false;
		}
	}

}
void AMyPlayerController::CursorTrace()//鼠标位置追踪代码
{

	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
	if (!CursorHit.bBlockingHit)return;//如果为命中返回结果

	LastActor = ThisActor;
	ThisActor = Cast<IEnemyInterface>(CursorHit.GetActor());

	if (ThisActor!=LastActor)
	{
		//这段怎么折叠起来的
		if (ThisActor)ThisActor->HighlightActor();
		if (LastActor)LastActor->UnHighlightActor();
	}
	//老的判定追踪代码
	// if (LastActor == nullptr) 
	// {
	// 	if (ThisActor != nullptr)
	// 	{
	// 		ThisActor ->HighlightActor();
	// 	}
	// 	else {};
	// }
	// else
	// {
	// 	if (ThisActor == nullptr)
	// 	{
	// 		LastActor->UnHighlightActor();
	// 	}
	// 	else {
	// 		if (LastActor != ThisActor)
	// 		{
	// 			LastActor->UnHighlightActor();
	// 			ThisActor->HighlightActor();
	// 		}
	// 		else {}
	//
	// 	}
	// }
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
	if (InputTag.MatchesTagExact(FMyGameplayTags::Get().InputTag_LMB))
	{
		bTargeting=ThisActor != nullptr;//ThisActor为鼠标悬停在敌人身上才会有值
		bAutoRunning = false;
		FollowTime=0.f;//重置统计时间
	}
}

void AMyPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if(!InputTag.MatchesTagExact(FMyGameplayTags::Get().InputTag_LMB))
	{
		if(GetASC())
		{
			GetASC()->AbilityInputTagHold(InputTag);
		}
		return;
	}
	
	if (bTargeting||bShiftKeyDown)//这一段判定是什么意思
	{
		if(GetASC())
		{
			GetASC()->AbilityInputTagHold(InputTag);
		}
	}
	
	else
	{
		const APawn* ControlledPawn=GetPawn();

		if (FollowTime<=ShortPressThreshold && ControlledPawn)
		{
			if (UNavigationPath* NavPath=UNavigationSystemV1::FindPathToLocationSynchronously(this,ControlledPawn->GetActorLocation(), CachedDestination))
			{
				Spline->ClearSplinePoints();//清除现有样条内的点;
				for (const FVector& PointLoc:NavPath->PathPoints)
				{
					Spline->AddSplinePoint(PointLoc,ESplineCoordinateSpace::World);//将新的位置添加到Spline中
					DrawDebugSphere(GetWorld(), PointLoc, 8.f, 8, FColor::Orange, false, 5.f); //点击后debug调试
				}
				//自动寻路到达目的后停止移动
				CachedDestination=NavPath->PathPoints[NavPath->PathPoints.Num()-1];
				bAutoRunning = true;//设置当前正常自动寻路状态 将在tick中更新位置
			}
		}
		//FollowTime=0.f;
		//bTargeting=false;//为何此处叠起来了又
	}
}

void AMyPlayerController::AbilityInputTagHold(FGameplayTag InputTag)
{
		if(!InputTag.MatchesTagExact(FMyGameplayTags::Get().InputTag_LMB))
		{
			if(GetASC())
			{
				GetASC()->AbilityInputTagHold(InputTag);
			}
			return;
		}

		if(bTargeting||bShiftKeyDown)
		{
			if(GetASC())
			{
				//点击敌人目标，将攻击敌人
				GetASC()->AbilityInputTagHold(InputTag);
			}
		}
		else
		{
			FollowTime+=GetWorld()->GetDeltaSeconds(); // 统计悬停时间来判断是否为点击
			if (CursorHit.bBlockingHit){CachedDestination=CursorHit.ImpactPoint;}//获取鼠标拾取位置
			if (APawn*ControlledPawn=GetPawn())
			{
				const FVector WorldDirection = (CachedDestination-ControlledPawn->GetActorLocation()).GetSafeNormal();
				ControlledPawn->AddMovementInput(WorldDirection);
			}
		}
	
}


UMyAbilitySystemComponentBase* AMyPlayerController::GetASC()
{
	if(MyAbilitySystemComponentBase == nullptr)
	{
		MyAbilitySystemComponentBase = Cast<UMyAbilitySystemComponentBase>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn()));
	}

	return MyAbilitySystemComponentBase;
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

	//绑定Shift按键事件
	EnhancedInputComponent->BindAction(ShiftAction, ETriggerEvent::Started, this, &ThisClass::ShiftPressed);
	EnhancedInputComponent->BindAction(ShiftAction, ETriggerEvent::Completed, this, &ThisClass::ShiftReleased);

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




