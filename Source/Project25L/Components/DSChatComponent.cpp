// Default
#include "Components/DSChatComponent.h"

// Game
#include "DSLogChannels.h"
#include "Player/DSPlayerController.h"
#include "Player/DSPlayerState.h"
#include "System/DSEventSystems.h"

UDSChatComponent::UDSChatComponent()
{
	
}

void UDSChatComponent::BeginPlay()
{
	Super::BeginPlay();

	SetIsReplicated(true);
}

void UDSChatComponent::SendChatMessageToServer(const FText& Message)
{
	if (false == Message.IsEmpty())
	{
		ServerRPC_SendChatMessage(Message);
	}
}

void UDSChatComponent::ServerRPC_SendChatMessage_Implementation(const FText& Message)
{
	APlayerController* PlayerController = Cast<APlayerController>(GetOwner());
	if(false == IsValid(PlayerController))
	{
		return ;
	}
	ADSPlayerState* PlayerState = Cast<ADSPlayerState>(PlayerController->PlayerState);

	if(false == IsValid(PlayerState))
	{
		return;
	}

	const ECharacterType SenderType = PlayerState->GetCharacterType();

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ADSPlayerController* OtherPC = Cast<ADSPlayerController>(It->Get());
		if (false == IsValid(OtherPC)) 
		{
			continue;
		}
		
		if (GetOwner() == OtherPC)
		{
			continue;
		}

		ADSPlayerState* OtherPS = Cast<ADSPlayerState>(OtherPC->PlayerState);
		if (false == IsValid(OtherPS)) 
		{
			continue;
		}

		const ECharacterType ReceiverType = OtherPS->GetCharacterType();

		// 나와 같은 타입이면
		if (ReceiverType == SenderType)
		{
			continue;
		}

		if(IsValid(OtherPC->GetChatComponent()))
		{
			OtherPC->GetChatComponent()->ClientRPC_ReceiveChatMessage(SenderType, Message);
		}
	}
}

void UDSChatComponent::ClientRPC_ReceiveChatMessage_Implementation(const ECharacterType SenderType, const FText& Message)
{
	OnChatReceived.Broadcast(SenderType, Message);
}
