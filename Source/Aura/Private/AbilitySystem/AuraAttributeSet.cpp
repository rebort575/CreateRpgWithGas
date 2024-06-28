// Copyright Rebort


#include "AbilitySystem/AuraAttributeSet.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilityTypes.h"
#include "AuraGameplayTags.h"
#include "GameplayEffectExtension.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Aura/AuraLogChannels.h"
#include "GameFramework/Character.h"
#include "GameplayEffectComponents/TargetTagsGameplayEffectComponent.h"
#include "Interaction/CombatInterface.h"
#include "Interaction/PlayerInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Player/AuraPlayerController.h"

UAuraAttributeSet::UAuraAttributeSet()
{
	const FAuraGameplayTags &GameplayTags = FAuraGameplayTags::Get();

	/** Primary Attributes */
	TagsToAttributes.Add(GameplayTags.AttributesPrimaryStrength, GetStrengthAttribute);
	TagsToAttributes.Add(GameplayTags.AttributesPrimaryIntelligence, GetIntelligenceAttribute);
	TagsToAttributes.Add(GameplayTags.AttributesPrimaryResilience, GetResilienceAttribute);
	TagsToAttributes.Add(GameplayTags.AttributesPrimaryVigor, GetVigorAttribute);

	/** Secondary Attributes */
	TagsToAttributes.Add(GameplayTags.AttributesSecondaryArmor, GetArmorAttribute);
	TagsToAttributes.Add(GameplayTags.AttributesSecondaryArmorPenetration, GetArmorPenetrationAttribute);
	TagsToAttributes.Add(GameplayTags.AttributesSecondaryBlockChance, GetBlockChanceAttribute);
	TagsToAttributes.Add(GameplayTags.AttributesSecondaryCriticalHitChance, GetCriticalHitChanceAttribute);
	TagsToAttributes.Add(GameplayTags.AttributesSecondaryCriticalHitDamage, GetCriticalHitDamageAttribute);
	TagsToAttributes.Add(GameplayTags.AttributesSecondaryCriticalHitResistance, GetCriticalHitResistanceAttribute);
	TagsToAttributes.Add(GameplayTags.AttributesSecondaryHealthRegeneration, GetHealthRegenerationAttribute);
	TagsToAttributes.Add(GameplayTags.AttributesSecondaryManaRegeneration, GetManaRegenerationAttribute);
	TagsToAttributes.Add(GameplayTags.AttributesSecondaryMaxHealth, GetMaxHealthAttribute);
	TagsToAttributes.Add(GameplayTags.AttributesSecondaryMaxMana, GetMaxManaAttribute);

	// Resistance Attributes
	TagsToAttributes.Add(GameplayTags.AttributesResistanceFire, GetFireResistanceAttribute);
	TagsToAttributes.Add(GameplayTags.AttributesResistancePhysical, GetPhysicalResistanceAttribute);
	TagsToAttributes.Add(GameplayTags.AttributesResistanceArcane, GetArcaneResistanceAttribute);
	TagsToAttributes.Add(GameplayTags.AttributesResistanceLighting, GetLightingResistanceAttribute);
}

void UAuraAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// vital attributes
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Mana, COND_None, REPNOTIFY_Always);

	// primary attributes
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Strength, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Intelligence, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Resilience, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Vigor, COND_None, REPNOTIFY_Always);

	// secondary attributes
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, ArmorPenetration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, BlockChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CriticalHitChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CriticalHitDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CriticalHitResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, HealthRegeneration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, ManaRegeneration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);

	// Resistance Attributes
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, ArcaneResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, PhysicalResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, LightingResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, FireResistance, COND_None, REPNOTIFY_Always);
}

void UAuraAttributeSet::PreAttributeChange(const FGameplayAttribute &Attribute, float &NewValue)
{
	// This function is meant to enforce things like "Health = Clamp(Health, 0, MaxHealth)" and NOT things like "trigger this extra thing if damage is applied, etc".
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
	if (Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMana());
	}
}

void UAuraAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData &Data)
{
	Super::PostGameplayEffectExecute(Data);

	FEffectProperties Props;
	SetEffectProperties(Data, Props);

	if (Props.TargetCharacter->Implements<UCombatInterface>() && ICombatInterface::Execute_IsDead(
		Props.TargetCharacter))
	{
		return;
	}

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
		UE_LOG(LogTemp, Warning, TEXT("Change Health on %s, Health: %f"), *Props.TargetAvatarActor->GetName(),
		       GetHealth());
	}
	if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		SetMana(FMath::Clamp(GetMana(), 0.f, GetMaxMana()));
	}
	if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		HandleIncomingDamage(Props);
	}
	if (Data.EvaluatedData.Attribute == GetIncomingXPAttribute())
	{
		HandleIncomingXP(Props);
	}
}

void UAuraAttributeSet::PostAttributeChange(const FGameplayAttribute &Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
	if (Attribute == GetMaxHealthAttribute() && bTopOffHealth)
	{
		SetHealth(GetMaxHealth());
		bTopOffHealth = false;
		UE_LOG(LogAura, Error, TEXT("In Post MaxHealth:%.2f, MaxMana:%.2f"), GetMaxHealth(), GetMaxMana());
	}

	if (Attribute == GetMaxManaAttribute() && bTopOffMana)
	{
		SetMana(GetMaxMana());
		bTopOffMana = false;
		UE_LOG(LogAura, Error, TEXT("In Post MaxHealth:%.2f, MaxMana:%.2f"), GetMaxHealth(), GetMaxMana());
	}
}

void UAuraAttributeSet::SetEffectProperties(const FGameplayEffectModCallbackData &Data, FEffectProperties &Props)
{
	Props.EffectContextHandle = Data.EffectSpec.GetContext();
	Props.SourceASC = Props.EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();

	if (IsValid(Props.SourceASC) && Props.SourceASC->AbilityActorInfo.IsValid() && Props.SourceASC->AbilityActorInfo->
		AvatarActor.IsValid())
	{
		Props.SourceAvatarActor = Props.SourceASC->AbilityActorInfo->AvatarActor.Get();
		Props.SourceController = Props.SourceASC->AbilityActorInfo->PlayerController.Get();
		if (Props.SourceController == nullptr && Props.SourceAvatarActor != nullptr)
		{
			if (const APawn *Pawn = Cast<APawn>(Props.SourceAvatarActor))
			{
				Props.SourceController = Pawn->GetController();
			}
		}
		if (Props.SourceController != nullptr)
		{
			Props.SourceCharacter = Cast<ACharacter>(Props.SourceController->GetPawn());
		}
	}

	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		Props.TargetAvatarActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		Props.TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		Props.TargetCharacter = Cast<ACharacter>(Props.TargetAvatarActor);
		Props.TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Props.TargetAvatarActor);
	}
}

void UAuraAttributeSet::ShowFloatingText(const FEffectProperties &Props, float Damage, bool bBlockedHit,
                                         bool bCriticalHit)
{
	UE_LOG(LogTemp, Warning, TEXT("IsBlockedHit :%d, IsCriticalHit :%d"), bBlockedHit, bCriticalHit);

	if (Props.SourceCharacter != Props.TargetCharacter)
	{
		if (AAuraPlayerController *PC = Cast<AAuraPlayerController>(Props.SourceCharacter->Controller))
		{
			PC->ShowDamageNumber(Damage, bBlockedHit, bCriticalHit, Props.TargetCharacter);
		}
		else if (PC = Cast<AAuraPlayerController>(Props.TargetCharacter->Controller), PC)
		{
			PC->ShowDamageNumber(Damage, bBlockedHit, bCriticalHit, Props.TargetCharacter);
		}
	}
}

