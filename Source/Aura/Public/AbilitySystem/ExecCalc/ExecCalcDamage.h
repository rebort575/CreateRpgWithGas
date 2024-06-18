// Copyright Rebort

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "ExecCalcDamage.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UExecCalcDamage : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	UExecCalcDamage();
	void DetermineDebuff(const FGameplayEffectCustomExecutionParameters &ExecutionParams,
	                     const FGameplayEffectSpec &Spec,
	                     FAggregatorEvaluateParameters EvaluationParameters,
	                     const TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition> &InTagsToDefs) const;

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters &ExecutionParams,
	                                    FGameplayEffectCustomExecutionOutput &OutExecutionOutput) const override;
};
