// Default
#include "Animation/AnimNotify/AnimNotifyState_EquipWeapon.h"

// Game
#include "Character/DSArmedCharacter.h"
#include "DSLogChannels.h"

void UAnimNotifyState_EquipWeapon::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	//*****************코드 리뷰 : MeshComp 널체크  ****************************//

	ADSArmedCharacter* Character = Cast<ADSArmedCharacter>(MeshComp->GetOwner());

	if (IsValid(Character))
	{
		Character->MoveEquip();
	}
}
