// Default
#include "GameData/DSCharacterDataAsset.h"

// UE

// Game


uint8 UDSCharacterDataAsset::GetKey()
{
	uint8 EnumAsUint8 = static_cast<uint8>(Type);
	return EnumAsUint8;
}

FPrimaryAssetId UDSCharacterDataAsset::GetPrimaryAssetId() const
{
	return FPrimaryAssetId("DSCharacterDataAsset", GetFName());
}
