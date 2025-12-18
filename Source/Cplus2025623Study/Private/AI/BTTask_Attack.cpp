// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_Attack.h"


EBTNodeResult::Type UBTTask_Attack::ExecuteTask(
	UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory
)
{
	// 教材中这里什么都不做
	// 只是为了让蓝图 ReceiveExecuteAI 能被触发
	return Super::ExecuteTask(OwnerComp, NodeMemory);
}