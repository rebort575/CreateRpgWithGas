// Copyright Rebort

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

/**
 * AuraGameplayTags
 *
 * Singleton containing native Gameplay Tags
 */

struct FAuraGameplayTags
{
public:
	static const FAuraGameplayTags &Get() { return GameplayTags; }
	static void InitializeNativeGameplayTags();


	FGameplayTag AttributesPrimaryStrength;
	FGameplayTag AttributesPrimaryIntelligence;
	FGameplayTag AttributesPrimaryResilience;
	FGameplayTag AttributesPrimaryVigor;


	FGameplayTag AttributesSecondaryArmor;
	FGameplayTag AttributesSecondaryArmorPenetration;
	FGameplayTag AttributesSecondaryBlockChance;
	FGameplayTag AttributesSecondaryCriticalHitChance;
	FGameplayTag AttributesSecondaryCriticalHitDamage;
	FGameplayTag AttributesSecondaryCriticalHitResistance;
	FGameplayTag AttributesSecondaryHealthRegeneration;
	FGameplayTag AttributesSecondaryManaRegeneration;
	FGameplayTag AttributesSecondaryMaxHealth;
	FGameplayTag AttributesSecondaryMaxMana;

	FGameplayTag AttributesResistanceFire;
	FGameplayTag AttributesResistanceLighting;
	FGameplayTag AttributesResistanceArcane;
	FGameplayTag AttributesResistancePhysical;

	FGameplayTag DebuffBurn;
	FGameplayTag DebuffStun;
	FGameplayTag DebuffArcane;
	FGameplayTag DebuffPhysical;

	FGameplayTag DebuffChance;
	FGameplayTag DebuffDamage;
	FGameplayTag DebuffDuration;
	FGameplayTag DebuffFrequency;

	FGameplayTag AttributeMetaIncomingXP;

	FGameplayTag InputTagLMB;
	FGameplayTag InputTagRMB;
	FGameplayTag InputTag1;
	FGameplayTag InputTag2;
	FGameplayTag InputTag3;
	FGameplayTag InputTag4;
	FGameplayTag InputTagPassive1;
	FGameplayTag InputTagPassive2;

	FGameplayTag Damage;
	FGameplayTag DamageFire;
	FGameplayTag DamageLighting;
	FGameplayTag DamageArcane;
	FGameplayTag DamagePhysical;

	FGameplayTag EffectHitReact;

	FGameplayTag AbilitiesAttack;
	FGameplayTag AbilitiesSummon;
	FGameplayTag AbilitiesNone;

	FGameplayTag AbilitiesHitReact;
	FGameplayTag AbilitiesStatusLocked;
	FGameplayTag AbilitiesStatusEligible;
	FGameplayTag AbilitiesStatusUnlocked;
	FGameplayTag AbilitiesStatusEquipped;

	FGameplayTag AbilitiesTypePassive;
	FGameplayTag AbilitiesTypeOffensive;
	FGameplayTag AbilitiesTypeNone;


	FGameplayTag AbilitiesFireBolt;
	FGameplayTag AbilitiesLightningElectrocute;

	FGameplayTag Cooldown_FireBolt;

	FGameplayTag CombatSocketWeapon;
	FGameplayTag CombatSocketRightHand;
	FGameplayTag CombatSocketLeftHand;
	FGameplayTag CombatSocketTail;

	FGameplayTag MontageAttack1;
	FGameplayTag MontageAttack2;
	FGameplayTag MontageAttack3;
	FGameplayTag MontageAttack4;

	FGameplayTag PlayerBlockInputPressed;
	FGameplayTag PlayerBlockInputHeld;
	FGameplayTag PlayerBlockInputReleased;
	FGameplayTag PlayerBlockCursorTrace;

	TMap<FGameplayTag, FGameplayTag> DamageTypesToResistances;
	TMap<FGameplayTag, FGameplayTag> DamageTypeToDebuffs;

private:
	static FAuraGameplayTags GameplayTags;
};
