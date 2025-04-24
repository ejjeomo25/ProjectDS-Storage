#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "Components/ListView.h"

// UHT
#include "DSListView.generated.h"

UCLASS()
class PROJECT25L_API UDSListView : public UListView
{
	GENERATED_BODY()
	
protected:
	virtual void HandleListEntryHovered(UUserWidget& EntryWidget) override;
	virtual void HandleListEntryUnhovered(UUserWidget& EntryWidget) override;
	virtual void OnItemClickedInternal(UObject* Item) override;
};
