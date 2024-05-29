// Copyright Rebort


#include "AuraGameplayTags.h"
#include "GameplayTagsManager.h"

FAuraGameplayTags FAuraGameplayTags::GameplayTags;

void FAuraGameplayTags::InitializeNativeGameplayTags()
{
	/**
	 * Primary Attributes
	 */
	GameplayTags.AttributesPrimaryStrength = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Primary.Strength"),
		FString("Increases physical damage")
	);

	GameplayTags.AttributesPrimaryIntelligence = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Primary.Intelligence"),
		FString("Increase magic damage")
	);

	GameplayTags.AttributesPrimaryResilience = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Primary.Resilience"),
		FString("Increase Armor and Armor Penetration")
	);

	GameplayTags.AttributesPrimaryVigor = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Primary.Vigor"),
		FString("Increase Health")
	);
	/**
	 * Secondary Attributes
	 */
	GameplayTags.AttributesSecondaryArmor = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.Armor"),
		FString("Reduce damage taken, improves Block Chance")
	);

	GameplayTags.AttributesSecondaryArmorPenetration = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.ArmorPenetration"),
		FString("Ignore Percentage of enemy Armor, increases Critical Hit Chance")
	);

	GameplayTags.AttributesSecondaryBlockChance = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.BlockChance"),
		FString("Chance to cut incoming damage in half")
	);

	GameplayTags.AttributesSecondaryCriticalHitChance = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.CriticalHitChance"),
		FString("Change to double damage plus critical hit bonus")
	);

	GameplayTags.AttributesSecondaryCriticalHitDamage = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.CriticalHitDamage"),
		FString("Bonus damage added when a critical hit is scored")
	);

	GameplayTags.AttributesSecondaryCriticalHitResistance = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.CriticalHitResistance"),
		FString("Reduces Critical Hit Chance of attacking enemies")
	);

	GameplayTags.AttributesSecondaryHealthRegeneration = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.HealthRegeneration"),
		FString("Amount of Health regenerated every 1 second")
	);

	GameplayTags.AttributesSecondaryManaRegeneration = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.ManaRegeneration"),
		FString("Amount of Mana regenerated every 1 second")
	);

	GameplayTags.AttributesSecondaryMaxHealth = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.MaxHealth"),
		FString("Maximum amount of Health obtainable")
	);

	GameplayTags.AttributesSecondaryMaxMana = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.MaxMana"),
		FString("Maximum amount of Mana obtainable")
	);

	/**
	 * Input Tags
	 */
	GameplayTags.InputTagLMB = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.LMB"),
		FString("Input Tag for Left Mouse Button")
	);

	GameplayTags.InputTagRMB = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.RMB"),
		FString("Input Tag for Right Mouse Button")
	);

	GameplayTags.InputTag1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.1"),
		FString("Input Tag for 1 key")
	);

	GameplayTags.InputTag2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.2"),
		FString("Input Tag for 2 key")
	);

	GameplayTags.InputTag3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.3"),
		FString("Input Tag for 3 key")
	);

	GameplayTags.InputTag4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.4"),
		FString("Input Tag for 4 key")
	);

	GameplayTags.Damage = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage"),
		FString("Damage")
	);

	GameplayTags.EffectHitReact = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Effects.HitReact"),
		FString("Tag granted when Hit Reacting")
	);


	/**
	 * Damage Types
	 */

	GameplayTags.DamageFire = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Fire"),
		FString("Fire Damage Type")
	);

	GameplayTags.DamageLighting = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Lighting"),
		FString("Lighting Damage Type")
	);

	GameplayTags.DamageArcane = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Arcane"),
		FString("Arcane Damage Type")
	);

	GameplayTags.DamagePhysical = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Physical"),
		FString("Physical Damage Type")
	);


	/**
	 * Resistances
	 */
	GameplayTags.AttributesResistanceArcane = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Resistance.Arcane"),
		FString("Resistance to Arcane damage")
	);

	GameplayTags.AttributesResistanceLighting = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Resistance.Lighting"),
		FString("Resistance to Lighting damage")
	);

	GameplayTags.AttributesResistanceFire = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Resistance.Fire"),
		FString("Resistance to Fire damage")
	);

	GameplayTags.AttributesResistancePhysical = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Resistance.Physical"),
		FString("Resistance to Physical damage")
	);

	/**
	 * Map of Damage Types to Resistances
	 */
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.DamageFire, GameplayTags.AttributesResistanceFire);
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.DamageLighting, GameplayTags.AttributesResistanceLighting);
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.DamageArcane, GameplayTags.AttributesResistanceArcane);
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.DamagePhysical, GameplayTags.AttributesResistancePhysical);

	/**
	 * Abilities
	 */
	GameplayTags.AbilitiesAttack = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.Attack"),
		FString("Attack Ability Tag")
	);
	GameplayTags.AbilitiesSummon = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.Summon"),
		FString("Summon Ability Tag")
	);
	GameplayTags.AbilitiesFireBolt= UGameplayTagsManager::Get().AddNativeGameplayTag(
    		FName("Abilities.Fire.FireBolt"),
    		FString("FireBolt Ability Tag")
    	);


	/**
	 * Cooldown
	 */
	GameplayTags.Cooldown_FireBolt= UGameplayTagsManager::Get().AddNativeGameplayTag(
    		FName("Cooldown.Fire.FireBolt"),
    		FString("FireBolt Cooldown Tag")
    	);

	/**
	 * Combat socket
	 */
	GameplayTags.CombatSocketWeapon = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Montage.Attack.Weapon"),
		FString("Weapon")
	);
	GameplayTags.CombatSocketLeftHand = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Montage.Attack.LeftHand"),
		FString("Left Hand")
	);
	GameplayTags.CombatSocketRightHand = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Montage.Attack.RightHand"),
		FString("Right Hand")
	);
	GameplayTags.CombatSocketTail = UGameplayTagsManager::Get().AddNativeGameplayTag(
    		FName("Montage.Attack.Tail"),
    		FString("Tail")
    	);

	/**
	 * Montage Tags
	 */
	GameplayTags.MontageAttack1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Montage.Attack.1"),
		FString("Attack 1")
	);

	GameplayTags.MontageAttack2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
 		FName("Montage.Attack.2"),
 		FString("Attack 2")
 	);
	
 	GameplayTags.MontageAttack3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
 		FName("Montage.Attack.3"),
 		FString("Attack 3")
 	);
	
	GameplayTags.MontageAttack4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Montage.Attack.4"),
		FString("Attack 4")
	);
}
