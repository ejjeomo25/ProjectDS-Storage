// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/DSGameMode.h"
#include "DSGameMode.h"

#include "DSLogChannels.h"

void ADSGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);

	DS_LOG(DSNetLog, Log, TEXT("111111"));
}

//APlayerController* ADSGameMode::Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
//{
//	APlayerController* NewPlayer = Super::Login(NewPlayer, InRemoteRole, Portal, Options, UniqueId, ErrorMessage);
//
//	DS_LOG(DSNetLog, Log, TEXT(""));
//
//	return NewPlayer;
//}



void ADSGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	DS_LOG(DSNetLog, Log, TEXT("22222"));
}

void ADSGameMode::StartPlay()
{
	Super::StartPlay();

	DS_LOG(DSNetLog, Log, TEXT("333333"));
}

APlayerController* ADSGameMode::SpawnPlayerController(ENetRole InRemoteRole, const FString& Options)
{
	APlayerController* NewPlayer = Super::SpawnPlayerController(InRemoteRole, Options);

	DS_LOG(DSNetLog, Log, TEXT("444444"));
	return NewPlayer;
}
