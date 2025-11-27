// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/MyPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "EnhancedInputSubsystems.h"
#include "MyGameplayTags.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "AbilitySystem/MyAbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/MyAttributeSet.h"
#include "Components/SplineComponent.h"
#include "GameFramework/Character.h"
#include "Input/InputComponentBase.h"
#include "Interaction/EnemyInterface.h"
#include "UI/Widget/DamageTextComponent.h"


AMyPlayerController::AMyPlayerController()  	//姝ゅ鏄瀯閫犲嚱鏁?涓轰粈涔堟瀯閫犲嚱鏁颁笉鐢ㄥ啓鍦ㄦ渶鍓嶉潰?瀹冩槸鍦ㄥ璞″垱寤烘椂璋冪敤鐨?鎴戣皟鏁磋繃浣嶇疆 闂鎹㈡垚涓轰粈涔堜笉鏀炬渶鍓嶉潰涔熻兘姝ｅ父杩愯
{
	bReplicates = true;//鏄惁灏嗘暟鎹紶閫佹湇鍔″櫒鏇存柊

	LastActor=nullptr;
	ThisActor=nullptr;
	
	Spline =CreateDefaultSubobject<USplineComponent>("Spline"); //鍦ㄦ瀯閫犲嚱鏁颁腑灏唖pline鍒濆鍖?
}

void AMyPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
//榧犳爣浣嶇疆灏勭嚎杩借釜
	CursorTrace();
//鑷姩瀵昏矾
	AutoRun();
}

void AMyPlayerController::ShowDamageNumber_Implementation(float DamageAmount, ACharacter* TargetCharacter, bool IsBlockedHit, bool IsCriticalHit)
{
	//棣栧厛瀵瑰弬鏁拌繘琛屽垽鏂紝浠ュ強瀵圭被鍒ゆ柇锛岃繖閲岀‘淇濇墽琛岄€昏緫鏃讹紝鐩爣瑙掕壊杩樻病鏈夎閿€姣?浣嗕负浠€涔堟槸鐩爣瑙掕壊?濡傛灉鐩爣瑙掕壊琚攢姣佷簡灏变笉鑳芥樉绀轰簡鍚?
	if(IsValid(TargetCharacter) && DamageTextComponentClass)
	{
		//鐒跺悗鍦ㄥ唴閮ㄥ疄渚嬪寲缁勪欢锛屽苟娉ㄥ唽锛屽湪瀹炰緥鍖栨椂锛岀涓€涓弬鏁扮浉褰撲簬浣滀负姝ょ粍浠剁殑鐖剁被锛屽綋PlayerController琚攢姣佹椂锛屽畠涔熶細琚攢姣併€?
		UDamageTextComponent* DamageText = NewObject<UDamageTextComponent>(TargetCharacter, DamageTextComponentClass);
		DamageText->RegisterComponent(); //鍔ㄦ€佸垱寤虹殑缁勪欢闇€瑕佽皟鐢ㄦ敞鍐?
		
		//鍦ㄨ幏鍙栧埌浣嶇疆鍚庯紝鐒跺悗灏嗗叾鍜岃鑹插垎绂伙紝闃叉瑙掕壊绉诲姩锛屼激瀹虫暟瀛椾篃璺熼殢绉诲姩銆?
		DamageText->AttachToComponent(TargetCharacter->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform); //鍏堥檮鍔犲埌瑙掕壊韬笂锛屼娇鐢ㄨ鑹蹭綅缃?
		DamageText->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform); //鐒跺悗浠庤鑹茶韩涓婂垎绂伙紝淇濊瘉鍦ㄤ竴涓綅缃挱鏀惧畬鎴愬姩鐢?
		DamageText->SetDamageText(DamageAmount,IsBlockedHit,IsCriticalHit); //璁剧疆鏄剧ず鐨勪激瀹虫暟瀛?
		
		
		
	}
}

