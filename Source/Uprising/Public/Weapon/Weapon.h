// Copyright 2017 Ground Breaking Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/MeshComponent.h"
#include "GameFramework/Actor.h"
#include "Animation/AimOffsetBlendSpace.h"

#include "Weapon.generated.h"

USTRUCT(BlueprintType)
struct CLIENT_API FWeaponEventParams
{
	GENERATED_USTRUCT_BODY()

	FWeaponEventParams() { }

	FWeaponEventParams(APawn *InWeaponOwner, class AWeapon *InWeapon) : WeaponOwner(InWeaponOwner), Weapon(InWeapon) { }

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	APawn *WeaponOwner;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class AWeapon *Weapon;

};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWeaponOwnerAssignedSignature, FWeaponEventParams const&, InParams);

UENUM(BlueprintType)
enum class EAttackTypeSelectionMethod : uint8
{
	Sequential,
	Random,
};

UCLASS(abstract, BlueprintType, Blueprintable)
class CLIENT_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	

	AWeapon(FObjectInitializer const& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "Init")
	void SetWeaponOwner(APawn *NewOwner);

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void SetEnableInteraction(bool bAllowInteraction);

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void ActivateWeapon();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void DeactivateWeapon();

	UFUNCTION(BlueprintCallable, Server, Category = "Attack")
	void StartAttack();

	UFUNCTION(BlueprintCallable, Server, Category = "Attack")
	void StopAttack();

	/* UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Attack")
	bool IsAttackAllowed(UAttackType *InAttackType) const; */

public:

/*	UPROPERTY(BlueprintAssignable, Category = "Event")
	FWeaponOwnerAssignedSignature OnWeaponOwnerSetup;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UAttackType> BaseAttackTypeClass;

	UFUNCTION(BlueprintNativeEvent, Category = "Attack")
	TSubclassOf<class UAttackType> GetNewAttackTypeClass() const; */

protected:

	static FName PrimitiveComponentName;

	static FName MeshComponentName;

	static FName AttackComponentName;

	virtual UAttackType* ChoooseNewAttackType() const;

public:	

	/**
	* Check if this weapon allows to start a new attack with the specified attack type
	*
	* @param AttackType - the attack type we're going to use for this attack
	*
	* @return bool - true if this weapon's ready to start an attack with the attack type
	*/
	virtual bool CanStartAttack(UAttackType *AttackType) const;

	/**
	* Check if this weapon allows to start the next attack simulation with the specified attack type
	*
	* @param AttackType - the attack type we're going to use for this attack simulation
	* @param SimulationCount - the attack type we're going to use for this attack simulation
	*
	* @return bool - true if this weapon's ready to start an attack simulation with the attack type
	*/
	virtual bool CanStartAttackSimulation(UAttackType *AttackType, int32 SimulationCount) const;

	virtual bool AttackAnimationExists(FName AttackTypeName) const;

	virtual void PlayAttackAnimation(FName AttackTypeName);

	virtual void AnimJumpToSection(EAttackAnimation::SectionType SectionType);

	/**
	* Check if headshot is allowed on the incoming actor
	*
	* @param Target - the actor to test on
	*
	* @return bool - true if the actor can take a headshot damage
	*/
	virtual bool IsHeadShotAllowedOnTarget(AActor *Target) const;

	/* AActor Interface */
	virtual void Tick(float DeltaTime) override;
	/* ~AActor Interface */

protected:

	/* AActor Interface */
	virtual void BeginPlay() override;
	/* ~AActor Interface */

private:

/*	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	APawn *WeaponOwner; */

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HeadShot", meta = (AllowPrivateAccess = "true"))
	uint8 bAllowHeadShot : 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Socket", meta = (AllowPrivateAccess = "true"))
	FName EquippedSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Socket", meta = (AllowPrivateAccess = "true"))
	FName UnequippedSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	UAnimMontage *OwnerEquipAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	UAnimMontage *OwnerUnequipAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AimOffset", meta = (AllowPrivateAccess = "true"))
	UAimOffsetBlendSpace *OwnerAimOffset;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UPrimitiveComponent *CollisionMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UMeshComponent *WeaponMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UAttackComponent *AttackComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TMap<FName, UAnimMontage> AttackAnims;

	UPROPERTY()
	class UAttackType *PreviousAttackType;

	UPROPERTY()
	class UAttackType *CurrentAttackType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack", meta = (AllowPrivateAccess = "true"))
	EAttackTypeSelectionMethod AttackTypeSelectionMethod;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack", meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<class UAttackType>> AvailableAttackTypes;

public:

	/* FORCEINLINE APawn* GetWeaponOwner() const { return WeaponOwner; }
	template<typename T>
	FORCEINLINE T* GetWeaponOwner() const { return Cast<T>(WeaponOwner); } */
	FORCEINLINE UPrimitiveComponent* GetCollisionMesh() const { return CollisionMesh; }
	FORCEINLINE UMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
	FORCEINLINE class UAttackComponent* GetAttackComponent() const { return AttackComponent; }
	FORCEINLINE bool IsHeadShotAllowed() const { return bAllowHeadShot; }
	FORCEINLINE FName GetEquippedSocketName() const { return EquippedSocketName; }
	FORCEINLINE FName GetUnequippedSocketName() const { return UnequippedSocketName; }
	FORCEINLINE UAnimMontage *GetOwnerEquipAnimation() const { return OwnerEquipAnimation; }
	FORCEINLINE UAnimMontage *GetOwnerUnequipAnimation() const { return OwnerUnequipAnimation; }
	FORCEINLINE UAimOffsetBlendSpace *GetOwnerAimOffset() const { return OwnerAimOffset; }

private:

//	virtual bool IsAttackAllowed_Implementation(UAttackType *InAttackType) const { return true; }
//	virtual TSubclassOf<class UAttackType> GetBaseAttackTypeClass_Implementation() const;
//	virtual TSubclassOf<class UAttackType> GetNewAttackTypeClass_Implementation() const { return nullptr; }

};

FName AWeapon::PrimitiveComponentName(TEXT("CollisionMesh"));
FName AWeapon::MeshComponentName(TEXT("WeaponMesh"));
FName AWeapon::AttackComponentName(TEXT("AttackComp"));