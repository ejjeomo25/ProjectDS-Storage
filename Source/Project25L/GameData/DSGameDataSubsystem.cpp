// Default
#include "GameData/DSGameDataSubsystem.h"

// UE
#include "Subsystems/GameInstanceSubsystem.h"

// Game
#include "GameData/DSCharacterDataAsset.h"
#include "GameData/DSCharacterStat.h"
#include "GameData/DSDataTables.h"
#include "GameData/DSNonCharacterStat.h"

UDSGameDataSubsystem::UDSGameDataSubsystem()
{
	static const FSoftObjectPath DataTablesAssetPath(TEXT("/Game/GameData/DataAsset/DA_DataTables.DA_DataTables"));
	DataTablesAsset = DataTablesAssetPath;
}

void UDSGameDataSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	LoadDataTables();
	LoadAllDataAssets();
}

void UDSGameDataSubsystem::Deinitialize()
{
	DataTables = nullptr;
	Super::Deinitialize();
}

void UDSGameDataSubsystem::LoadAllDataAssets()
{
	LoadDataAsset<UDSCharacterDataAsset, ECharacterType>(CharacterDataMap);

}

void UDSGameDataSubsystem::LoadDataTables()
{
	if (DataTablesAsset.IsValid())
	{
		DataTables = DataTablesAsset.Get();
	}
	else if (DataTablesAsset.ToSoftObjectPath().IsValid())
	{
		DataTables = DataTablesAsset.LoadSynchronous();
		DS_LOG(LogTemp, Log, TEXT("DataTables is loaded"));
	}

	if (!DataTables)
	{
		DS_LOG(LogTemp, Warning, TEXT("DataTables asset is not loaded or assigned!"));
	}
}

const UDSCharacterDataAsset* UDSGameDataSubsystem::GetCharacterDataByType(ECharacterType InType) const
{
	if (const UDSCharacterDataAsset* const* FoundData = CharacterDataMap.Find(InType))
	{
		return *FoundData;
	}
	return nullptr;
}

UDSGameDataSubsystem* UDSGameDataSubsystem::Get(UObject* WorldContextObject)
{
	if (!WorldContextObject)
	{
		return nullptr;
	}

	UWorld* World = WorldContextObject->GetWorld();
	check(World);

	UGameInstance* GameInstance = World->GetGameInstance();
	check(GameInstance);

	return GameInstance->GetSubsystem<UDSGameDataSubsystem>();
}

FTableRowBase* UDSGameDataSubsystem::GetItemData(EDataTableType DataTableType, int32 ItemID)
{
	UDataTable* DataTable = GetDataTable(DataTableType);

	if (IsValid(DataTable))
	{
		FString ID = FString::Printf(TEXT("%d"), ItemID);

		FName DataName = FName(ID);

		FTableRowBase* Row = DataTable->FindRow<FTableRowBase>(DataName, ID);

		if (Row != nullptr)
		{
			return Row;
		}
	}

	return nullptr;
}

UDataTable* UDSGameDataSubsystem::GetDataTable(EDataTableType InTableType) const
{
	if (DataTables)
	{
		const TObjectPtr<UDataTable>* FoundTable = DataTables->DataTableMap.Find(InTableType);
		if (FoundTable)
		{
			return FoundTable->Get();
		}
	}

	DS_LOG(LogTemp, Warning, TEXT("Requested DataTable for type %d not found!"), static_cast<uint8>(InTableType));
	return nullptr;
}