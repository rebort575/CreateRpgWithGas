// Copyright Rebort


#include "UI/WidgetController/SpellMenuWidgetController.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Player/AuraPlayerState.h"

void USpellMenuWidgetController::BroadcastInitialValues()
{
	BroadcastAbilityInfo();
	SpellPointsChangedDelegate.Broadcast(GetAuraPS()->GetSpellPoints());
}

void USpellMenuWidgetController::BindCallbacksToDependencies()
{
	GetAuraASC()->AbilityStatusChanged.AddLambda(
		[this](const FGameplayTag &AbilityTag, const FGameplayTag &StatusTag, int32 AbilityLevel)
		{
			if (AbilityInfo)
			{
				if (AbilityTag.MatchesTagExact(SelectedAbility.Ability))
				{
					SelectedAbility.Status = StatusTag;
					TryEnableButtons();
				}
				FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
				Info.StatusTag = StatusTag;
				AbilityInfoDelegate.Broadcast(Info);
			}
		});
	GetAuraPS()->OnSpellPointsChangedDelegate.AddLambda(
		[this](int32 Points)
		{
			CurrentSpellPoints = Points;
			TryEnableButtons();

			SpellPointsChangedDelegate.Broadcast(Points);
		});
	GetAuraASC()->AbilityEquipped.AddUObject(this, &USpellMenuWidgetController::OnAbilityEquipped);
}

#pragma optimize("", off)
void USpellMenuWidgetController::SpellGlobeSelected(const FGameplayTag &AbilityTag)
{
	if (bWaitingForEquipSelection)
	{
		const FGameplayTag SelectedAbilityType = AbilityInfo->FindAbilityInfoForTag(AbilityTag).AbilityType;
		StopWaitForEquipDelegate.Broadcast(SelectedAbilityType);
		bWaitingForEquipSelection = false;
	}

	const int32 SpellPoints = GetAuraPS()->GetSpellPoints();
	const FAuraGameplayTags &GameplayTags = FAuraGameplayTags::Get();
	FGameplayTag AbilityStatus;

	const bool bTagValid = AbilityTag.IsValid();
	const bool bTagNone = AbilityTag.MatchesTag(GameplayTags.AbilitiesNone);
	const FGameplayAbilitySpec *AbilitySpec = GetAuraASC()->GetSpecFromAbilityTag(AbilityTag);
	if (!bTagValid || bTagNone || AbilitySpec == nullptr)
	{
		AbilityStatus = GameplayTags.AbilitiesStatusLocked;
	}
	else
	{
		AbilityStatus = GetAuraASC()->GetStatusFromSpec(*AbilitySpec);
	}

	SelectedAbility.Ability = AbilityTag;
	SelectedAbility.Status = AbilityStatus;

	bool bEnableSpendPoints, bEnableEquip;
	ShouldEnableButtons(AbilityStatus, SpellPoints, bEnableSpendPoints, bEnableEquip);

	FString Description, NextLevelDescription;
	GetAuraASC()->GetDescriptionsByAbilityTag(AbilityTag, Description, NextLevelDescription);
	SpellGlobeSelectedDelegate.Broadcast(bEnableSpendPoints, bEnableEquip, Description, NextLevelDescription);
}

void USpellMenuWidgetController::GlobeDeselected()
{
	const FGameplayTag SelectedAbilityType = AbilityInfo->FindAbilityInfoForTag(SelectedAbility.Ability).AbilityType;
	StopWaitForEquipDelegate.Broadcast(SelectedAbilityType);
	bWaitingForEquipSelection = false;

	SelectedAbility.Ability = FAuraGameplayTags::Get().AbilitiesNone;
	SelectedAbility.Status = FAuraGameplayTags::Get().AbilitiesStatusLocked;
	SpellGlobeSelectedDelegate.Broadcast(false, false, FString(), FString());
}

void USpellMenuWidgetController::TryEnableButtons()
{
	if (SelectedAbility.Ability == FAuraGameplayTags::Get().AbilitiesNone)
	{
		return;
	}
	bool bEnableSpendPoints, bEnableEquip;
	ShouldEnableButtons(SelectedAbility.Status, CurrentSpellPoints, bEnableSpendPoints, bEnableEquip);

	FString Description, NextLevelDescription;
	GetAuraASC()->GetDescriptionsByAbilityTag(SelectedAbility.Ability, Description, NextLevelDescription);
	SpellGlobeSelectedDelegate.Broadcast(bEnableSpendPoints, bEnableEquip, Description, NextLevelDescription);
}

