// Copyright Rebort

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerController.h"
#include "AuraPlayerController.generated.h"

class UAuraAbilitySystemComponent;
class UAuraInputConfig;
class UInputMappingContext;
class UInputAction;
class IEnemyInterface;
/**
 * 
 */
UCLASS()
class AURA_API AAuraPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AAuraPlayerController();

	virtual void PlayerTick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputMappingContext> AuraContext;


	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> MoveAction;

	void Move(const struct FInputActionValue& InputActionVal);

	void CursorTrace();
	IEnemyInterface* LastActor;
	IEnemyInterface* ThisActor;

	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UAuraInputConfig> InputConfig;

	TObjectPtr<UAuraAbilitySystemComponent> AuraAbilitySystemComponent;
	UAuraAbilitySystemComponent* GetASC();
};
