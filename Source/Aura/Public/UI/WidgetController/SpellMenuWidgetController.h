// Copyright Rebort

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "SpellMenuWidgetController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FSpellGlobeSelectedSignature, bool, bSpendPointButtonEnabled, bool,
                                              bEquipButtonEnabled, FString, Description, FString, NextLevelDescription);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWaitForEquipSelectionSignature, const FGameplayTag&, AbilityType);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSpellGlobeReassignedSignature, const FGameplayTag&, AbilityType);

struct FSelectedAbility
{
	FGameplayTag Ability = FGameplayTag();
	FGameplayTag Status = FGameplayTag();
};

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class AURA_API USpellMenuWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FOnPlayerStatChangeSignature SpellPointsChangedDelegate;

	UPROPERTY(BlueprintAssignable)
	FWaitForEquipSelectionSignature WaitForEquipSelectionDelegate;

	UPROPERTY(BlueprintAssignable)
	FWaitForEquipSelectionSignature StopWaitForEquipDelegate;

	UPROPERTY(BlueprintAssignable)
	FSpellGlobeReassignedSignature SpellGlobeReassignedDelegate;

	virtual void BroadcastInitialValues() override;

	virtual void BindCallbacksToDependencies() override;

	UPROPERTY(BlueprintAssignable)
	FSpellGlobeSelectedSignature SpellGlobeSelectedDelegate;

	UFUNCTION(BlueprintCallable)
	void SpellGlobeSelected(const FGameplayTag &AbilityTag);

	UFUNCTION(BlueprintCallable)
	void GlobeDeselected();

	void TryEnableButtons();

	UFUNCTION(BlueprintCallable)
	void SpendPointButtonPressed();

	UFUNCTION(BlueprintCallable)
	void EquipButtonPressed();

	UFUNCTION(BlueprintCallable)
	void SpellRowGlobePressed(const FGameplayTag &SlotTag, const FGameplayTag &AbilityType);

	void OnAbilityEquipped(const FGameplayTag &AbilityTag, const FGameplayTag &Status, const FGameplayTag &Slot,
	                       const FGameplayTag &PrevSlot);

private:
	static void ShouldEnableButtons(const FGameplayTag &AbilityStatus, int32 SpellPoints,
	                                bool &bShouldEnableSpellPointsButton,
	                                bool &bShouldEnableEquipButton);
	FSelectedAbility SelectedAbility;
	int32 CurrentSpellPoints;
	bool bWaitingForEquipSelection = false;
	FGameplayTag SelectedSlot;
};