void USpellMenuWidgetController::SpendPointButtonPressed()
{
	if (GetAuraASC())
	{
		GetAuraASC()->ServerSpendSpellPoint(SelectedAbility.Ability);
	}
}

void USpellMenuWidgetController::EquipButtonPressed()
{
	const FGameplayTag &AbilityType = AbilityInfo->FindAbilityInfoForTag(SelectedAbility.Ability).AbilityType;
	WaitForEquipSelectionDelegate.Broadcast(AbilityType);
	bWaitingForEquipSelection = true;

	const FGameplayTag SelectedStatus = GetAuraASC()->GetStatusFromAbilityTag(SelectedAbility.Ability);
	if (SelectedStatus.MatchesTagExact(FAuraGameplayTags::Get().AbilitiesStatusEquipped))
	{
		SelectedSlot = GetAuraASC()->GetInputTagFromAbilityTag(SelectedAbility.Ability);
	}
}

void USpellMenuWidgetController::SpellRowGlobePressed(const FGameplayTag &SlotTag, const FGameplayTag &AbilityType)
{
	if (!bWaitingForEquipSelection)
	{
		return;
	}

	// Check selected ability against the slot's ability type.
	const FGameplayTag &SelectedAbilityType = AbilityInfo->FindAbilityInfoForTag(SelectedAbility.Ability).AbilityType;
	if (!SelectedAbilityType.MatchesTagExact(AbilityType))
	{
		return;
	}

	GetAuraASC()->ServerEquipAbility(SelectedAbility.Ability, SlotTag);

}

void USpellMenuWidgetController::OnAbilityEquipped(const FGameplayTag &AbilityTag, const FGameplayTag &Status,
                                                   const FGameplayTag &Slot, const FGameplayTag &PrevSlot)
{
	bWaitingForEquipSelection = false;

	const FAuraGameplayTags &GameplayTags = FAuraGameplayTags::Get();
	FAuraAbilityInfo LastSlotInfo;
	LastSlotInfo.StatusTag = GameplayTags.AbilitiesStatusUnlocked;
	LastSlotInfo.InputTag = PrevSlot;
	LastSlotInfo.AbilityTag = GameplayTags.AbilitiesNone;
	AbilityInfoDelegate.Broadcast(LastSlotInfo);

	FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
	Info.StatusTag = Status;
	Info.InputTag = Slot;
	AbilityInfoDelegate.Broadcast(Info);

	StopWaitForEquipDelegate.Broadcast(Info.AbilityType);

	SpellGlobeReassignedDelegate.Broadcast(AbilityTag);
	GlobeDeselected();
}

void USpellMenuWidgetController::ShouldEnableButtons(const FGameplayTag &AbilityStatus, int32 SpellPoints,
                                                     bool &bShouldEnableSpellPointsButton,
                                                     bool &bShouldEnableEquipButton)
{
	const FAuraGameplayTags &GameplayTags = FAuraGameplayTags::Get();

	bShouldEnableSpellPointsButton = bShouldEnableEquipButton = false;
	if (AbilityStatus.MatchesTagExact(GameplayTags.AbilitiesStatusLocked))
	{
		bShouldEnableEquipButton = false;
	}
	else if (AbilityStatus.MatchesTagExact(GameplayTags.AbilitiesStatusEligible))
	{
		bShouldEnableEquipButton = false;
		if (SpellPoints > 0)
		{
			bShouldEnableSpellPointsButton = true;
		}
	}
	else if (AbilityStatus.MatchesTagExact(GameplayTags.AbilitiesStatusUnlocked))
	{
		bShouldEnableEquipButton = true;
		if (SpellPoints > 0)
		{
			bShouldEnableSpellPointsButton = true;
		}
	}
	else if (AbilityStatus.MatchesTagExact(GameplayTags.AbilitiesStatusEquipped))
	{
		bShouldEnableEquipButton = true;
		if (SpellPoints > 0)
		{
			bShouldEnableSpellPointsButton = true;
		}
	}
}
#pragma optimize("", on)
