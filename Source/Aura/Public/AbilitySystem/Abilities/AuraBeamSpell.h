// Copyright Rebort

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"
#include "AuraBeamSpell.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraBeamSpell : public UAuraDamageGameplayAbility
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void StoreMouseDataInfo(const FHitResult &HitResult);

	UFUNCTION(BlueprintCallable)
	void StoreOwnerPlayerVariables();

	UFUNCTION(BlueprintCallable)
	void TraceFirstTarget(const FVector &BeamTargetLocation);

	UFUNCTION(BlueprintCallable)
	void StoreAdditionalTarget(TArray<AActor *> &OutAdditionalTargets);

	UFUNCTION(BlueprintImplementableEvent)
	void PrimaryTargetDead(AActor *DeadActor);

	UFUNCTION(BlueprintImplementableEvent)
	void AdditionalTargetDead(AActor *DeadActor);

protected:
	UPROPERTY(BlueprintReadWrite, Category="Beam")
	FVector MouseHitLocation;

	UPROPERTY(BlueprintReadWrite, Category="Beam")
	TObjectPtr<AActor> MouseHitActor;

	UPROPERTY(BlueprintReadWrite, Category="Beam")
	TObjectPtr<APlayerController> OwnerPlayerController;

	UPROPERTY(BlueprintReadWrite, Category="Beam")
	TObjectPtr<ACharacter> OwnerCharacter;

	UPROPERTY(EditDefaultsOnly)
	int32 MaxNumShockTargets = 5;
};
