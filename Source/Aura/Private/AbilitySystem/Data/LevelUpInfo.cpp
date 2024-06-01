// Copyright Rebort


#include "AbilitySystem/Data/LevelUpInfo.h"

int32 ULevelUpInfo::FindLevelForXP(int32 XP) const
{
	int32 Level = 0;
	for (const FAuraLevelUpInfo& Info : LevelUpInfos)
	{
		if (XP >= Info.LevelUpRequirement)
		{
			++Level;
		}
		else
		{
			break;
		}
	}
	return Level;
}
