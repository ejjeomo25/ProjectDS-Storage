// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpringArmComponent.h"
#include "DSSpringArmComponent.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT25L_API UDSSpringArmComponent : public USpringArmComponent
{
	GENERATED_BODY()
	
public:

protected:

	virtual void UpdateDesiredArmLocation(bool bDoTrace, bool bDoLocationLag, bool bDoRotationLag, float DeltaTime) override;
};
