// Copyright Rebort


#include "AbilitySystem/AuraAbilitySystemLibrary.h"

#include "AuraAbilityTypes.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Game/AuraGameModeBase.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"
#include "UI/WidgetController/AuraWidgetController.h"

bool UAuraAbilitySystemLibrary::MakeWidgetControllerParams(const UObject *WorldContextObject,
                                                           FWidgetControllerParams &OutWCParams, AAuraHUD *&OutAuraHUD)
{
	if (APlayerController *PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		if (AAuraHUD *AuraHUD = Cast<AAuraHUD>(PC->GetHUD()))
		{
			OutAuraHUD = AuraHUD;
			AAuraPlayerState *PS = PC->GetPlayerState<AAuraPlayerState>();
			UAbilitySystemComponent *ASC = PS->GetAbilitySystemComponent();
			UAttributeSet *AS = PS->GetAttributeSet();
			OutWCParams.AttributeSet = AS;
			OutWCParams.AbilitySystemComponent = ASC;
			OutWCParams.PlayerState = PS;
			OutWCParams.PlayerController = PC;
			return true;
		}
	}
	return false;
}

UOverlayWidgetController *UAuraAbilitySystemLibrary::GetOverlayWidgetController(const UObject *WorldContextObject)
{
	FWidgetControllerParams WCParams;
	AAuraHUD *AuraHUD;
	if (MakeWidgetControllerParams(WorldContextObject, WCParams, AuraHUD))
	{
		return AuraHUD->GetOverlayWidgetController(WCParams);
	}
	return nullptr;
}

UAttributeMenuWidgetController *UAuraAbilitySystemLibrary::GetAttributeMenuWidgetController(
	const UObject *WorldContextObject)
{
	FWidgetControllerParams WCParams;
	AAuraHUD *AuraHUD;
	if (MakeWidgetControllerParams(WorldContextObject, WCParams, AuraHUD))
	{
		return AuraHUD->GetAttributeMenuWidgetController(WCParams);
	}
	return nullptr;
}

USpellMenuWidgetController *UAuraAbilitySystemLibrary::GetSpellMenuWidgetController(const UObject *WorldContextObject)
{
	FWidgetControllerParams WCParams;
	AAuraHUD *AuraHUD;
	if (MakeWidgetControllerParams(WorldContextObject, WCParams, AuraHUD))
	{
		return AuraHUD->GetSpellMenuWidgetController(WCParams);
	}
	return nullptr;
}

void UAuraAbilitySystemLibrary::InitializeDefaultAttributes(const UObject *WorldContextObject,
                                                            ECharacterClass CharacterClass, float Level,
                                                            UAbilitySystemComponent *ASC)
{
	AActor *AvatarActor = ASC->GetAvatarActor();

	UCharacterClassInfo *CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	FCharacterClassDefaultInfo ClassDefaultInfo = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);

	FGameplayEffectContextHandle PrimaryAttributesContextHandle = ASC->MakeEffectContext();
	PrimaryAttributesContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle PrimaryAttributesSpecHandle = ASC->MakeOutgoingSpec(
		ClassDefaultInfo.PrimaryAttributes, Level, PrimaryAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*PrimaryAttributesSpecHandle.Data.Get());

	FGameplayEffectContextHandle SecondaryAttributesContextHandle = ASC->MakeEffectContext();
	SecondaryAttributesContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle SecondaryAttributesSpecHandle = ASC->MakeOutgoingSpec(
		CharacterClassInfo->SecondaryAttributes, Level, SecondaryAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*SecondaryAttributesSpecHandle.Data.Get());

	FGameplayEffectContextHandle VitalAttributesContextHandle = ASC->MakeEffectContext();
	VitalAttributesContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle VitalAttributesSpecHandle = ASC->MakeOutgoingSpec(
		CharacterClassInfo->VitalAttributes, Level, VitalAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*VitalAttributesSpecHandle.Data.Get());
}

void UAuraAbilitySystemLibrary::GiveStartupAbilities(const UObject *WorldContextObject, UAbilitySystemComponent *ASC,
                                                     ECharacterClass CharacterClass)
{
	UCharacterClassInfo *CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	if (CharacterClassInfo == nullptr)
	{
		return;
	}
	for (TSubclassOf<UGameplayAbility> AbilityClass : CharacterClassInfo->CommonAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1.f);
		ASC->GiveAbility(AbilitySpec);
	}
	const FCharacterClassDefaultInfo &DefaultInfo = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);
	for (TSubclassOf<UGameplayAbility> AbilityClass : DefaultInfo.StartupAbilities)
	{
		if (ASC->GetAvatarActor()->Implements<UCombatInterface>())
		{
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass,
			                                                        ICombatInterface::Execute_GetPlayerLevel(
				                                                        ASC->GetAvatarActor()));
			ASC->GiveAbility(AbilitySpec);
		}
	}
}

