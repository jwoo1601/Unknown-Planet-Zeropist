// Copyright 2017 Ground Breaking Games. All Rights Reserved.

#include "ActorDamageBPLibrary.h"
#include "Client.h"
#include "GameMode/ZGameMode.h"

void UActorDamageBPLibrary::ActorReactToDamage(UObject const* WorldContextObject, FDamageableActorState const& InCurrentActorState, int32 InitialDamage, FDamageableActorState &OutFinalActorState, int32 &OutFinalDamage)
{
	UWorld *World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);

	if (World)
	{
		int32 FinalDamage = InitialDamage;

		if (InCurrentActorState.HP > 0)
		{
			if (InCurrentActorState.Shield > 0)
			{
				AZGameMode *GameModeInst = Cast<AZGameMode>(World->GetAuthGameMode());
				if (GameModeInst)
				{
					float DamageReductionRate = GameModeInst->GetDamageReductionRate();
					if (!FMath::IsNearlyZero(DamageReductionRate))
					{
						int32 DamageToReduce;

						if (InCurrentActorState.Shield >= InitialDamage)
						{
							DamageToReduce = FMath::FloorToInt(InitialDamage * DamageReductionRate);
						}

						else
						{
							DamageToReduce = FMath::FloorToInt(InCurrentActorState.Shield * DamageReductionRate);
						}

						CLIENT_LOG(TEXT("Reduced Damage by Shield: %i"), DamageToReduce);

						FinalDamage -= DamageToReduce;

						CLIENT_LOG(TEXT("Final Damage: %i"), FinalDamage);
					}

					OutFinalActorState.Shield = FMath::Max(0, InCurrentActorState.Shield - InitialDamage);
				}

				else
				{
					CLIENT_ERROR(TEXT("Failed to get an instance of AZGameMode via GetAuthGameMode: Skipped Damaging Process"))
				}
			}

			OutFinalActorState.HP = FMath::Max(0, InCurrentActorState.HP - FinalDamage);
		}

		OutFinalDamage = FinalDamage;
	}

	else
	{
		CLIENT_ERROR(TEXT("Failed to apply damage to current FDamageableActorState: World is NULL!"))
	}
}


