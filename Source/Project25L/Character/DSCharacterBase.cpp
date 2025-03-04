//Default
#include "Character/DSCharacterBase.h"

//UE
#include "Components/CapsuleComponent.h"
#include "Engine/StreamableManager.h"
#include "GameFramework/CharacterMovementComponent.h"

//Game
#include "DSLogChannels.h"

#include "Skill/DSSkillControlComponent.h"
#include "Stat/DSStatComponent.h"
#include "System/DSEventSystems.h"

ADSCharacterBase::ADSCharacterBase(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	//Pawn 
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = true;

	GetCharacterMovement()->MaxWalkSpeed = 450.f;
	GetCharacterMovement()->JumpZVelocity = 0.f; //점프 할 수 없음.
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.0f, 0.f); //FORCEINLINE FRotator(T InPitch, T InYaw, T InRoll);
	GetCharacterMovement()->AirControl = 0.35f; //점프가 없기때문에 공중에 날 확률은 적지만, 있을 수도 있음.
	GetCharacterMovement()->bOrientRotationToMovement = false; //캐릭터나 액터가 이동할 때 그 방향으로 회전을 수행(마우스에 따라서 회전을 수행하도록 도와줌)
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f; //점차 멈추는 효과 
	GetCharacterMovement()->bWantsToCrouch = false;
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	//CapsuleComponent
	GetCapsuleComponent()->InitCapsuleSize(25.f, 90.0f);

	//SkeletalMesh
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -90.f), FRotator(0.0f, -90.f, 0.f));
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetCollisionProfileName(TEXT("NoCollision"));

	/*아래 코드 삭제 예정*/
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeletalMeshRef(TEXT("/Script/Engine.SkeletalMesh'/Game/Characters/Mannequins/Meshes/SKM_Manny.SKM_Manny'"));

	if (SkeletalMeshRef.Object)
	{
		GetMesh()->SetSkeletalMesh(SkeletalMeshRef.Object);
	}

	//Default AnimInstance
	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimInstanceRef(TEXT("/Game/Characters/Mannequins/Animations/ABP_Manny.ABP_Manny_C"));

	if (AnimInstanceRef.Class)
	{
		GetMesh()->SetAnimClass(AnimInstanceRef.Class);
	}
	/*데이터 테이블로 변환 예정*/

	SkillControlComponent = CreateDefaultSubobject<UDSSkillControlComponent>(TEXT("SkillControlComponent"));
	StatComponent = CreateDefaultSubobject<UDSStatComponent>(TEXT("StatComponent"));

}

void ADSCharacterBase::OnTest()
{
	DS_NETLOG(DSLog, Warning, TEXT("OnTest 함수 호출됨!"));
}

void ADSCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	DSEVENT_DELEGATE_BIND(GameEvent.OnDs_delegate, this, &ADSCharacterBase::OnTest);
	DSEVENT_DELEGATE_INVOKE(GameEvent.OnDs_delegate);
	DSEVENT_DELEGATE_REMOVE(GameEvent.OnDs_delegate, this);
}

void ADSCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	InitSkillActorInfo();
}

void ADSCharacterBase::OnRep_Controller()
{
	Super::OnRep_Controller();
	
	InitSkillActorInfo();
}

void ADSCharacterBase::InitSkillActorInfo()
{
	if (GetSkillControlComponent())
	{
		GetSkillControlComponent()->InitSkillActorInfo(this, this);
	}
}

void ADSCharacterBase::Initialize()
{
	TSoftObjectPtr<USkeletalMesh> SkeletalMesh; //실제론 데이터를 받도록 한다.
	
	FStreamableManager StreamableManager;
	StreamableManager.RequestAsyncLoad(SkeletalMesh.ToSoftObjectPath(), FStreamableDelegate::CreateLambda([WeakPtr = TWeakObjectPtr<ADSCharacterBase>(this), SkeletalMesh]()
		{
			
			if (SkeletalMesh.IsValid())
			{
				ADSCharacterBase* Character = WeakPtr.Get();

				if (IsValid(Character))
				{
					USkeletalMeshComponent* SkeletalMeshComponent = Character->GetMesh();

					if (IsValid(SkeletalMeshComponent))
					{
						SkeletalMeshComponent->SetSkeletalMesh(SkeletalMesh.Get());
					}
				}
			}
		}));
}

