// Copyright Rebort


#include "UI/WidgetController/OverlayWidgetController.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "Aura/AuraLogChannels.h"
#include "Player/AuraPlayerState.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
	OnHealthChange.Broadcast(GetAuraAS()->GetHealth());
	OnMaxHealthChange.Broadcast(GetAuraAS()->GetMaxHealth());
	OnManaChange.Broadcast(GetAuraAS()->GetMana());
	OnMaxManaChange.Broadcast(GetAuraAS()->GetMaxMana());
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	GetAuraPS()->OnXPChangedDelegate.AddUObject(this, &UOverlayWidgetController::OnXPChanged);
	GetAuraPS()->OnLevelChangedDelegate.AddLambda(
		[this](int32 NewLevel)
		{
			UE_LOG(LogAura, Log, TEXT("OnLevelChangedDelegate %d"), GetWorld()->GetNetMode());
			OnPlayerLevelChangedDelegate.Broadcast(NewLevel);
		});

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAuraAS()->GetHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData &Data)
		{
			OnHealthChange.Broadcast(Data.NewValue);
		}
		);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAuraAS()->GetMaxHealthAttribute()).
	                        AddLambda(
		                        [this](const FOnAttributeChangeData &Data)
		                        {
			                        OnMaxHealthChange.Broadcast(Data.NewValue);
		                        }
		                        );

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAuraAS()->GetManaAttribute()).AddLambda(
		[this](const FOnAttributeChangeData &Data)
		{
			OnManaChange.Broadcast(Data.NewValue);
		}
		);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAuraAS()->GetMaxManaAttribute()).AddLambda(
		[this](const FOnAttributeChangeData &Data)
		{
			OnMaxManaChange.Broadcast(Data.NewValue);
		}
		);

	if (UAuraAbilitySystemComponent *AuraASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent); AuraASC)
	{
		AuraASC->AbilityEquipped.AddUObject(this, &UOverlayWidgetController::OnAbilityEquipped);
		if (AuraASC->bStartupAbilitiesGiven)
		{
			BroadcastAbilityInfo();
		}
		else
		{
			AuraASC->AbilitiesGivenDelegate.AddUObject(this, &UOverlayWidgetController::BroadcastAbilityInfo);
		}

		AuraASC->EffectAssetTags.AddLambda(
			[this](const FGameplayTagContainer &AssetTags)
			{
				for (const auto &Tag : AssetTags)
				{
					FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(FName("Message"));
					if (Tag.MatchesTag(MessageTag))
					{
						const FUIWidgetRow *Row = GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable, Tag);
						MessageWidgetRowDelegate.Broadcast(*Row);
					}
				}
			}
			);
	}
}

void UOverlayWidgetController::OnXPChanged(int32 NewXP)
{
	const ULevelUpInfo *LevelUpInfo = GetAuraPS()->LevelUpInfo;
	checkf(LevelUpInfo, TEXT("Unable to find LevelUpInfo"))

	const int32 Level = LevelUpInfo->FindLevelForXP(NewXP);
	const int32 MaxLevel = LevelUpInfo->LevelUpInfos.Num() - 1;
	if (Level > 0 && Level <= MaxLevel)
	{
		const int32 XPNeededThisLevel = LevelUpInfo->LevelUpInfos[Level].LevelUpRequirement;
		const int32 XPGainedThisLevel = NewXP - LevelUpInfo->LevelUpInfos[Level - 1].LevelUpRequirement;
		const float XPProgress = static_cast<float>(XPGainedThisLevel) / static_cast<float>(XPNeededThisLevel);
		OnXPPercentChangedDelegate.Broadcast(XPProgress);
		UE_LOG(LogAura, Log, TEXT("Change XP bar progress to :%.2f"), XPProgress);
	}
}

void UOverlayWidgetController::OnAbilityEquipped(const FGameplayTag &AbilityTag, const FGameplayTag &Status,
                                                 const FGameplayTag &Slot, const FGameplayTag &PrevSlot)
{
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
}
