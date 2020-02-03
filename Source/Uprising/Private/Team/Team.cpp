// Copyright 2017 Ground Breaking Games. All Rights Reserved.

#include "Team.h"
#include "GameMode/ZGameMode.h"

/* FTeamCreationEvent */
FTeamCreationEvent::FTeamCreationEvent() { }
FTeamCreationEvent::FTeamCreationEvent(AZGameMode *InGamemode, FName InTeamID, FColor InTeamColor/*, ATeam::PlayerListType InMemberList*/) : Gamemode(InGamemode), TeamID(InTeamID), TeamColor(InTeamColor)/*, InitialMemberList(InMemberList) */ { }
/* End FTeamCreationEvent */

/* FTeamMemberEvent */
FTeamMemberEvent::FTeamMemberEvent() { }
FTeamMemberEvent::FTeamMemberEvent(UZPlayer *InPlayer) : Player(InPlayer) { }
/* End FTeamMemberEvent */

/* FTeamColorChangedEvent */
FTeamColorChangedEvent::FTeamColorChangedEvent() { }
FTeamColorChangedEvent::FTeamColorChangedEvent(FColor InColor) : NewColor(InColor) { }
/* End FTeamColorChangedEvent */

/* ATeam */
ATeam::ATeam(const FObjectInitializer &ObjectInitializer) : Super(ObjectInitializer) { }

void ATeam::Initialize(AZGameMode *InGamemode, FName InID, FColor InColor)
{
	checkf(InGamemode, TEXT("InOwner must not be NULL"));
	//checkf(!InID.IsNone(), TEXT("InID must be an valid ID"));

	Gamemode = InGamemode;
	ID = InID;
	TeamColor = InColor;

	OnTeamCreated(FTeamCreationEvent(Gamemode, ID, TeamColor));
}

void ATeam::OnTeamCreated_Implementation(const FTeamCreationEvent &e) { }
void ATeam::OnTeamDestroyed_Implementation() { }
void ATeam::OnTeamColorChanged_Implementation(const FTeamColorChangedEvent &e) { }
void ATeam::OnMemberJoined_Implementation(const FTeamMemberEvent &e) { }
void ATeam::OnMemberLeft_Implementation(const FTeamMemberEvent &e) { }

AZGameMode* ATeam::GetGamemode() const { return Gamemode; }
FName ATeam::GetID() const { return ID; }
FColor ATeam::GetTeamColor() const { return TeamColor; }
const ThreadSafeZPlayerList& ATeam::GetPlayerList() const { return PlayerList; }

#if WITH_EDITOR
void ATeam::SetNumMaxMember(int32 InNumMaxMember)
{
	NumMaxMember = InNumMaxMember;
	// PlayerList.ClampMax(InMemberCount);
}
#endif
/* End ATeam */