// Default
#include "UI/DSPrimaryLayout.h"

// UE
#include "GameFramework/PlayerController.h"


// Game
#include "System/DSUIManagerSubsystem.h"
#include "DSLogChannels.h"
#include "UI/DSWidgetLayer.h"
#include "GameData/GameplayTag/DSGameplayTags.h"



UDSPrimaryLayout::UDSPrimaryLayout(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer)
	, LayersMap()
{

}

void UDSPrimaryLayout::NativeConstruct()
{
    Super::NativeConstruct();

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

	if (IsValid(FindLayer))
	{
		return FindLayer->PushWidget(WidgetTag);
	}

	return nullptr;
}

void UDSPrimaryLayout::PopContentfromLayer(FGameplayTag WidgetTag)
{
	UDSWidgetLayer* FindLayer = FindLayerByTag(WidgetTag);

	if (IsValid(FindLayer))
	{
		FindLayer->PopWidget();
	}
}



void UDSPrimaryLayout::ClearLayer(FGameplayTag WidgetTag)
{
	UDSWidgetLayer* FindLayer = FindLayerByTag(WidgetTag);
	if (IsValid(FindLayer))
	{
		FindLayer->ClearStack();
	}
}

void UDSPrimaryLayout::RegisterLayers()
{

	if (IsValid(GameMenuLayer) && IsValid(ModalLayer) && IsValid(GameLayer))
	{
		LayersMap.Add(FDSTags::GetDSTags().UI_Layer_GameMenu, GameMenuLayer);
		LayersMap.Add(FDSTags::GetDSTags().UI_Layer_Modal,  ModalLayer);
		LayersMap.Add(FDSTags::GetDSTags().UI_Layer_Game, GameLayer);
	}
	
	if (IsValid(GameLayer))
	{
		GameLayer->PushWidget(FDSTags::GetDSTags().UI_Layer_Game_Main);
	}
}

