#pragma once
// Default
#include "CoreMinimal.h"

// UE
#include "Engine/AssetManager.h"
#include "System/DSEnums.h"

// Game
#include "System/DSEventSystems.h"
#include "DSLogChannels.h"

// UHT
#include "DSGameDataSubsystem.generated.h"

class UDSCharacterDataAsset;
class UDSComboActionData;
class UDSDataTables;
struct FDSCharacterStat;
struct FDSNonCharacterStat;

/**
 * @brief 게임 인스턴스 수준에서 다양한 데이터 에셋 및 데이터 테이블을 관리하는 서브시스템.
 */
UCLASS()
class PROJECT25L_API UDSGameDataSubsystem final : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	UDSGameDataSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
 
	/**
	 * @brief 지정한 AssetType에 해당하는 DataAsset들을 비동기 로드하여, 지정된 TMap에 등록하는 템플릿 함수.
	 *
	 * UAssetManager를 통해 TAssetType에 해당하는 PrimaryAssetId 목록을 받아,
	 * 각 Asset을 비동기 로드 후 GetKey()로 고유 키(TKeyType)를 추출하여 OutDataAssetMap에 등록합니다.
	 * 이때 중복 키는 무시하고, 로드 실패 시 로그를 남깁니다.
	 * 모든 Asset이 로드되면 bIsAllAssetsLoaded를 true로 설정 후 OnAllAssetsLoaded 델리게이트를 broadcast합니다.
	 *
	 * @tparam TAssetType	로드할 DataAsset 클래스의 타입 (예: 캐릭터 데이터를 위한 UDSCharacterDataAsset)
	 * @tparam TKeyType		TMap 등록 시 사용될 키의 타입 (예: 캐릭터 데이터의 경우 ECharacterType과 같이 캐릭터 종류를 나타내는 열거형)
	 *
	 * @param OutDataAssetMap   로드된 DataAsset들을 저장할 TMap.
	 *                          각 항목의 키는 Asset의 GetKey()를 통해 얻은 값(TKeyType),
	 *							값은 해당 DataAsset 객체의 포인터(TAssetType*)
	 */
	template<typename TAssetType, typename TKeyType>
	void LoadDataAssetAsync(TMap<TKeyType, TAssetType*>& OutDataAssetMap);

	void LoadAllDataAssetsAsync();
	void LoadDataTables();

	const UDSCharacterDataAsset* GetCharacterDataByType(ECharacterType InType) const;
	const UDSComboActionData* GetComboActionData() const;

	static UDSGameDataSubsystem* Get(UObject* WorldContextObject);

	/**
	 * @brief Enum 값에 해당하는 이름의 행 데이터(FTableRowBase)를 반환하는 템플릿 함수.
	 *
	 * 주어진 DataType 값을 문자열 변환 후 "::"로 분리해 행 이름을 추출합니다.
	 * 주어진 EDataTableType에 해당하는 데이터 테이블에 해당 행(TRowType)이 있으면 포인터를, 없으면 nullptr을 반환합니다
	 *
	 * @tparam TRowType		반환할 행 데이터 구조체의 타입 (예: 캐릭터 스탯 정보를 담은 FDSCharacterStat)
	 * @tparam TEnumType    데이터 테이블 행을 식별하는 Enum 타입 (예: 캐릭터 타입을 구분하는 ECharacterType)
	 *
	 * @param DataTableType 검색할 데이터 테이블의 종류 (EDataTableType)
	 * @param DataType		찾고자 하는 행 데이터에 해당하는 Enum 값 (예: ECharacterType::Girl)
	 *
	 * @return TRowType*   검색된 행 데이터의 포인터, 찾을 수 없는 경우 nullptr 반환
	 */
	template<typename TRowType, typename TEnumType>
	TRowType* GetDataRowByEnum(EDataTableType DataTableType, TEnumType DataType);

	/**
	 * @brief ID에 해당하는 이름의 행 데이터(FTableRowBase)를 반환하는 함수.
	 *
	 * @param DataTableType 검색할 데이터 테이블의 종류 (EDataTableType)
	 * @param DataID		찾고자 하는 행 데이터에 해당하는 ID 값 (예: 202)
	 *
	 * @return FTableRowBase*   검색된 행 데이터의 포인터, 찾을 수 없는 경우 nullptr 반환
	 */
	FTableRowBase* GetDataRowByID(EDataTableType DataTableType, int32 DataID);

	/**
	 * @brief DataTable들을 관리하는 DataTables 데이터 에셋을 반환하는 함수.
	 */
	UFUNCTION(BlueprintCallable, Category = "Data Table")
	const UDSDataTables* GetDataTables() const { return DataTables; }

	/**
	 * @brief 지정한 EDataTableType에 해당하는 UDataTable을 반환하는 함수.
	 */
	UFUNCTION(BlueprintCallable, Category = "Data Table")
	UDataTable* GetDataTable(EDataTableType TableType) const;

	UPROPERTY(EditDefaultsOnly, Category = "Data Table")
	TSoftObjectPtr<UDSDataTables> DataTablesAsset;

