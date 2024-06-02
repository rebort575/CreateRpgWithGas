// Copyright Rebort

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPlayerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AURA_API IPlayerInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent)
	int32 FindLevelForXP(int32 InXP);

	UFUNCTION(BlueprintNativeEvent)
	int32 GetXP();

	UFUNCTION(BlueprintNativeEvent)
	int32 GetAttributePointReward(int32 Level);

	UFUNCTION(BlueprintNativeEvent)
	int32 GetSpellPointReward(int32 Level);

	UFUNCTION(BlueprintNativeEvent)
	void AddToXP(int32 InXP);

	UFUNCTION(BlueprintNativeEvent)
	void AddToPlayerLevel(int32 InLevel);

	UFUNCTION(BlueprintNativeEvent)
	void AddToAttributePoints(int32 InAttributePoints);

	UFUNCTION(BlueprintNativeEvent)
	void AddToSpellPoints(int32 InSpellPoints);

	UFUNCTION(BlueprintNativeEvent)
	int32 GetAttributePoints();

	UFUNCTION(BlueprintNativeEvent)
	int32 GetSpellPoints();

	UFUNCTION(BlueprintNativeEvent)
	void LevelUp();
};
