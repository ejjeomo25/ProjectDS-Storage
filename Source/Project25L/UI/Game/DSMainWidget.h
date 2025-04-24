#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "Blueprint/UserWidget.h"

// UHT
#include "DSMainWidget.generated.h"

class UVerticalBox;
class UDSPlayerInfo;
class UDS_HPBar;

UCLASS()
class PROJECT25L_API UDSMainWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UDSMainWidget(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;

	UDS_HPBar* GetHPBar(int32 PlayerIndex);
	UDSPlayerInfo* GetPlayerInfo(int32 PlayerIndex);
protected:
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UVerticalBox> VBox_Info;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UDSPlayerInfo> PlayerInfo;

	/*0번이 개인, 나머진 각자*/
	UPROPERTY(Transient)
	TArray<TObjectPtr<UDSPlayerInfo>> PlayerInfos;

};
