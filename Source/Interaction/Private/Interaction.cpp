// Copyright Epic Games, Inc. All Rights Reserved.

#include "Interaction.h"

#define LOCTEXT_NAMESPACE "FInteractionModule"

void FInteractionModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FInteractionModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE

DEFINE_LOG_CATEGORY(LogInteraction);
	
IMPLEMENT_MODULE(FInteractionModule, Interaction)