protected:
	UPROPERTY()
	UDSDataTables* DataTables;

	UPROPERTY()
	bool bIsAllAssetsLoaded = false;

	/** 비동기 로드해야 하는 Data Asset 종류(번들) 개수. */
	UPROPERTY()
	int PendingAssetLoadsCount = 0;

public:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Data Map")
	TMap<ECharacterType, UDSCharacterDataAsset*> CharacterDataMap;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Data Map")
	TObjectPtr<UDSComboActionData> ComboActionData;
};

template<typename TAssetType, typename TKeyType>
void UDSGameDataSubsystem::LoadDataAssetAsync(TMap<TKeyType, TAssetType*>& OutDataAssetMap)
{
	UAssetManager& AssetManager = UAssetManager::Get();
	TArray<FPrimaryAssetId> AssetIds;
	const FPrimaryAssetType& AssetType = TAssetType::StaticClass()->GetDefaultObject()->GetPrimaryAssetId().PrimaryAssetType;

	AssetManager.GetPrimaryAssetIdList(AssetType, AssetIds);
	if (AssetIds.Num() == 0)
	{
		DS_LOG(DSDataLog, Warning, TEXT("No assets of type %s found!"), *AssetType.ToString());
		return;
	}

	TWeakObjectPtr<UDSGameDataSubsystem> WeakThis = this;
	AssetManager.LoadPrimaryAssets(AssetIds, TArray<FName>(), FStreamableDelegate::CreateLambda([WeakThis, AssetIds, AssetType, &OutDataAssetMap]()
		{
			if (!WeakThis.IsValid())
			{
				DS_LOG(DSDataLog, Warning, TEXT("Owner object is no longer valid. Aborting asset loading."));
				return;
			}

			UAssetManager& AssetManager = UAssetManager::Get();
			for (const FPrimaryAssetId& AssetId : AssetIds)
			{
				TAssetType* Asset = Cast<TAssetType>(AssetManager.GetPrimaryAssetObject(AssetId));
				if (IsValid(Asset))
				{
					TKeyType Key = static_cast<TKeyType>(Asset->GetKey());

					if (OutDataAssetMap.Contains(Key))
					{
						DS_LOG(DSDataLog, Warning, TEXT("Duplicate asset detected. Asset '%s' is skipped because a data asset for this key is already loaded."), *Asset->GetName());
					}
					else
					{
						OutDataAssetMap.Add(Key, Asset);
						DS_LOG(DSDataLog, Log, TEXT("Loaded asset: %s"), *Asset->GetName());
					}
				}
				else
				{
					DS_LOG(DSDataLog, Error, TEXT("Failed to load asset: %s"), *AssetId.ToString());
				}
			}
			DS_LOG(DSDataLog, Log, TEXT("All assets of type %s loaded successfully!"), *AssetType.ToString());

			if (--WeakThis->PendingAssetLoadsCount == 0)
			{
				DS_LOG(DSDataLog, Log, TEXT("All assets loaded successfully!"));
				WeakThis->bIsAllAssetsLoaded = true;
				DSEVENT_DELEGATE_INVOKE(GameEvent.OnAllAssetsLoaded);
			}
		}));
}

template<typename TRowType, typename TEnumType>
TRowType* UDSGameDataSubsystem::GetDataRowByEnum(EDataTableType DataTableType, TEnumType DataType)
{
	UDataTable* DataTable = GetDataTable(DataTableType);

	if (IsValid(DataTable))
	{
		FString DataName = UEnum::GetValueAsString(DataType);
		FString Left, Right;
		DataName.Split(TEXT("::"), &Left, &Right);

		FName DataRightName = FName(Right);

		TRowType* Row = DataTable->FindRow<TRowType>(DataRightName, DataRightName.ToString());

		if (Row != nullptr)
		{
			return Row;
		}
	}

	return nullptr;
}