void UAuraAttributeSet::SendXPEvent(const FEffectProperties &Props)
{
	if (Props.TargetCharacter->Implements<UCombatInterface>())
	{
		const int32 TargetLevel = ICombatInterface::Execute_GetPlayerLevel(Props.TargetCharacter);
		const ECharacterClass CharacterClass = ICombatInterface::Execute_GetCharacterClass(Props.TargetCharacter);
		const int32 XPReward = UAuraAbilitySystemLibrary::GetXPRewardForClassAndLevel(
			Props.TargetCharacter, CharacterClass, TargetLevel);

		const FAuraGameplayTags &GameplayTags = FAuraGameplayTags::Get();
		FGameplayEventData Payload;
		Payload.EventTag = GameplayTags.AttributeMetaIncomingXP;
		Payload.EventMagnitude = XPReward;
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Props.SourceCharacter,
		                                                         GameplayTags.AttributeMetaIncomingXP, Payload);
	}
}

void UAuraAttributeSet::OnRep_Health(const FGameplayAttributeData &OldHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Health, OldHealth);
}

void UAuraAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData &OldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxHealth, OldMaxHealth);
}

void UAuraAttributeSet::OnRep_Mana(const FGameplayAttributeData &OldMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Mana, OldMana);
}

void UAuraAttributeSet::OnRep_MaxMana(const FGameplayAttributeData &OldMaxMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxMana, OldMaxMana);
}

void UAuraAttributeSet::OnRep_Strength(const FGameplayAttributeData &OldStrength) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Strength, OldStrength);
}

void UAuraAttributeSet::OnRep_Intelligence(const FGameplayAttributeData &OldIntelligence) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Intelligence, OldIntelligence);
}

void UAuraAttributeSet::OnRep_Resilience(const FGameplayAttributeData &OldResilience) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Resilience, OldResilience);
}

void UAuraAttributeSet::OnRep_Vigor(const FGameplayAttributeData &OldVigor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Vigor, OldVigor);
}

void UAuraAttributeSet::OnRep_Armor(const FGameplayAttributeData &OldArmor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Armor, OldArmor);
}

void UAuraAttributeSet::OnRep_ArmorPenetration(const FGameplayAttributeData &OldArmorPenetration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, ArmorPenetration, OldArmorPenetration);
}

void UAuraAttributeSet::OnRep_BlockChance(const FGameplayAttributeData &OldBlockChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, BlockChance, OldBlockChance);
}

void UAuraAttributeSet::OnRep_CriticalHitChance(const FGameplayAttributeData &OldCriticalHitChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, CriticalHitChance, OldCriticalHitChance);
}

void UAuraAttributeSet::OnRep_CriticalHitDamage(const FGameplayAttributeData &OldCriticalHitDamage) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, CriticalHitDamage, OldCriticalHitDamage);
}

void UAuraAttributeSet::OnRep_CriticalHitResistance(const FGameplayAttributeData &OldCriticalHitResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, CriticalHitResistance, OldCriticalHitResistance);
}

void UAuraAttributeSet::OnRep_HealthRegeneration(const FGameplayAttributeData &OldHealthRegeneration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, HealthRegeneration, OldHealthRegeneration);
}

void UAuraAttributeSet::OnRep_ManaRegeneration(const FGameplayAttributeData &OldManaRegeneration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, ManaRegeneration, OldManaRegeneration);
}

void UAuraAttributeSet::OnRep_FireResistance(const FGameplayAttributeData &OldFireResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, FireResistance, OldFireResistance);
}

void UAuraAttributeSet::OnRep_LightingResistance(const FGameplayAttributeData &OldLightingResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, LightingResistance, OldLightingResistance);
}

void UAuraAttributeSet::OnRep_ArcaneResistance(const FGameplayAttributeData &OldArcaneResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, ArcaneResistance, OldArcaneResistance);
}

void UAuraAttributeSet::OnRep_PhysicalResistance(const FGameplayAttributeData &OldPhysicalResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, PhysicalResistance, OldPhysicalResistance);
}

