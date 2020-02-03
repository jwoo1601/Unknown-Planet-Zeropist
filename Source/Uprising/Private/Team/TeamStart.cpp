// Copyright 2017 Ground Breaking Games. All Rights Reserved.

#include "TeamStart.h"


ATeamStart::ATeamStart(const FObjectInitializer &ObjectInitializer) : Super(ObjectInitializer)
{

}

FName ATeamStart::GetTeamID() const { return TeamID; }
ATeam* ATeamStart::GetTargetTeam() const { return TargetTeam; }
