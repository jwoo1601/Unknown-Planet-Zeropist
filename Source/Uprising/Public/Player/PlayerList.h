// Copyright 2017 Ground Breaking Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Misc/ScopeLock.h"

#define DECLARE_PLAYERLIST_ELEMENT_CLASS() friend class TPlayerList;

#define NEW_SCOPE_LOCK(Condition, SynchObjName, Description) { if (Condition) { FScopeLock __##Description##_Lock(&SynchObjName); } }

template<typename InPlayerType, typename InIDType>
struct PlayerTypeTraits
{

	typedef typename TCallTraits<InPlayerType>::ValueType PlayerType;
	typedef typename PlayerType* StorageElementType;
	typedef typename TCallTraits<StorageElementType>::ParamType PlayerParamType;

	typedef typename TCallTraits<InIDType>::ValueType IDType;
	typedef typename TCallTraits<InIDType>::ParamType IDParamType;

	static FORCEINLINE IDType GetIdentifier(PlayerParamType InPlayer)
	{
		return InPlayer->GetPlayerTag();
	}

};

template<typename InPlayerType, typename TypeTraits, bool bThreadSafe = false>
class CLIENT_API TPlayerList
{
//	typedef typename TRemoveConst<typename TRemovePointer<typename TRemoveReference<typename TRemoveExtent<InPlayerType>::Type>::Type>::Type>::Type PlayerType;
//	typedef PlayerType* StorageElementType;
//	static_assert(TIsDerivedFrom<PlayerType, UZPlayer>::IsDerived, "InPlayerType must be derived from UZPlayer class");

	typedef typename TypeTraits::PlayerType PlayerType;
	typedef typename TypeTraits::StorageElementType StorageElementType;
	typedef typename TypeTraits::PlayerParamType PlayerParamType;

	typedef typename TypeTraits::IDType IDType;
	typedef typename TypeTraits::IDParamType IDParamType;

	typedef TPlayerList<PlayerType, TypeTraits, bThreadSafe> MyType;

	struct PlayerKeyFuncs : BaseKeyFuncs<StorageElementType, IDType, false>
	{
		// typedef typename TCallTraits<IDType>::ParamType KeyInitType;
		typedef IDParamType KeyInitType;
		//typedef typename TCallTraits<ElementType>::ParamType ElementInitType;
		typedef PlayerParamType ElementInitType;;

		/**
		* @return The key used to index the given element.
		*/
		static FORCEINLINE KeyInitType GetSetKey(ElementInitType Element)
		{
			return Element->GetIdentifier();
		}

		/**
		* @return True if the keys match.
		*/
		static FORCEINLINE bool Matches(KeyInitType A, KeyInitType B)
		{
			return A == B;
		}

		/** Calculates a hash index for a key. */
		static FORCEINLINE uint32 GetKeyHash(KeyInitType Key)
		{
			return GetTypeHash(Key);
		}
	};

	typedef TSet<StorageElementType, PlayerKeyFuncs> StorageType;

public:

	TPlayerList(uint32 InitialCount = 6)
	{
		NEW_SCOPE_LOCK(bThreadSafe, SynchStorage, WRITE);

		InternalStorage.Reserve(InitialCount);
	}

	TPlayerList(std::initializer_list<StorageElementType> InitList)
	{
		NEW_SCOPE_LOCK(bThreadSafe, SynchStorage, WRITE);

		InternalStorage.Append(InitList);
	}

	template<bool bThreadSafeA>
	TPlayerList(const TPlayerList<StorageElementType, TypeTraits, bThreadSafeA> &InPlayerList)
	{
		NEW_SCOPE_LOCK(bThreadSafe, SynchStorage, WRITE);
		NEW_SCOPE_LOCK(bThreadSafeA, InPlayerList.SynchStorage, READ);

		InternalStorage.Append(InPlayerList.InternalStorage);
	}