void UAuraAttributeSet::HandleIncomingDamage(const FEffectProperties &Props)
{
	const float LocalIncomingDamage = GetIncomingDamage();
	SetIncomingDamage(0.f);
	if (LocalIncomingDamage > 0.f)
	{
		const float NewHealth = GetHealth() - LocalIncomingDamage;
		SetHealth(FMath::Clamp(NewHealth, 0.f, GetMaxHealth()));

		const bool bFatal = NewHealth <= 0.f;
		if (bFatal)
		{
			if (ICombatInterface *CombatInterface = Cast<ICombatInterface>(Props.TargetAvatarActor))
			{
				CombatInterface->Die(UAuraAbilitySystemLibrary::GetDeathImpulse(Props.EffectContextHandle));
			}
			SendXPEvent(Props);
		}
		else
		{
			if (Props.TargetCharacter->Implements<UCombatInterface>() && !ICombatInterface::Execute_IsInBeingShockLoop(
				Props.TargetCharacter))
			{
				FGameplayTagContainer TagContainer;
				TagContainer.AddTag(FAuraGameplayTags::Get().EffectHitReact);
				Props.TargetASC->TryActivateAbilitiesByTag(TagContainer);
			}


			const FVector KnockbackForce = UAuraAbilitySystemLibrary::GetKnockbackForce(Props.EffectContextHandle);
			if (!KnockbackForce.IsNearlyZero())
			{
				Props.TargetCharacter->LaunchCharacter(KnockbackForce, true, true);
			}
		}

		const bool bBlocked = UAuraAbilitySystemLibrary::IsBlockedHit(Props.EffectContextHandle);
		const bool bCriticalHit = UAuraAbilitySystemLibrary::IsCriticalHit(Props.EffectContextHandle);
		ShowFloatingText(Props, LocalIncomingDamage, bBlocked, bCriticalHit);

		if (UAuraAbilitySystemLibrary::IsSuccessfulDebuff(Props.EffectContextHandle))
		{
			Debuff(Props);
		}
	}
}

void UAuraAttributeSet::HandleIncomingXP(const FEffectProperties &Props)
{
	const float LocalIncomingXP = GetIncomingXP();
	UE_LOG(LogAura, Log, TEXT("IncomingXP :%f"), LocalIncomingXP);
	SetIncomingXP(0.f);
	if (Props.SourceCharacter->Implements<UPlayerInterface>() && Props.SourceCharacter->Implements<
		UCombatInterface>())
	{
		const int32 CurrentLevel = ICombatInterface::Execute_GetPlayerLevel(Props.SourceCharacter);
		const int32 CurrentXP = IPlayerInterface::Execute_GetXP(Props.SourceCharacter);
		const int32 NewLevel = IPlayerInterface::Execute_FindLevelForXP(
			Props.SourceCharacter, LocalIncomingXP + CurrentXP);

		const int32 NumLevelUps = NewLevel - CurrentLevel;
		if (NumLevelUps > 0)
		{
			IPlayerInterface::Execute_AddToPlayerLevel(Props.SourceCharacter, NumLevelUps);
			int32 AttributePointsReward = 0;
			int32 SpellPointsReward = 0;
			for (int i = 0; i < NumLevelUps; ++i)
			{
				AttributePointsReward += IPlayerInterface::Execute_GetAttributePointReward(
					Props.SourceCharacter, CurrentLevel + i);
				SpellPointsReward += IPlayerInterface::Execute_GetSpellPointReward(
					Props.SourceCharacter, CurrentLevel + i);
			}
			IPlayerInterface::Execute_AddToAttributePoints(Props.SourceCharacter, AttributePointsReward);
			IPlayerInterface::Execute_AddToSpellPoints(Props.SourceCharacter, SpellPointsReward);
			IPlayerInterface::Execute_LevelUp(Props.SourceCharacter);

			SetHealth(GetMaxHealth());
			SetMana(GetMaxMana());
			UE_LOG(LogAura, Error, TEXT("In Pre MaxHealth:%.2f, MaxMana:%.2f"), GetMaxHealth(), GetMaxMana());

			bTopOffHealth = true;
			bTopOffMana = true;
		}
		IPlayerInterface::Execute_AddToXP(Props.SourceCharacter, LocalIncomingXP);
	}
}

