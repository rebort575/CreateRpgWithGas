// Copyright Rebort


#include "AbilitySystem/Passive/PassiveNiagaraComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Interaction/CombatInterface.h"

UPassiveNiagaraComponent::UPassiveNiagaraComponent()
{
	bAutoActivate = false;
}

void UPassiveNiagaraComponent::BeginPlay()
{
	Super::BeginPlay();

	if (UAuraAbilitySystemComponent *AuraASC = Cast<UAuraAbilitySystemComponent>(
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner())))
	{
		AuraASC->ActivatePassiveEffect.AddUObject(this, &UPassiveNiagaraComponent::OnPassiveActivate);
	}
	else if (ICombatInterface *CombatInterface = Cast<ICombatInterface>(GetOwner()); CombatInterface)
	{
		CombatInterface->GetOnAscRegisteredDelegate().AddWeakLambda(this, [this](UAbilitySystemComponent *ASC)
		{
			if (UAuraAbilitySystemComponent *AuraASC = Cast<UAuraAbilitySystemComponent>(ASC))
			{
				AuraASC->ActivatePassiveEffect.AddUObject(this, &UPassiveNiagaraComponent::OnPassiveActivate);
			}
		});
	}
}

void UPassiveNiagaraComponent::OnPassiveActivate(const FGameplayTag &AbilityTag, bool bActivate)
{
	if (!AbilityTag.MatchesTagExact(PassiveSpellTag))
	{
		return;
	}
	if (bActivate)
	{
		if (!IsActive())
		{
			Activate();
		}
	}
	else
	{
		Deactivate();
	}
}
