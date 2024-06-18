// Copyright Rebort


#include "AbilitySystem/AuraAbilitySystemLibrary.h"

#include "AuraAbilityTypes.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
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

bool UAuraAbilitySystemLibrary::IsSuccessfulDebuff(const FGameplayEffectContextHandle &EffectContextHandle)
{
	if (const FAuraGameplayEffectContext *AuraGameplayEffectContext = static_cast<const FAuraGameplayEffectContext *>(
		EffectContextHandle.Get()))
	{
		return AuraGameplayEffectContext->IsSuccessfulDebuff();
	}
	return false;
}

float UAuraAbilitySystemLibrary::GetDebuffDamage(const FGameplayEffectContextHandle &EffectContextHandle)
{
	if (const FAuraGameplayEffectContext *AuraGameplayEffectContext = static_cast<const FAuraGameplayEffectContext *>(
		EffectContextHandle.Get()))
	{
		return AuraGameplayEffectContext->GetDebuffDamage();
	}
	return 0.f;
}

float UAuraAbilitySystemLibrary::GetDebuffDuration(const FGameplayEffectContextHandle &EffectContextHandle)
{
	if (const FAuraGameplayEffectContext *AuraGameplayEffectContext = static_cast<const FAuraGameplayEffectContext *>(
		EffectContextHandle.Get()))
	{
		return AuraGameplayEffectContext->GetDebuffDuration();
	}
	return 0.f;
}

float UAuraAbilitySystemLibrary::GetDebuffFrequency(const FGameplayEffectContextHandle &EffectContextHandle)
{
	if (const FAuraGameplayEffectContext *AuraGameplayEffectContext = static_cast<const FAuraGameplayEffectContext *>(
		EffectContextHandle.Get()))
	{
		return AuraGameplayEffectContext->GetDebuffFrequency();
	}
	return 0.f;
}

FGameplayTag UAuraAbilitySystemLibrary::GetDamageType(const FGameplayEffectContextHandle &EffectContextHandle)
{
	if (const FAuraGameplayEffectContext *AuraGameplayEffectContext = static_cast<const FAuraGameplayEffectContext *>(
		EffectContextHandle.Get()))
	{
		if (AuraGameplayEffectContext->GetDamageType().IsValid())
		{
			return *AuraGameplayEffectContext->GetDamageType().Get();
		}
	}
	return FGameplayTag();
}

FVector UAuraAbilitySystemLibrary::GetDeathImpulse(const FGameplayEffectContextHandle &EffectContextHandle)
{
	if (const FAuraGameplayEffectContext *AuraGameplayEffectContext = static_cast<const FAuraGameplayEffectContext *>(
		EffectContextHandle.Get()))
	{
		return AuraGameplayEffectContext->GetDeathImpulse();
	}
	return FVector::ZeroVector;
}

