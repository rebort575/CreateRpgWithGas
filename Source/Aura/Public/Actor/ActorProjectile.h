// Copyright Rebort

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ActorProjectile.generated.h"

class UProjectileMovementComponent;
class USphereComponent;

UCLASS()
class AURA_API AActorProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AActorProjectile();

	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComponent;
protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> Sphere;
};
