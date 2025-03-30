// Default
#include "GameData/DSGameDataSubsystem.h"

// UE
#include "Subsystems/GameInstanceSubsystem.h"

// Game
#include "GameData/DSCharacterDataAsset.h"
#include "GameData/DSCharacterStat.h"
#include "GameData/DSDataTables.h"
#include "GameData/DSNonCharacterStat.h"
#include "GameData/Skill/DSComboActionData.h"

UDSGameDataSubsystem::UDSGameDataSubsystem()
{
	static const FSoftObjectPath DataTablesAssetPath(TEXT("/Game/GameData/DataAsset/DA_DataTables.DA_DataTables"));
	DataTablesAsset = DataTablesAssetPath;

	//*****************코드 리뷰 : PendingAssetLoadsCount 초기화 리스트 ****************************//
}

void UDSGameDataSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	LoadDataTables();
	LoadAllDataAssetsAsync();
}

void UDSGameDataSubsystem::Deinitialize()
{
	DataTables = nullptr;
	Super::Deinitialize();
}

void UDSGameDataSubsystem::LoadAllDataAssetsAsync()
{
	PendingAssetLoadsCount = 1;

	LoadDataAssetAsync<UDSCharacterDataAsset, ECharacterType>(CharacterDataMap);

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
		DS_LOG(DSDataLog, Log, TEXT("DataTables is loaded"));
	}

	//*****************코드 리뷰 : nullptr ****************************//
	if (!DataTables)
	{
		DS_LOG(DSDataLog, Warning, TEXT("DataTables asset is not loaded or assigned!"));
	}
}
//*****************코드 리뷰 : 템플릿화 ****************************//
const UDSCharacterDataAsset* UDSGameDataSubsystem::GetCharacterDataByType(ECharacterType InType) const
{

	//*****************코드 리뷰 : 분리해서 작성해라.***************************//
	/* if문에 다 때려넣지 말아라. 가독성이 안좋다.*/
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

EDataTableType UDSGameDataSubsystem::ConvertToDataTableType(ECharacterType CharacterType)
{
	switch(CharacterType)
	{
	case ECharacterType::Girl:
		return EDataTableType::GirlSkillAttributeData;
	case ECharacterType::Boy:
		return EDataTableType::BoySkillAttributeData;
	case ECharacterType::Mister:
		return EDataTableType::MisterSkillAttributeData;
	}
	return EDataTableType();
}

FTableRowBase* UDSGameDataSubsystem::GetDataRowByID(EDataTableType DataTableType, int32 DataID)
{
	UDataTable* DataTable = GetDataTable(DataTableType);

	if (IsValid(DataTable))
	{
		FString ID = FString::Printf(TEXT("%d"), DataID);

		FName DataName = FName(ID);

		FTableRowBase* Row = DataTable->FindRow<FTableRowBase>(DataName, ID);

		if (nullptr != Row)
		{
			return Row;
		}
	}

	return nullptr;
}

UDataTable* UDSGameDataSubsystem::GetDataTable(EDataTableType InTableType) const
{

	//*****************코드 리뷰 : nullptr 체크 ****************************//
	if (DataTables)
	{
		const TObjectPtr<UDataTable>* FoundTable = DataTables->DataTableMap.Find(InTableType);


		//*****************코드 리뷰 : nullptr 체크 ****************************//
		if (FoundTable)
		{
			return FoundTable->Get();
		}
	}

	DS_LOG(DSDataLog, Warning, TEXT("Requested DataTable for type %d not found!"), static_cast<uint8>(InTableType));
	return nullptr;
}