UCharacterClassInfo *UAuraAbilitySystemLibrary::GetCharacterClassInfo(const UObject *WorldContextObject)
{
	const AAuraGameModeBase *AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (nullptr == AuraGameMode)
	{
		return nullptr;
	}

	return AuraGameMode->CharacterClassInfo;
}

UAbilityInfo *UAuraAbilitySystemLibrary::GetAbilityInfo(const UObject *WorldContextObject)
{
	const AAuraGameModeBase *AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (nullptr == AuraGameMode)
	{
		return nullptr;
	}

	return AuraGameMode->AbilityInfo;
}

bool UAuraAbilitySystemLibrary::IsBlockedHit(const FGameplayEffectContextHandle &EffectContextHandle)
{
	if (const FAuraGameplayEffectContext *AuraGameplayEffectContext = static_cast<const FAuraGameplayEffectContext *>(
		EffectContextHandle.Get()))
	{
		return AuraGameplayEffectContext->IsBlockedHit();
	}
	return false;
}

bool UAuraAbilitySystemLibrary::IsCriticalHit(const FGameplayEffectContextHandle &EffectContextHandle)
{
	if (const FAuraGameplayEffectContext *AuraGameplayEffectContext = static_cast<const FAuraGameplayEffectContext *>(
		EffectContextHandle.Get()))
	{
		return AuraGameplayEffectContext->IsCriticalHit();
	}
	return false;
}

void UAuraAbilitySystemLibrary::SetIsBlockedHit(FGameplayEffectContextHandle &EffectContextHandle, bool bInIsBlockedHit)
{
	if (FAuraGameplayEffectContext *AuraGameplayEffectContext = static_cast<FAuraGameplayEffectContext *>(
		EffectContextHandle.Get()))
	{
		AuraGameplayEffectContext->SetIsBlockedHit(bInIsBlockedHit);
	}
}

void UAuraAbilitySystemLibrary::SetIsCriticalHit(FGameplayEffectContextHandle &EffectContextHandle,
                                                 bool bInIsCriticalHit)
{
	if (FAuraGameplayEffectContext *AuraGameplayEffectContext = static_cast<FAuraGameplayEffectContext *>(
		EffectContextHandle.Get()))
	{
		AuraGameplayEffectContext->SetIsCriticalHit(bInIsCriticalHit);
	}
}

void UAuraAbilitySystemLibrary::GetLivePlayersWithinRadius(const UObject *WorldContextObject,
                                                           TArray<AActor *> &OutOverlappingActors,
                                                           TArray<AActor *> &ActorsToIgnore, float Radius,
                                                           const FVector &SphereOrigin)
{
	FCollisionQueryParams SphereParams;
	SphereParams.AddIgnoredActors(ActorsToIgnore);

	TArray<FOverlapResult> Overlaps;
	if (const UWorld *World = GEngine->GetWorldFromContextObject(WorldContextObject,
	                                                             EGetWorldErrorMode::LogAndReturnNull))
	{
		World->OverlapMultiByObjectType(Overlaps, SphereOrigin, FQuat::Identity,
		                                FCollisionObjectQueryParams(
			                                FCollisionObjectQueryParams::InitType::AllDynamicObjects),
		                                FCollisionShape::MakeSphere(Radius), SphereParams);
		for (FOverlapResult &Overlap : Overlaps)
		{
			if (!Overlap.GetActor()->Implements<UCombatInterface>())
			{
				return;
			}
			if (ICombatInterface::Execute_IsDead(Overlap.GetActor()))
			{
				return;
			}

			OutOverlappingActors.AddUnique(Overlap.GetActor());
		}
	}
}

bool UAuraAbilitySystemLibrary::IsNotFriend(AActor *FirstActor, AActor *SecondActor)
{
	const bool bFirstIsPlayer = FirstActor->ActorHasTag(FName("Player"));
	const bool bSecondIsPlayer = SecondActor->ActorHasTag(FName("Player"));
	return (bFirstIsPlayer && !bSecondIsPlayer) || (!bFirstIsPlayer && bSecondIsPlayer);
}

int32 UAuraAbilitySystemLibrary::GetXPRewardForClassAndLevel(const UObject *WorldContextObject,
                                                             ECharacterClass CharacterClass, int32 CharacterLevel)
{
	UCharacterClassInfo *CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	if (CharacterClassInfo == nullptr)
	{
		return 0;
	}

	const FCharacterClassDefaultInfo &Info = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);
	return static_cast<int32>(Info.XPReward.GetValueAtLevel(CharacterLevel));
}