	template<bool bThreadSafeA>
	TPlayerList(TPlayerList<StorageElementType, TypeTraits, bThreadSafeA> &&InPlayerList)
	{
		NEW_SCOPE_LOCK(bThreadSafe, SynchStorage, WRITE);
		NEW_SCOPE_LOCK(bThreadSafeA, InPlayerList.SynchStorage, READ);

		InternalStorage.Append(MoveTemp(InPlayerList.InternalStorage));
	}

	TPlayerList(const TArray<StorageElementType> &InArray)
	{
		NEW_SCOPE_LOCK(bThreadSafe, SynchStorage, WRITE);

		InternalStorage.Append(InArray);
	}
	
	TPlayerList(TArray<StorageElementType> &&InArray)
	{
		NEW_SCOPE_LOCK(bThreadSafe, SynchStorage, WRITE);

		InternalStorage.Append(InArray);
	}

	TPlayerList(const TSet<StorageElementType> &InSet)
	{
		NEW_SCOPE_LOCK(bThreadSafe, SynchStorage, WRITE);

		InternalStorage.Append(InSet);
	}

	TPlayerList(TSet<StorageElementType> &&InSet)
	{
		NEW_SCOPE_LOCK(bThreadSafe, SynchStorage, WRITE);

		InternalStorage.Append(MoveTemp(InSet));
	}

	MyType& operator=(std::initializer_list<StorageElementType> InitList)
	{
		NEW_SCOPE_LOCK(bThreadSafe, SynchStorage, WRITE);

		InternalStorage = InitList;

		return *this;
	}

	template<bool bThreadSafeA>
	MyType& operator=(const TPlayerList<StorageElementType, TypeTraits, bThreadSafeA> &InPlayerList)
	{
		NEW_SCOPE_LOCK(bThreadSafe, SynchStorage, WRITE);
		NEW_SCOPE_LOCK(bThreadSafeA, InPlayerList.SynchStorage, READ);

		InternalStorage = InPlayerList.InternalStorage;

		return *this;
	}

	template<bool bThreadSafeA>
	MyType& operator=(TPlayerList<StorageElementType, TypeTraits, bThreadSafeA> &&InPlayerList)
	{
		NEW_SCOPE_LOCK(bThreadSafe, SynchStorage, WRITE);
		NEW_SCOPE_LOCK(bThreadSafeA, InPlayerList.SynchStorage, READ);

		InternalStorage = MoveTemp(InPlayerList.InternalStorage);

		return *this;
	}

	// will be replaced as Clear soon
	FORCEINLINE void Empty(int32 ExpectedNumElements = 0)
	{
		NEW_SCOPE_LOCK(bThreadSafe, InternalStorage, WRITE);
		InternalStorage.Empty(ExpectedNumElements);
	}

	// will be replaced as Clear soon
	FORCEINLINE void Reset()
	{
		NEW_SCOPE_LOCK(bThreadSafe, InternalStorage, WRITE);
		InternalStorage.Reset();
	}

	// will be replaced as Clear soon
	FORCEINLINE void Shrink()
	{
		NEW_SCOPE_LOCK(bThreadSafe, InternalStorage, WRITE);
		InternalStorage.Shrink();
	}

	FORCEINLINE int32 Num() const
	{
		return InternalStorage.Num();
	}

	// ClampMax(int32 NumMaxMembers)
	// Optimize(bool bKeepIterationOrder) -> Memory Optimization (Shrink, Compact, etc...)
	// Clear() -> Remove All Elements but Keep size as NumMaxMembers
	// GetAllocatedSize()
	// void ReplacePlayer(IDType), void ReplacePlayer(const PlayerType)
	// SendPacket(...) SendPacketToAll(...)

	bool AddPlayer(IDType InID)
	{
		NEW_SCOPE_LOCK(bThreadSafe, InternalStorage, READ_AND_WRITE);

		if (*InternalStorage.Find(InID) == nullptr)
		{
			StorageElementType Target;

			for (TObjectIterator<StorageElementType> It; It; ++It)
			{
				StorageElementType Player = Cast<PlayerType>(It->GetObject());

				if (Player)
				{
					Player->GetIdentifier() == InID;
					Target = Player;
				}
			}

			InternalStorage.Add(Target);
			return true;
		}

		return false;
	}
	FORCEINLINE bool AddPlayer(PlayerParamType InPlayer)
	{
		return AddPlayer(InPlayer.GetIdentifier());
	}

