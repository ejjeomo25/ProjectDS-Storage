// Default
#include "GameData/DSCharacterDataAsset.h"

// UE

// Game


uint32 UDSCharacterDataAsset::GetKey()
{
	uint32 EnumAsUint8 = static_cast<uint32>(Type);
	return EnumAsUint8;
}

FPrimaryAssetId UDSCharacterDataAsset::GetPrimaryAssetId() const
{
	return FPrimaryAssetId("DSCharacterDataAsset", GetFName());
}
