#pragma once
// Default
#include "CoreMinimal.h"

// Game
#include "UI/Base/DSUserWidget.h"

// UHT
#include "DSReadyBoardPanel.generated.h"


UCLASS()
class PROJECT25L_API UDSReadyBoardPanel : public UDSUserWidget
{
	GENERATED_BODY()
	
public:
	UDSReadyBoardPanel(const FObjectInitializer& ObjectInitializer);
};
