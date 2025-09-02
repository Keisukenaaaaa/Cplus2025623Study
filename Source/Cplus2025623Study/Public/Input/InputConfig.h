
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "InputConfig.generated.h"

USTRUCT(BlueprintType)
struct FInputActionStruct
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	const class UInputAction* InputAction=nullptr;
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag InputTag=FGameplayTag();
};
/**
 * 
 */
UCLASS()
class CPLUS2025623STUDY_API UInputConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	TArray<FInputActionStruct> AbilityInputActions;

	const UInputAction* FindAbilityInputActionForTag(const FGameplayTag& InputTag,bool bLogNotFound=false) const;
	
};
