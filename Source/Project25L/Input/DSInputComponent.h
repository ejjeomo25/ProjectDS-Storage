#pragma once
//Default
#include "CoreMinimal.h"

//UE
#include "EnhancedInputComponent.h"

//Game
#include "DSInputConfig.h"

//UHT
#include "DSInputComponent.generated.h"

/** UDSInputComponent
 * 
 *	UDSInputConfig를 사용해서 Input을 맵핑하는 Component
 */
UCLASS()
class PROJECT25L_API UDSInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:
	UDSInputComponent(const FObjectInitializer& ObjectInitializer);

	// Press (NativeAction)
	template<class UserClass, typename FuncType>
	void BindSingleActions(const UDSInputConfig* InputConfig, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func);

	template<class UserClass, typename PressedFuncType, typename ReleasedFuncType>
	void BindDualActions(const UDSInputConfig* InputConfig, const FGameplayTag& InputTag, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc);

	template <class UserClass, typename PressedFuncType, typename HeldFuncType, typename ReleasedFuncType>
	void BindTripleActions(const UDSInputConfig* InputConfig, const FGameplayTag& InputTag, UserClass* Object, PressedFuncType PressedFunc, HeldFuncType HeldFunc, ReleasedFuncType ReleasedFunc);

	void RemoveBinds(const UInputAction* InputAction);

	TArray<FEnhancedInputActionEventBinding*>  FindActionEventBinding(const UInputAction* InputAction);
};


template <class UserClass, typename FuncType>
void UDSInputComponent::BindSingleActions(const UDSInputConfig* InputConfig, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func)
{
	check(InputConfig)
	if (const UInputAction* IA = InputConfig->FindNativeInputActionForTag(InputTag))
	{
		BindAction(IA, TriggerEvent, Object, Func);
	}
}

template <class UserClass, typename PressedFuncType, typename ReleasedFuncType>
void UDSInputComponent::BindDualActions(const UDSInputConfig* InputConfig, const FGameplayTag& InputTag, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc)
{
	check(InputConfig);

	if (const UInputAction* IA = InputConfig->FindDualInputActionsForTag(InputTag))
	{

		if (PressedFunc)
		{
			BindAction(IA, ETriggerEvent::Started, Object, PressedFunc);
		}
		if (ReleasedFunc)
		{
			BindAction(IA, ETriggerEvent::Completed, Object, ReleasedFunc);
		}

	}
}

template <class UserClass, typename PressedFuncType, typename HeldFuncType, typename ReleasedFuncType>
void UDSInputComponent::BindTripleActions(const UDSInputConfig* InputConfig, const FGameplayTag& InputTag, UserClass* Object, PressedFuncType PressedFunc, HeldFuncType HeldFunc, ReleasedFuncType ReleasedFunc)
{
	check(InputConfig);

	if (const UInputAction* IA = InputConfig->FindTripleInputActionsForTag(InputTag))
	{
		if (PressedFunc)
		{
			BindAction(IA, ETriggerEvent::Started, Object, PressedFunc);
		}
		if (HeldFunc)
		{
			BindAction(IA, ETriggerEvent::Ongoing, Object, HeldFunc);
		}
		if (ReleasedFunc)
		{
			BindAction(IA, ETriggerEvent::Completed, Object, ReleasedFunc);
		}
	}
}

