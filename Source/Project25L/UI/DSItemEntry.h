#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"

// UHT
#include "DSItemEntry.generated.h"

class UImage;
class UTextBlock;

UCLASS()
class PROJECT25L_API UDSItemEntry : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
	
public:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

	void SetFocus(bool bIsFocus);
protected:
	void LoadImg(TSoftObjectPtr<UTexture2D> &Data);


protected:
	
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> IMG_Background;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> IMG_ItemIcon;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> Text_ItemName;

};
