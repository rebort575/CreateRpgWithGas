// Copyright Rebort


#include "AbilitySystem/Abilities/AuraFireBolt.h"

#include "AuraGameplayTags.h"

FString UAuraFireBolt::GetDescription(int32 Level)
{
	const int32 Damage = GetDamageByDamageType(Level, FAuraGameplayTags::Get().DamageFire);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	if (Level == 1)
	{
		return FString::Printf(TEXT(
			// Title
			"<Title>FIRE BOLT</>\n\n"

			// Details
			"<Small>Level: </><Level>%d</>\n"
			"<Small>ManaCost: </><ManaCost>%.1f</>\n"
			"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"

			// Description
			"<Default>Launched a bolt of fire, exploding on impact and dealing: </>"
			"<Damage>%d</>"
			"<Default> fire damage with a chance to burn</>\n\n"
			), Level, ManaCost, Cooldown, Damage);
	}
	else
	{
		return FString::Printf(TEXT(
			// Title
			"<Title>FIRE BOLT</>\n\n"

			// Details
			"<Small>Level: </><Level>%d</>\n"
			"<Small>ManaCost: </><ManaCost>%.1f</>\n"
			"<Small>Cooldown: </><Cooldown>%.1f</>\n"

			// Description
			"<Default>Launched %d bolts of fire, exploding on impact and dealing: </>"
			"<Damage>%d</>"
			"<Default> fire damage with a chance to burn</>\n\n"
			), Level, ManaCost, Cooldown, FMath::Min(Level, NumProjectiles), Damage);
	}
	return FString();
}

FString UAuraFireBolt::GetNextLevelDescription(int32 Level)
{
	const int32 Damage = GetDamageByDamageType(Level, FAuraGameplayTags::Get().DamageFire);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	return FString::Printf(TEXT(
		// Title
		"<Title>NEXT LEVEL: </>\n\n"

		// Details
		"<Small>Level: </><Level>%d</>\n"
		"<Small>ManaCost: </><ManaCost>%.1f</>\n"
		"<Small>Cooldown: </><Cooldown>%.1f</>\n"

		// Description
		"<Default>Launched %d bolts of fire, exploding on impact and dealing: </>"
		"<Damage>%d</>"
		"<Default> fire damage with a chance to burn</>\n\n"
		), Level, ManaCost, Cooldown, FMath::Min(Level, NumProjectiles), Damage);
}
