// Default
#include "Gimmick/SpawnerVolume/DSMonsterSpawnerVolume.h"

void ADSMonsterSpawnerVolume::BeginPlay()
{
	Super::BeginPlay(); 

	if (HasAuthority())
	{
		SpawnMonster(MonsterType);
	}
}