	FORCEINLINE void Append(const TArray<StorageElementType> &InArray)
	{
		NEW_SCOPE_LOCK(bThreadSafe, InternalStorage, WRITE);
		InternalStorage.Append(InArray);
	}

	FORCEINLINE void Append(TArray<StorageElementType> &&InArray)
	{
		NEW_SCOPE_LOCK(bThreadSafe, InternalStorage, WRITE);
		InternalStorage.Append(MoveTemp(InArray));
	}

	FORCEINLINE void Append(const TSet<StorageElementType> &InSet)
	{
		NEW_SCOPE_LOCK(bThreadSafe, InternalStorage, WRITE);
		InternalStorage.Append(InSet);
	}

	FORCEINLINE void Append(TSet<StorageElementType> &&InSet)
	{
		NEW_SCOPE_LOCK(bThreadSafe, InternalStorage, WRITE);
		InternalStorage.Append(MoveTemp(InSet));
	}

	template<bool bThreadSafeA>
	FORCEINLINE void Append(const TPlayerList<StorageElementType, TypeTraits, bThreadSafeA> &InPlayerList)
	{
		NEW_SCOPE_LOCK(bThreadSafe, InternalStorage, WRITE);
		NEW_SCOPE_LOCK(bThreadSafeA, InPlayerList.InternalStorage, READ);
		InternalStorage.Append(InPlayerList.InternalStorage);
	}

	template<bool bThreadSafeA>
	FORCEINLINE void Append(TPlayerList<StorageElementType, TypeTraits, bThreadSafeA> &&InPlayerList)
	{
		NEW_SCOPE_LOCK(bThreadSafe, InternalStorage, WRITE);
		NEW_SCOPE_LOCK(bThreadSafeA, InPlayerList.InternalStorage, READ);
		InternalStorage.Append(MoveTemp(InPlayerList.InternalStorage));
	}

	bool RemovePlayer(IDType InID)
	{
		NEW_SCOPE_LOCK(bThreadSafe, InternalStorage, WRITE_AND_READ);
		FSetElementId ID = InternalStorage.FindId(InID);
		if (ID.IsValidId())
		{
			InternalStorage.Remove(ID);
			return true;
		}

		return false;
	}

	FORCEINLINE bool RemovePlayer(PlayerParamType InPlayer)
	{
		return RemovePlayer(InPlayer.GetIdentifier());
	}

	FORCEINLINE bool HasMember(IDType InID) const
	{
		NEW_SCOPE_LOCK(bThreadSafe, InternalStorage, READ);
		return InternalStorage.Find(InID) != nullptr;
	}

	FORCEINLINE bool HasMember(PlayerParamType InPlayer) const
	{
		return HasMember(InPlayer->GetIdentifier());
	}
	
	// StorageElementType Find(IDType InID);
	// template<typename ContainerClass>
	// ContainerClass<PlayerType>& FindAllIf(TFunction<bool(const PlayerType)> InFunc);

	// Sort(...)

	// friend FArchive& operator<<(FArchive& Ar, TPlayerList& Set);

	FORCEINLINE	TArray<StorageElementType> ToArray()
	{
		return InternalStorage.Array();
	}

	FORCEINLINE TSet<StorageElementType> ToSet()
	{
		return TSet<StorageElementType>(InternalStorage);
	}
	
private:

	TSet<StorageElementType, PlayerKeyFuncs> InternalStorage;
	
	struct FThreadSafeDummy { };
	typename TChooseClass<bThreadSafe, FCriticalSection, FThreadSafeDummy>::Result SynchStorage;
	
};

#undef NEW_SCOPE_LOCK

template<typename PlayerType, typename TypeTraits>
using TThreadSafePlayerList = TPlayerList<PlayerType, TypeTraits, true>;