#pragma optimize("" ,off)
void UAuraAttributeSet::Debuff(const FEffectProperties &Props)
{
	const FAuraGameplayTags &GameplayTags = FAuraGameplayTags::Get();
	FGameplayEffectContextHandle EffectContext = Props.SourceASC->MakeEffectContext();
	EffectContext.AddSourceObject(Props.SourceAvatarActor);

	const FGameplayTag DamageType = UAuraAbilitySystemLibrary::GetDamageType(Props.EffectContextHandle);
	const float DebuffDamage = UAuraAbilitySystemLibrary::GetDebuffDamage(Props.EffectContextHandle);
	const float DebuffDuration = UAuraAbilitySystemLibrary::GetDebuffDuration(Props.EffectContextHandle);
	const float DebuffFrequency = UAuraAbilitySystemLibrary::GetDebuffFrequency(Props.EffectContextHandle);

	FString DebuffName = FString::Printf(TEXT("DynamicDebuff_%s"), *DamageType.ToString());
	UGameplayEffect *Effect = NewObject<UGameplayEffect>(GetTransientPackage(), FName(DebuffName));
	Effect->DurationPolicy = EGameplayEffectDurationType::HasDuration;
	Effect->Period = DebuffFrequency;
	Effect->DurationMagnitude = FScalableFloat(DebuffDuration);
	// Effect->InheritableOwnedTagsContainer.AddTag(GameplayTags.DamageTypeToDebuffs[DamageType]);

	const FGameplayTag &DebuffTag = GameplayTags.DamageTypeToDebuffs[DamageType];
	FInheritedTagContainer TagContainer = FInheritedTagContainer();
	UTargetTagsGameplayEffectComponent &Component = Effect->FindOrAddComponent<UTargetTagsGameplayEffectComponent>();
	TagContainer.Added.AddTag(DebuffTag);
	// TagContainer.CombinedTags.AddTag(DebuffTag);
	if (DebuffTag.MatchesTagExact(GameplayTags.DebuffStun))
	{
		TagContainer.Added.AddTag(GameplayTags.PlayerBlockCursorTrace);
		// TagContainer.CombinedTags.AddTag(GameplayTags.PlayerBlockCursorTrace);

		TagContainer.Added.AddTag(GameplayTags.PlayerBlockInputHeld);
		// TagContainer.CombinedTags.AddTag(GameplayTags.PlayerBlockInputHeld);

		TagContainer.Added.AddTag(GameplayTags.PlayerBlockInputPressed);
		// TagContainer.CombinedTags.AddTag(GameplayTags.PlayerBlockInputPressed);

		TagContainer.Added.AddTag(GameplayTags.PlayerBlockInputReleased);
		// TagContainer.CombinedTags.AddTag(GameplayTags.PlayerBlockInputReleased);
	}
	Component.SetAndApplyTargetTagChanges(TagContainer);

	Effect->StackingType = EGameplayEffectStackingType::AggregateBySource;
	Effect->StackLimitCount = 1;

	int32 Index = Effect->Modifiers.Num();
	Effect->Modifiers.Add(FGameplayModifierInfo());
	FGameplayModifierInfo &ModifierInfo = Effect->Modifiers[Index];
	ModifierInfo.ModifierMagnitude = FScalableFloat(DebuffDamage);
	ModifierInfo.ModifierOp = EGameplayModOp::Additive;
	ModifierInfo.Attribute = UAuraAttributeSet::GetIncomingDamageAttribute();

	FGameplayEffectSpec *MutableSpec = new FGameplayEffectSpec(Effect, EffectContext, 1.f);
	if (MutableSpec)
	{
		FAuraGameplayEffectContext *AuraContext = static_cast<FAuraGameplayEffectContext *>(MutableSpec->GetContext().
			Get());
		TSharedPtr<FGameplayTag> DebuffDamageType = MakeShareable(new FGameplayTag(DamageType));
		AuraContext->SetDamageType(DebuffDamageType);
	}
	Props.TargetASC->ApplyGameplayEffectSpecToSelf(*MutableSpec);
}
#pragma optimize("" ,on)