void AMyPlayerController::AutoRun()
{
	if(!bAutoRunning) return;
	if(APawn* ControlledPawn = GetPawn())
	{
		//鎵惧埌璺濈鏍锋潯鏈€杩戠殑浣嶇疆
		const FVector LocationOnSpline = Spline->FindLocationClosestToWorldLocation(ControlledPawn->GetActorLocation(), ESplineCoordinateSpace::World);
		//鑾峰彇杩欎釜浣嶇疆鍦ㄦ牱鏉′笂鐨勬柟鍚?
		const FVector Direction = Spline->FindDirectionClosestToWorldLocation(LocationOnSpline, ESplineCoordinateSpace::World);
		ControlledPawn->AddMovementInput(Direction);

		const float DistanceToDestination = (LocationOnSpline - CachedDestination).Length();
		if(DistanceToDestination <= AutoRunAcceptanceRadius)
		{
			bAutoRunning = false;
		}
	}

}
void AMyPlayerController::CursorTrace()//榧犳爣浣嶇疆杩借釜浠ｇ爜
{

	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
	if (!CursorHit.bBlockingHit)return;//濡傛灉涓哄懡涓繑鍥炵粨鏋?

	LastActor = ThisActor;
	ThisActor = Cast<IEnemyInterface>(CursorHit.GetActor());

	if (ThisActor!=LastActor)
	{
		//杩欐鎬庝箞鎶樺彔璧锋潵鐨?
		if (ThisActor)ThisActor->HighlightActor();
		if (LastActor)LastActor->UnHighlightActor();
	}
	//鑰佺殑鍒ゅ畾杩借釜浠ｇ爜
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

void AMyPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
    if (InputTag.MatchesTagExact(FMyGameplayTags::Get().InputTag_LMB))
    {
        bTargeting=ThisActor != nullptr;
        bAutoRunning = false;
        FollowTime=0.f;
    }
    if (GetASC())
    {
        GetASC()->AbilityInputTagPressed(InputTag);
    }
}

void AMyPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
    if(!InputTag.MatchesTagExact(FMyGameplayTags::Get().InputTag_LMB))
    {
        if(GetASC())
        {
            GetASC()->AbilityInputTagReleased(InputTag);
        }
        return;
    }
    
    if (bTargeting||bShiftKeyDown)
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
                Spline->ClearSplinePoints();
                for (const FVector& PointLoc:NavPath->PathPoints)
                {
                    Spline->AddSplinePoint(PointLoc,ESplineCoordinateSpace::World);
                    DrawDebugSphere(GetWorld(), PointLoc, 8.f, 8, FColor::Orange, false, 5.f);
                }
                CachedDestination=NavPath->PathPoints[NavPath->PathPoints.Num()-1];
                bAutoRunning = true;
            }
        }
    }
    if (GetASC())
    {
        GetASC()->AbilityInputTagReleased(InputTag);
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
                GetASC()->AbilityInputTagHold(InputTag);
            }
        }
        else
        {
            FollowTime+=GetWorld()->GetDeltaSeconds();
            if (CursorHit.bBlockingHit){CachedDestination=CursorHit.ImpactPoint;}
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

	//浠庢湰鍦拌鑹茶韩涓婅幏鍙栧埌瀹冪殑瀛愮郴缁?
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (Subsystem)
	{
	
	Subsystem->AddMappingContext(AuraContext, 0);//鍙互瀛樺湪澶氫釜鎿嶄綔鏄犲皠锛屾牴鎹紭鍏堢骇瑙﹀彂
		}

	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	
	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);//灏嗛紶鏍囬攣瀹氬湪瑙嗗彛鍐?
	InputModeData.SetHideCursorDuringCapture(false);//榧犳爣琚崟鑾锋椂鏄惁闅愯棌
	SetInputMode(InputModeData);
}

void AMyPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UInputComponentBase* EnhancedInputComponent = CastChecked<UInputComponentBase>(InputComponent);//姝ゅ涓轰粈涔堜粠EnhancedInputComponent鍒囨崲鍒癠InputComponentBase?杩欓噷闈笉鏄病鏈夌Щ鍔ㄧ殑閫昏緫鍚?鍩烘湰娌′簨浜? =浠栫户鎵胯嚜EnhancedInputComponent

	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMyPlayerController::Move);

	//缁戝畾Shift鎸夐敭浜嬩欢
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







