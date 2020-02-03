// Copyright 2017 Ground Breaking Games. All Rights Reserved.

#include "ZGameMode.h"
#include "Client.h"
#include "Hero/HeroCharacter.h"
#include "Player/ZPlayerController.h"
#include "Team/Team.h"
#include "Team/TeamStart.h"
#include "UI/ZPlayerHUD.h"

AZGameMode::AZGameMode(const FObjectInitializer &ObjectInitializer) : Super(ObjectInitializer), bCanModifyMaxLevel(true), bCanModifyNumTeam(true), bCanModifyNumMaxMember(true)
{
	//temporary code
	ATeam *Team = NewObject<ATeam>();
	Team->Initialize(this, FName(), FColor::White);

	TeamList.Add(Team);
	//end temporary code

	DefaultPawnClass = AHeroCharacter::StaticClass();
	HUDClass = AZPlayerHUD::StaticClass();
	PlayerControllerClass = AZPlayerController::StaticClass();
}

void AZGameMode::BeginPlay()
{
	Super::BeginPlay();


}

void AZGameMode::StartPlay()
{
	Super::StartPlay();
}

AActor* AZGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	APlayerStart *FinalStart = nullptr;

	for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
	{
		APlayerStart *Start = *It;
#ifdef WITH_EDITORONLY_DATA
		if (Start->PlayerStartTag == TEXT("TEST_START"))
		{
			return Start;
		}
#endif

		if (ATeamStart *TeamStart = Cast<ATeamStart>(Start))
		{
			/* if (AZPlayerController *Controller = Cast<AZPlayerController>(Player) && Controller->GetCurrentTeam()->GetID() == TeamStart->GetTeamID())
			{
				TeamStart->SetTargetTeam(Controller->GetCurrentTeam());
			} */
		}
	}
	
	if (FinalStart == nullptr)
	{
		FinalStart = Cast<APlayerStart>(Super::ChoosePlayerStart(Player));
	}

	return FinalStart;
}

void AZGameMode::PostEditChangeProperty(FPropertyChangedEvent &e) {
	Super::PostEditChangeProperty(e);

	/* FName PropertyName = e.Property != nullptr ? e.Property->GetFName() : NAME_None;

#pragma region
	if (PropertyName == GET_MEMBER_NAME_CHECKED(AZGameMode, HeroMaxLevel)) {
		bCanModifyMaxLevel = false;

		if (OldMaxLevel > MaxLevel) {
			int32 Diff = OldMaxLevel - MaxLevel;
			for (int32 i = 0; i < Diff; i++)
				XPTable.Pop(false);

			XPTable.Shrink();
		}

		else if (OldMaxLevel < MaxLevel) {
			UGlobalData *Instance = UGlobalData::Get();
			if (Instance == nullptr)
				CLIENT_ERROR(TEXT("Failed to Get UGlobalData Instance"))

			else {
				int32 Diff = MaxLevel - OldMaxLevel;
				XPTable.Reserve(XPTable.Num() + Diff);

				for (int32 i = 0; i < Diff; i++) {
					int32 CurrentLevel = XPTable.Num() + 1;

					if (XPCalcMethod == EXPCalcMethod::Custom)
						XPTable.Add(FHeroXPData(CurrentLevel, CalculateExp(CurrentLevel)));

					else
						XPTable.Add(FHeroXPData(CurrentLevel, Instance->EditorDefaultXP));
				}
			}
		}

		OldMaxLevel = MaxLevel;
		bCanModifyMaxLevel = true;
	}
#pragma endregion HeroMaxLevel -> XPTable

	else if (PropertyName == GET_MEMBER_NAME_CHECKED(AZGameMode, NumTeam)) {
		bCanModifyNumTeam = false;
		
		int32 Num = FMath::Abs(OldNumTeam - NumTeam);
		if (OldNumTeam > NumTeam) {
			for (int32 i = 0; i < Num; i++)
				TeamList.Pop(false);

			TeamList.Shrink();
		}

		else if (OldNumTeam < NumTeam) {
			TeamList.Reserve(TeamList.Num() + Num);
			
			for (int32 i = 0; i < Num; i++) {
				int32 CurrentID = TeamList.Num() + 1;
				
				ATeam *Team = NewObject<ATeam>(this);
				Team->Initialize(this, CurrentID, FColor::White);

				TeamList.Add(Team);
			}
		}

		OldNumTeam = NumTeam;
		bCanModifyNumTeam = true;
	}

	else if (PropertyName == GET_MEMBER_NAME_CHECKED(AZGameMode, NumMaxMember)) {
		bCanModifyNumMaxMember = false;
		bCanModifyNumTeam = false;

		for (ATeam *Team : TeamList)
			Team->SetNumMaxMember(NumMaxMember);

		bCanModifyNumMaxMember = true;
		bCanModifyNumTeam = true;
	} */
}

float AZGameMode::GetDamageReductionRate() const
{
	return DamageReductionRate;
}
int32 AZGameMode::GetXPGen() const
{
	return XPGen;
}

int32 AZGameMode::GetHeroMaxLevel() const
{
	return HeroMaxLevel;
}

const TArray<FHeroLevelData>& AZGameMode::GetXPTable() const
{
	return HeroXPTable;
}

int32 AZGameMode::GetNumTeam() const
{
	return NumTeam;
}

const TArray<ATeam*>& AZGameMode::GetTeamList() const
{
	return TeamList;
}

int32 AZGameMode::GetNumMaxMember() const
{
	return NumMaxMember;
}
