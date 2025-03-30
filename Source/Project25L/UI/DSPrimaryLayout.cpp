// Default
#include "UI/DSPrimaryLayout.h"

// UE
#include "GameFramework/PlayerController.h"


// Game
#include "System/DSUIManagerSubsystem.h"
#include "DSLogChannels.h"
#include "UI/DSWidgetLayer.h"

void UDSPrimaryLayout::NativeConstruct()
{
    Super::NativeConstruct();
	if (!GameMenuLayer)
	{
		GameMenuLayer = Cast<UDSWidgetLayer>(GetWidgetFromName(TEXT("GameMenuLayer")));
	}
	if (!ModalLayer)
	{
		ModalLayer = Cast<UDSWidgetLayer>(GetWidgetFromName(TEXT("ModalLayer")));
	}
	RegisterLayers();
	
}

UDSWidgetLayer* UDSPrimaryLayout::FindLayerByTag(FGameplayTag WidgetTag)
{
	for (const auto& Pair : LayersMap)
	{
		if (WidgetTag.MatchesTag(Pair.Key)) 
		{
			return Pair.Value;
		}
	}
	return nullptr;
}

UUserWidget* UDSPrimaryLayout::PushContentToLayer(FGameplayTag WidgetTag)
{

	UDSWidgetLayer* FindLayer = FindLayerByTag(WidgetTag);

	if (FindLayer)
	{
		return FindLayer->PushWidget(WidgetTag);
	}

	return nullptr;
}

void UDSPrimaryLayout::PopContentfromLayer(FGameplayTag WidgetTag)
{
	UDSWidgetLayer* FindLayer = FindLayerByTag(WidgetTag);

	if (FindLayer)
	{
		FindLayer->PopWidget();
	}
}



void UDSPrimaryLayout::ClearLayer(FGameplayTag WidgetTag)
{
	UDSWidgetLayer* FindLayer = FindLayerByTag(WidgetTag);

	if (FindLayer)
	{
		FindLayer->ClearStack();
	}
}

void UDSPrimaryLayout::RegisterLayers()
{
	LayersMap.Add(FGameplayTag::RequestGameplayTag(FName("UI.Layer.GameMenu")), GameMenuLayer);
	LayersMap.Add(FGameplayTag::RequestGameplayTag(FName("UI.Layer.Modal")), ModalLayer);

}

