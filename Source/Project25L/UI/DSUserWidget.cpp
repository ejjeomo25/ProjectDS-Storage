// Default
#include "UI/DSUserWidget.h"

// UE
#include "Components/Image.h"

// Game
#include "System/DSGameDataSubsystem.h"
UDSUserWidget::UDSUserWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, DefaultIcon(nullptr)
	, FocusedIcon(nullptr)
{
}

void UDSUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	FocusedIcon = LoadTexture(FocusedTexture);
	DefaultIcon = LoadTexture(DefaultTexture);
}

UTexture2D* UDSUserWidget::LoadTexture(TSoftObjectPtr<UTexture2D>& Data)
{
	UDSGameDataSubsystem::StreamableManager.LoadSynchronous(Data);

	return Data.Get();
}

void UDSUserWidget::SetFocus(UImage* Image, bool bIsFocus)
{

	if (false == IsValid(Image) || false == IsValid(FocusedIcon) || false == IsValid(DefaultIcon))
	{
		return;
	}

	if (bIsFocus)
	{
		Image->SetBrushFromTexture(FocusedIcon);
	}
	else
	{
		Image->SetBrushFromTexture(DefaultIcon);
	}
}
