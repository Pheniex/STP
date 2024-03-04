// Copyright Epic Games, Inc. All Rights Reserved.

#include "StanzzaTestProjectGameMode.h"
#include "StanzzaTestProjectCharacter.h"
#include "UObject/ConstructorHelpers.h"

AStanzzaTestProjectGameMode::AStanzzaTestProjectGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
