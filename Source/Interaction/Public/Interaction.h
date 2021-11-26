// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogInteraction, Log, All);

#define COLLISION_INTERACTABLE					ECollisionChannel::ECC_GameTraceChannel1
#define COLLISION_PICKUP						ECollisionChannel::ECC_GameTraceChannel2
#define COLLISION_ABILITY						ECollisionChannel::ECC_GameTraceChannel3
#define COLLISION_PROJECTILE					ECollisionChannel::ECC_GameTraceChannel4
#define COLLISION_ABILITYOVERLAPPROJECTILE		ECollisionChannel::ECC_GameTraceChannel5

class FInteractionModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
