// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractableInterface.generated.h"

UINTERFACE(MinimalAPI)
class UInteractableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for Actors that can be interacted with through the GameplayAbilitySystem.
 */
class INTERACTION_API IInteractableInterface
{
	GENERATED_BODY()
};
