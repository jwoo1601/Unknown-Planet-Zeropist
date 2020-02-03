// Copyright 2017 Ground Breaking Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Player/PlayerList.h"

#include "ZPlayer.generated.h"

USTRUCT(BlueprintType)
struct FUniqueTag
{
	GENERATED_BODY()

public:

	friend FArchive& operator<<(FArchive& Ar, FUniqueTag &Tag)
	{
		return Ar << Tag.PersistentName << Tag.UniqueId;
	}

	bool Serialize(FArchive& Ar)
	{
		Ar << *this;
		return true;
	}


private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FName PersistentName;

	union
	{
		struct
		{
			uint8 Idx0;
			uint8 Idx1;
			uint8 Idx2;
			uint8 Idx3;
		};

		UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		uint32 UniqueId;
	};

public:

	FORCEINLINE FName GetPersistentName() const
	{
		return PersistentName;
	}

	FORCEINLINE uint32 GetUniqueId() const
	{
		return UniqueId;
	}

	FORCEINLINE bool operator==(const FUniqueTag &InTag) const
	{
		return PersistentName == InTag.PersistentName && UniqueId == InTag.UniqueId;
	}

	FORCEINLINE bool operator!=(const FUniqueTag &InTag) const
	{
		return !(*this == InTag);
	}

};

inline uint32 GetTypeHash(const FUniqueTag &InTag)
{
	return GetTypeHash(InTag.GetPersistentName()) + InTag.GetUniqueId();
}

/**
 * 
 */
UCLASS(BlueprintType)
class CLIENT_API UZPlayer : public UObject
{
	GENERATED_BODY()

public:

	UZPlayer(const FObjectInitializer &ObjectInitializer) : Super(ObjectInitializer) { };

	UFUNCTION(BlueprintCallable)
	FORCEINLINE FName GetPlayerDisplayName() const
	{
		return PlayerTag.GetPersistentName();
	}

	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool HasTeam() const
	{
		return Team == nullptr;
	}

	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool HasGroup() const
	{
		return false;
	}

	// uint32 GetNumOfFriends() const
	// bool AddFriend(UZPlayer *InPlayer & FUniqueTag &InTag)
	// bool RemoveFriend(UZPlayer *InPlayer & FUniqueTag &InTag)
	// bool JoinGroup(UPlayerGroup *InGroup)
	// bool LeaveGroup()
	// bool IsGroupLeader() const
	// bool IsInGame() const
	// bool IsSearchingforMatch const

	/* Need to Add these? */
	// FGroupJoinSignature OnJoinGroup
	// FGroupLeaveSignature OnLeaveGroup

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	struct FUniqueTag PlayerTag;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UPlayerGroup *Group;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class ATeam *Team;

	/* UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	UPlayerList *FriendList; */

	//TThreadSafePlayerList<UZPlayer, PlayerTypeTraits<UZPlayer, FUniqueTag>> FriendList;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int32 CurrentPlayTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int32 TotalPlayTime;

public:

	FORCEINLINE FUniqueTag GetPlayerTag() const
	{
		return PlayerTag;
	}

	FORCEINLINE const UPlayerGroup* GetGroup() const
	{
		return Group;
	}

	FORCEINLINE const ATeam* GetTeam() const
	{
		return Team;
	}

	/* FORCEINLINE const UPlayerList* GetFriendsList() const
	{
		return FriendList;
	} */

	/* FORCEINLINE bool operator==(const UZPlayer &InPlayer) const
	{
		return PlayerTag == InPlayer.PlayerTag;
	}
	
	FORCEINLINE bool operator!=(const UZPlayer &InPlayer) const
	{
		return !(*this == InPlayer);
	} */

public:

};

/* inline uint32 GetTypeHash(const UZPlayer &InPlayer)
{
	return GetTypeHash(InPlayer.GetPlayerTag());
} */

typedef TPlayerList<UZPlayer, PlayerTypeTraits<UZPlayer, FUniqueTag>> ZPlayerList;
typedef TPlayerList<UZPlayer, PlayerTypeTraits<UZPlayer, FUniqueTag>, true> ThreadSafeZPlayerList;