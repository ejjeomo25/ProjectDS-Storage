#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "Blueprint/UserWidget.h"

// UHT
#include "DSPlayerInfo.generated.h"

class UDS_HPBar;
class UTextBlock;
class UImage;

UCLASS()
class PROJECT25L_API UDSPlayerInfo : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UDSPlayerInfo(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;
public:
	void SetText(float CurrentHP, float MaxHP);

	UDS_HPBar* GetHPBar() { return ProgressBar_HP; }
protected:
	
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UDS_HPBar> ProgressBar_HP;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> IMG_PlayerIcon;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> Text_PlayerName;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> Text_HP;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> Text_MaxHP;

};
