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
class UDSDataTables;
struct FDSCharacterStat;
struct FDSNonCharacterStat;

UCLASS()
class PROJECT25L_API UDSGameDataSubsystem final : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	UDSGameDataSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
 
	/**
	 * @brief 주어진 AssetType에 해당하는 DataAsset들을 비동기 로드하여 지정한 TMap에 등록하는 템플릿 함수.
	 *
	 * @tparam TAssetType 로드할 DataAsset 클래스 타입입니다.
	 *                    예를 들어, 캐릭터 데이터를 위한 UDSCharacterDataAsset, 스킬 데이터를 위한 UDSSkillData 등.
	 * @tparam TKeyType     TMap에 등록할 때 사용할 Key의 타입입니다.
	 *                  예를 들어, 캐릭터 데이터의 경우 캐릭터 종류인 ECharacterType을 사용합니다.
	 *
	 * @param OutDataAssetMap   로드한 DataAsset들을 저장할 TMap입니다.
	 *                          키는 GetKey 함수를 통해 얻은 값(TKeyType), 값은 해당 DataAsset의 포인터(TAssetType*)가 됩니다.
	 */
	template<typename TAssetType, typename TKeyType>
	void LoadDataAsset(TMap<TKeyType, TAssetType*>& OutDataAssetMap)
	{
		UAssetManager& AssetManager = UAssetManager::Get();
		TArray<FPrimaryAssetId> AssetIds;
		const FPrimaryAssetType& AssetType = TAssetType::StaticClass()->GetDefaultObject()->GetPrimaryAssetId().PrimaryAssetType;

		AssetManager.GetPrimaryAssetIdList(AssetType, AssetIds);
		if (AssetIds.Num() == 0)
		{
			DS_LOG(LogTemp, Warning, TEXT("No assets of type %s found!"), *AssetType.ToString());
			return;
		}

		TWeakObjectPtr<UDSGameDataSubsystem> WeakThis = this;
		AssetManager.LoadPrimaryAssets(AssetIds, TArray<FName>(), FStreamableDelegate::CreateLambda([WeakThis, AssetIds, AssetType, &OutDataAssetMap]()
			{
				if (!WeakThis.IsValid())
				{
					DS_LOG(LogTemp, Warning, TEXT("Owner object is no longer valid. Aborting asset loading."));
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
							DS_LOG(LogTemp, Warning, TEXT("Duplicate asset detected. Asset '%s' is skipped because a data asset for this key is already loaded."), *Asset->GetName());
						}
						else
						{
							OutDataAssetMap.Add(Key, Asset);
							DS_LOG(LogTemp, Log, TEXT("Loaded asset: %s"), *Asset->GetName());
						}
					}
					else
					{
						DS_LOG(LogTemp, Error, TEXT("Failed to load asset: %s"), *AssetId.ToString());
					}
				}
				DS_LOG(LogTemp, Log, TEXT("All assets of type %s loaded successfully!"), *AssetType.ToString());

				if (--WeakThis->PendingAssetLoadsCount == 0)
				{
					DS_LOG(LogTemp, Log, TEXT("All assets loaded successfully!"));
					WeakThis->bIsAllAssetsLoaded = true;
					DSEVENT_DELEGATE_INVOKE(GameEvent.OnAllAssetsLoaded);
				}
			}));
	}
	void LoadAllDataAssets();

	void LoadDataTables();
	const UDSCharacterDataAsset* GetCharacterDataByType(ECharacterType InType) const;
	static UDSGameDataSubsystem* Get(UObject* WorldContextObject);

	/**
	 * @brief 
	 *
	 * @tparam TDataRow		리턴할 데이터 테이블의 한 줄 정보
	 *						예를 들어, 몬스터에 대한 struct 정보 한줄 
	 * @tparam TEnumType    세부적인 데이터 타입
	 *						예를 들어, 몬스터 데이터 테이블의 실제 몬스터 타입
	 *
	 * @param DataTableType 사용할 데이터 테이블 타입  
	 *                   
	 * @param DataType		데이터 테이블 중 목표로 하는 실제 데이터 타입 (내부적으로 정의된)
	 */
	template<typename TDataRow, typename TEnumType>
	TDataRow* GetDataRow(EDataTableType DataTableType, TEnumType DataType);

	FTableRowBase* GetItemData(EDataTableType DataTableType, int32 ItemID);

	/**
	 * DataTables Data Asset에 접근하는 함수.
	 */
	UFUNCTION(BlueprintCallable, Category = "Data Table")
	UDSDataTables* GetDataTables() const { return DataTables; }

	/**
	 * 특정 EDataTableType에 해당하는 UDataTable을 반환하는 함수.
	 */
	UFUNCTION(BlueprintCallable, Category = "Data Table")
	UDataTable* GetDataTable(EDataTableType TableType) const;
protected:
	// 비동기 로드해야 하는 Data Asset 번들 개수
	UPROPERTY()
	int PendingAssetLoadsCount = 1;

	// Data Asset Map
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Map")
	TMap<ECharacterType, UDSCharacterDataAsset*> CharacterDataMap;

	// Data Table
	UPROPERTY(EditDefaultsOnly, Category = "Data Table")
	TSoftObjectPtr<UDSDataTables> DataTablesAsset;
	UPROPERTY()
	UDSDataTables* DataTables;


	UPROPERTY()
	bool bIsAllAssetsLoaded = false;
};


template<typename TDataRow, typename TEnumType>
inline TDataRow* UDSGameDataSubsystem::GetDataRow(EDataTableType DataTableType, TEnumType DataType)
{
	UDataTable* DataTable = GetDataTable(DataTableType);

	if (IsValid(DataTable))
	{
		FString DataName = UEnum::GetValueAsString(DataType);
		FString Left, Right;
		DataName.Split(TEXT("::"), &Left, &Right);

		FName DataRightName = FName(Right);

		TDataRow* Row = DataTable->FindRow<TDataRow>(DataRightName, DataRightName.ToString());

		if (Row != nullptr)
		{
			return Row;
		}
	}

	return nullptr;
}
