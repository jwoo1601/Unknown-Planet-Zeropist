#pragma once

#include "CoreMinimal.h"
#include "CollisionQueryParams.h"
#include "Engine/EngineTypes.h"

#include "DamageMisc.generated.h"

#define COLLISION_HERO ECC_GameTraceChannel1
#define COLLISION_WEAPON ECC_GameTraceChannel2
#define COLLISION_STATIC_DAMAGEABLE ECC_GameTraceChannel3
#define COLLISION_DYNAMIC_DAMAGEABLE ECC_GameTraceChannel4

USTRUCT()
struct CLIENT_API FDamageableActorSettings
{	
	GENERATED_USTRUCT_BODY()

	FDamageableActorSettings();

	FORCEINLINE void AddDamageableActorType(ECollisionChannel InChannel)
	{
		if (!(IsBuiltInType(InChannel) || DamageableActorTraceChannels.Contains(InChannel)))
		{
			DamageableActorTraceChannels.Add((uint8)InChannel);
		}
	}

	FORCEINLINE void RemoveDamageableActorType(ECollisionChannel InChannel)
	{
		if (!IsBuiltInType(InChannel) && DamageableActorTraceChannels.Contains(InChannel))
		{
			DamageableActorTraceChannels.Remove((uint8)InChannel);
		}
	}

	FORCEINLINE auto GetDamageableActorTypeIterator()
	{
		return DamageableActorTraceChannels.CreateConstIterator();
	}

	FORCEINLINE void MakeObjectQueryParams(FCollisionObjectQueryParams &OutParams)
	{
		for (auto It = DamageableActorTraceChannels.CreateConstIterator(); It; ++It)
		{
			OutParams.AddObjectTypesToQuery((ECollisionChannel)(*It));
		}
	}

private:

	FORCEINLINE bool IsBuiltInType(ECollisionChannel InChannel)
	{
		return InChannel == ECC_Pawn ||
			   InChannel == ECC_Destructible ||
			   InChannel == COLLISION_HERO ||
			   InChannel == COLLISION_STATIC_DAMAGEABLE ||
			   InChannel == COLLISION_DYNAMIC_DAMAGEABLE;
	}

	UPROPERTY()
	TSet<uint8> DamageableActorTraceChannels;

};

static FDamageableActorSettings GDamageableActorSettings;

USTRUCT(BlueprintType)
struct CLIENT_API FDamageableActorState
{
	GENERATED_USTRUCT_BODY()

	FDamageableActorState() { }
	
	FDamageableActorState(int32 InHP, int32 InShield) : HP(InHP), Shield(InShield) { }

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 HP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Shield;

};