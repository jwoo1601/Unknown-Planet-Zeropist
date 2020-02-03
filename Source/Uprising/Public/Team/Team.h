// Copyright 2017 Ground Breaking Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Player/PlayerList.h"
#include "Player/ZPlayer.h"

#include "Team.generated.h"

USTRUCT(BlueprintType)
struct CLIENT_API FTeamCreationEvent
{
	GENERATED_BODY()

public:

	FTeamCreationEvent();

	FTeamCreationEvent(class AZGameMode *InGamemode, FName InTeamID, FColor InTeamColor/*, ATeam::PlayerListType InMemberList*/);

	UPROPERTY(Transient)
	class AZGameMode *Gamemode;

	UPROPERTY(Transient)
	FName TeamID;

	UPROPERTY(Transient)
	FColor TeamColor;

	//ATeam::PlayerListType InitialMemberList;

};

USTRUCT(BlueprintType)
struct CLIENT_API FTeamMemberEvent
{
	GENERATED_BODY()

public:

	FTeamMemberEvent();

	FTeamMemberEvent(UZPlayer *InPlayer);

	UPROPERTY(Transient)
	UZPlayer *Player;

};

USTRUCT(BlueprintType)
struct CLIENT_API FTeamColorChangedEvent
{
	GENERATED_BODY()

public:

	FTeamColorChangedEvent();

	FTeamColorChangedEvent(FColor InColor);

	UPROPERTY(Transient)
	FColor NewColor;

};

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, notplaceable)
class CLIENT_API ATeam : public AActor
{
	GENERATED_BODY()

	friend class AZGameMode;

public:

	ATeam(const FObjectInitializer &ObjectInitializer);

	UFUNCTION(BlueprintCallable)
	void Initialize(class AZGameMode *InGamemode, FName InID, FColor InTeamColor);

	/* UFUNCTION(BlueprintCallable)
	void ReserveMembers(uint32 InCount);

	UFUNCTION(BlueprintCallable)
	bool AddMember(UZPlayer *InPlayer);

	UFUNCTION(BlueprintCallable)
	bool RemoveMember(UZPlayer *InPlayer);

	UFUNCTION(BlueprintCallable)
	void ClearMemberList(); */

protected:

	// will be replaced as Dynamic Multicast Delegates
	UFUNCTION(BlueprintNativeEvent, Category = "Event|Team")
	void OnTeamCreated(const FTeamCreationEvent &e);
	virtual void OnTeamCreated_Implementation(const FTeamCreationEvent &e);

	UFUNCTION(BlueprintNativeEvent, Category = "Event|Team")
	void OnTeamDestroyed();
	virtual void OnTeamDestroyed_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = "Event|Team")
	void OnTeamColorChanged(const FTeamColorChangedEvent &e);
	virtual void OnTeamColorChanged_Implementation(const FTeamColorChangedEvent &e);

	UFUNCTION(BlueprintNativeEvent, Category = "Event|Member")
	void OnMemberJoined(const FTeamMemberEvent &e);
	virtual void OnMemberJoined_Implementation(const FTeamMemberEvent &e);

	UFUNCTION(BlueprintNativeEvent, Category = "Event|Member")
	void OnMemberLeft(const FTeamMemberEvent &e);
	virtual void OnMemberLeft_Implementation(const FTeamMemberEvent &e);
	// End

private:

	/* Owner of this team. usually AZGameMode */
	UPROPERTY(transient)
	class AZGameMode *Gamemode;

	// Use FName::IsNone to check if this name is valid
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Property", meta = (AllowPrivateAccess = "true"))
	FName ID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Property", meta = (AllowPrivateAccess = "true"))
	FColor TeamColor;

#if WITH_EDITORONLY_DATA
	UPROPERTY(VisibleAnywhere, Category = "Property", meta = (DisplayName = "Number of Max Member", AllowPrivateAccess = "true"))
	int32 NumMaxMember;
#endif

	ThreadSafeZPlayerList PlayerList;

public:
	
	FORCEINLINE class AZGameMode* GetGamemode() const;
	FORCEINLINE FName GetID() const;
	FORCEINLINE FColor GetTeamColor() const;
	FORCEINLINE const ThreadSafeZPlayerList& GetPlayerList() const;

#if WITH_EDITOR
	FORCEINLINE void SetNumMaxMember(int32 InNumMaxMember);
#endif
	
};