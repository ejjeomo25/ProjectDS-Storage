#pragma once
// Default
#include "CoreMinimal.h"

// UE
#include "Blueprint/UserWidget.h"

// UHT
#include "DS_HPBar.generated.h"

class UProgressBar;

UCLASS()
class PROJECT25L_API UDS_HPBar : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UDS_HPBar(const FObjectInitializer& ObjectInitializer);

	// 전달된 HP는 0~100 퍼센트 기준 (ex. 75.0f = 75%)
	void SetHP(float CurrentHP, float MaxHP);
protected:

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UProgressBar> ProgressBar_HP;
};
