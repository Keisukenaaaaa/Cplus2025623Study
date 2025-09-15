// Fill out your copyright notice in the Description page of Project Settings.
//这页代码中关于网络同步的问题需要好好研究一下,什么是预测窗口?

#include "AbilitySystem/TargetDataUnderMouse.h"

#include "Player/MyPlayerController.h"

UTargetDataUnderMouse* UTargetDataUnderMouse::CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility)
{
	UTargetDataUnderMouse * MyObj=NewAbilityTask<UTargetDataUnderMouse>(OwningAbility);
	return MyObj;
}

void UTargetDataUnderMouse::Activate()
{
	//是否由客户端控制
	const bool bIsLocallyControlled=Ability->GetCurrentActorInfo()->IsLocallyControlled();
	if (bIsLocallyControlled)
	{
		SendMouseCursorData();
	}
	else
	{
		const FGameplayAbilitySpecHandle SpecHandle=GetAbilitySpecHandle();
		const FPredictionKey ActivationPredictionKey = GetActivationPredictionKey();
		AbilitySystemComponent.Get()->AbilityTargetDataSetDelegate(SpecHandle,ActivationPredictionKey).AddUObject(this,&UTargetDataUnderMouse::OnTargetDataReplicatedCallback);

		//判断在服务器上 上面的委托是否已经广播过?
		const bool bCalledDelegate = AbilitySystemComponent.Get()->CallReplicatedTargetDataDelegatesIfSet(SpecHandle, ActivationPredictionKey);
		if(!bCalledDelegate)
		{
			//设置服务器端等待PlayerData数据的上传
			SetWaitingOnRemotePlayerData();
		}
	}

}

void UTargetDataUnderMouse::SendMouseCursorData()
{
	//创建一个预测窗口 该窗口允许客户端在不确定服务器响应的情况下 对游戏状态进行预测性更新;
	//->是啥意思
	FScopedPredictionWindow ScopedPrediction(AbilitySystemComponent.Get(),true);

	//获取鼠标拾取结果`;为什么此处要重复获取
	//此处是否可以写成MYplayercontroller? 为什么不能?他不是继承了父类也该有这个方法吗?
	APlayerController* PC =Ability->GetCurrentActorInfo()->PlayerController.Get();
	FHitResult CursorHit;
	PC->GetHitResultUnderCursor(ECC_Visibility,false,CursorHit);

	//创建需要上传服务器端的TargetData
	FGameplayAbilityTargetData_SingleTargetHit *Data=new FGameplayAbilityTargetData_SingleTargetHit();
	Data->HitResult=CursorHit;
	
	//创建TargetData句柄上传到服务器需要上传句柄
	FGameplayAbilityTargetDataHandle DataHandle;
	DataHandle.Add(Data);

	//将targetData上传只服务器端;
	//不是这些方法哪来的啊?
	AbilitySystemComponent->ServerSetReplicatedTargetData(
		GetAbilitySpecHandle(),
		GetActivationPredictionKey(),
		DataHandle,
		FGameplayTag(),
		AbilitySystemComponent->ScopedPredictionKey);

	//判断服务器端是否通过验证
	if(ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(DataHandle);
	}
}

void UTargetDataUnderMouse::OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle,
	FGameplayTag ActivationTag)
{
	//通知客户端 服务器端已经收到并处理了从客户端复制的数据目标(将服务器的targetData应用到客户端,并清除掉缓存)
	AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(),GetActivationPredictionKey());
	//判断服务器是否通过验证
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(DataHandle);
	}
}
