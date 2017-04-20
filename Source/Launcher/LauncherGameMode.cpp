// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "Launcher.h"
#include "LauncherGameMode.h"
#include "LauncherHUD.h"
#include "LauncherCharacter.h"

ALauncherGameMode::ALauncherGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = ALauncherHUD::StaticClass();
}
