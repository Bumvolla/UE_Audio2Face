// Copyright Epic Games, Inc. All Rights Reserved.

#include "UE_A2F.h"

#define LOCTEXT_NAMESPACE "FUE_A2FModule"

void FUE_A2FModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FUE_A2FModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FUE_A2FModule, UE_A2F)