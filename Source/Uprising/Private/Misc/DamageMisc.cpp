#include "DamageMisc.h"	

FDamageableActorSettings::FDamageableActorSettings()
{
	DamageableActorTraceChannels.Reserve(5);
	DamageableActorTraceChannels.Add(ECC_Pawn);
	DamageableActorTraceChannels.Add(ECC_Destructible);
	DamageableActorTraceChannels.Add(COLLISION_HERO);
	DamageableActorTraceChannels.Add(COLLISION_STATIC_DAMAGEABLE);
	DamageableActorTraceChannels.Add(COLLISION_DYNAMIC_DAMAGEABLE);
}