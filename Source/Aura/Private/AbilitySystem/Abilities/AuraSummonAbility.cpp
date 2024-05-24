// Copyright Rebort


#include "AbilitySystem/Abilities/AuraSummonAbility.h"

#include "Kismet/KismetSystemLibrary.h"

TArray<FVector> UAuraSummonAbility::GetSpawnLocations()
{
	TArray<FVector> SpawnLocations;
	const FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector();
	const FVector Location = GetAvatarActorFromActorInfo()->GetActorLocation();
	const FVector LeftOfSpread = Forward.RotateAngleAxis(-SpawnSpread / 2.f, FVector::UpVector);
	const FVector RightOfSpread = Forward.RotateAngleAxis(SpawnSpread / 2.f, FVector::UpVector);
	const float DeltaSpread = SpawnSpread / NumMinions;
	for (int i = 0; i < NumMinions; ++i)
	{
		const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i, FVector::UpVector);
		// UKismetSystemLibrary::DrawDebugArrow(GetAvatarActorFromActorInfo(), Location, Location + Direction * MaxSpawnDistance, 4.f, FLinearColor::Green, 3.f);

		FVector ChooseSpawnLocation = Location + Direction * FMath::FRandRange(MinSpawnDistance, MaxSpawnDistance);
		// DrawDebugSphere(GetWorld(), ChooseSpawnLocation, 18.f, 12, FColor::Cyan, false,3.f);
		// DrawDebugSphere(GetWorld(), Location+Direction*MinSpawnDistance, 5.f, 12, FColor::Red, false,3.f);
		// DrawDebugSphere(GetWorld(), Location+Direction*MaxSpawnDistance, 5.f, 12, FColor::Red, false,3.f);

		FHitResult Hit;
		GetWorld()->LineTraceSingleByChannel(Hit, ChooseSpawnLocation + FVector(0.f, 0.f, 400.f),
		                                     ChooseSpawnLocation - FVector(0.f, 0.f, 400.f), ECC_Visibility);
		if (Hit.bBlockingHit)
		{
			ChooseSpawnLocation = Hit.ImpactPoint;
		}
		SpawnLocations.Add(ChooseSpawnLocation);
	}

	return SpawnLocations;
}

TSubclassOf<APawn> UAuraSummonAbility::GetRandomMinionClass()
{
	const int Selection = FMath::RandRange(0, MinionClasses.Num() - 1);
	return MinionClasses[Selection];
}
