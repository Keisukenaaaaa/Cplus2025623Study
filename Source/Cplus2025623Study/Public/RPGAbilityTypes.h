#pragma once //预处理指令 确保这个头文件只被包含（include）一次，防止重复定义。
//为什么这个代码要手动在IDE里创建?是因为没有基本类?

#include "GameplayEffectTypes.h"
#include "RPGAbilityTypes.generated.h"
//在蓝图中定义结构体，并继承FGameplayEffectContext?什么意思 这是蓝图吗?
USTRUCT(BlueprintType) //在蓝图中可作为类型使用
struct FRPGGameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY() //宏 自动生成构造函数、析构函数、拷贝构造函数等

public:
	bool IsBlockedHit() const { return bIsBlockedHit; }
	bool IsCriticalHit() const { return BIsCriticalHit; }

	void SetIsBlockedHit(const bool bInIsBlockedHit) { bIsBlockedHit = bInIsBlockedHit; }
	void SetIsCriticalHit(const bool bInIsCriticalHit) { BIsCriticalHit = bInIsCriticalHit; }

	//为什么此处的方法要写在头文件里
	/** 返回用于序列化的实际结构体 */
	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FGameplayEffectContext::StaticStruct();
	}

	/** 用于序列化类的参数 */
	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess) override;
	//这段序列化的源码需要去问下gpt

protected:
	//增加两个参数，用于设置或者获取当前技能是否暴击或者格挡
	UPROPERTY()
	bool bIsBlockedHit = false; //格挡

	UPROPERTY()
	bool BIsCriticalHit = false; //暴击
};