FVector UAuraAbilitySystemLibrary::GetKnockbackForce(const FGameplayEffectContextHandle &EffectContextHandle)
{
	if (const FAuraGameplayEffectContext *AuraGameplayEffectContext = static_cast<const FAuraGameplayEffectContext *>(
		EffectContextHandle.Get()))
	{
		return AuraGameplayEffectContext->GetKncokbackForce();
	}
	return FVector::ZeroVector;
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

void UAuraAbilitySystemLibrary::SetIsSuccessfulDebuff(FGameplayEffectContextHandle &EffectContextHandle,
                                                      bool InSuccessfulDebuff)
{
	if (FAuraGameplayEffectContext *AuraGameplayEffectContext = static_cast<FAuraGameplayEffectContext *>(
		EffectContextHandle.Get()))
	{
		AuraGameplayEffectContext->SetIsSuccessfulDebuff(InSuccessfulDebuff);
	}
}

void UAuraAbilitySystemLibrary::SetDebuffDamage(FGameplayEffectContextHandle &EffectContextHandle, float InDebuffDamage)
{
	if (FAuraGameplayEffectContext *AuraGameplayEffectContext = static_cast<FAuraGameplayEffectContext *>(
		EffectContextHandle.Get()))
	{
		AuraGameplayEffectContext->SetDebuffDamage(InDebuffDamage);
	}
}

void UAuraAbilitySystemLibrary::SetDebuffDuration(FGameplayEffectContextHandle &EffectContextHandle,
                                                  float InDebuffDuration)
{
	if (FAuraGameplayEffectContext *AuraGameplayEffectContext = static_cast<FAuraGameplayEffectContext *>(
		EffectContextHandle.Get()))
	{
		AuraGameplayEffectContext->SetDebuffDuration(InDebuffDuration);
	}
}

void UAuraAbilitySystemLibrary::SetDebuffFrequency(FGameplayEffectContextHandle &EffectContextHandle,
                                                   float InDebuffFrequency)
{
	if (FAuraGameplayEffectContext *AuraGameplayEffectContext = static_cast<FAuraGameplayEffectContext *>(
		EffectContextHandle.Get()))
	{
		AuraGameplayEffectContext->SetDebuffFrequency(InDebuffFrequency);
	}
}

void UAuraAbilitySystemLibrary::SetDamageType(FGameplayEffectContextHandle &EffectContextHandle,
                                              const FGameplayTag &InDamageType)
{
	if (FAuraGameplayEffectContext *AuraGameplayEffectContext = static_cast<FAuraGameplayEffectContext *>(
		EffectContextHandle.Get()))
	{
		const TSharedPtr<FGameplayTag> DamageType = MakeShared<FGameplayTag>(InDamageType);
		AuraGameplayEffectContext->SetDamageType(DamageType);
	}
}

void UAuraAbilitySystemLibrary::SetDeathImpulse(FGameplayEffectContextHandle &EffectContextHandle,
                                                const FVector &InImpulse)
{
	if (FAuraGameplayEffectContext *AuraGameplayEffectContext = static_cast<FAuraGameplayEffectContext *>(
		EffectContextHandle.Get()))
	{
		AuraGameplayEffectContext->SetDeathImpulse(InImpulse);
	}
}

void UAuraAbilitySystemLibrary::SetKnockbackForce(FGameplayEffectContextHandle &EffectContextHandle,
                                                  const FVector &InForce)
{
	if (FAuraGameplayEffectContext *AuraGameplayEffectContext = static_cast<FAuraGameplayEffectContext *>(
		EffectContextHandle.Get()))
	{
		AuraGameplayEffectContext->SetKnockbackForce(InForce);
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

FGameplayEffectContextHandle UAuraAbilitySystemLibrary::ApplyDamageEffect(const FDamageEffectParams &DamageEffectParams)
{
	const FAuraGameplayTags &GameplayTags = FAuraGameplayTags::Get();
	const AActor *SourceAvatarActor = DamageEffectParams.SourceAbilitySystemComponent->GetAvatarActor();

	FGameplayEffectContextHandle EffectContext = DamageEffectParams.SourceAbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(SourceAvatarActor);
	SetDeathImpulse(EffectContext, DamageEffectParams.DeathImpulse);
	SetKnockbackForce(EffectContext, DamageEffectParams.KnockbackForce);

	const FGameplayEffectSpecHandle SpecHandle = DamageEffectParams.SourceAbilitySystemComponent->MakeOutgoingSpec(
		DamageEffectParams.DamageGameplayEffectClass,
		DamageEffectParams.AbilityLevel, EffectContext);

	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, DamageEffectParams.DamageType,
	                                                              DamageEffectParams.BaseDamage);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.DebuffChance,
	                                                              DamageEffectParams.DebuffChance);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.DebuffDamage,
	                                                              DamageEffectParams.DebuffDamage);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.DebuffDuration,
	                                                              DamageEffectParams.DebuffDuration);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.DebuffFrequency,
	                                                              DamageEffectParams.DebuffFrequency);
	DamageEffectParams.TargetAbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	return EffectContext;
}
