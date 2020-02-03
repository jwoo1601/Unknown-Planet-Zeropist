// Copyright 2017 Ground Breaking Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimMontage.h"
#include "CameraShake.h"

#include "Weapon/RangedWeapon.h"
#include "Weapon/Reloadable.h"
#include "Weapon/Attack/AttackComponent.h"

#include "GunWeapon.generated.h"

USTRUCT(BlueprintType)
struct CLIENT_API FGunWeaponEventParams
{
	GENERATED_USTRUCT_BODY()

	FGunWeaponEventParams() { }

	FGunWeaponEventParams(APawn *InWeaponOwner, class AGunWeapon *InWeapon) : WeaponOwner(InWeaponOwner), Weapon(InWeapon) { }

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	APawn *WeaponOwner;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class AGunWeapon *Weapon;

};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGunReloadStartedSignature, FGunWeaponEventParams const&, InParams);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGunReloadCompletedSignature, FGunWeaponEventParams const&, InParams);

namespace EGunFireEffect
{
	enum Type
	{
		FE_Flash,
		FE_BulletTrail
	};
}

/**
 * 
 */
UCLASS(abstract, BlueprintType, Blueprintable)
class CLIENT_API AGunWeapon : public ARangedWeapon, public IReloadable
{
	GENERATED_BODY()

	friend class UAnimNotify_ReloadEnd;

public:

	AGunWeapon(FObjectInitializer const& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "Attack")
	void StartFire();

	UFUNCTION(BlueprintCallable, Category = "Attack")
	void StopFire();

public:

	UPROPERTY(BlueprintAssignable, Category = "Reload")
	FGunReloadStartedSignature OnReloadStarted;

	UPROPERTY(BlueprintAssignable, Category = "Reload")
	FGunReloadCompletedSignature OnReloadCompleted;

protected:

	UFUNCTION(BlueprintNativeEvent, Category = "Event", meta = (DisplayName = "ReloadStarted"))
	void Event_OnReloadStarted();

	UFUNCTION(BlueprintNativeEvent, Category = "Event", meta = (DisplayName = "ReloadCompleted"))
	void Event_OnReloadCompleted();

	/* AActor Interface */
	virtual void BeginPlay() override;
	/* ~AActor Interface */

private:

	void NotifyReloadAnimationEnded();

public:

	/* AActor Interface */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	/* ~AActor Interface */

	/* AWeapon Interface */
	virtual bool IsAttackAllowed_Implementation(class UAttackType *InAttackType) const override;
	virtual TSubclassOf<class UAttackType> GetBaseAttackTypeClass_Implementation() const override;
	/* ~AWeapon Interface */

	/* IReloadable Interface */
	UFUNCTION(BlueprintNativeEvent, BlueprintPure, BlueprintCallable, Category = "Reload")
	bool CanReload() const;
	virtual bool CanReload_Implementation() const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Reload")
	void Reload();
	virtual void Reload_Implementation() override;

	virtual void AddReloadStartedEvent(FScriptDelegate const& InDelegate) override;
	virtual void AddReloadCompletedEvent(FScriptDelegate const& InDelegate) override;
	virtual void RemoveReloadStartedEvent(FScriptDelegate const& InDelegate) override;
	virtual void RemoveReloadCompletedEvent(FScriptDelegate const& InDelegate) override;
	/* ~IReloadable Interface */
	
private:

	UPROPERTY(BlueprintReadOnly, Category = "Reload", meta = (AllowPrivateAccess = "true"))
	uint8 bIsReloading : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resource", meta = (AllowPrivateAccess = "true"))
	UTexture *PreviewImage;

	UPROPERTY(BlueprintReadOnly, Category = "Ammo", meta = (AllowPrivateAccess = "true"))
	int32 CurrentAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ammo", meta = (AllowPrivateAccess = "true", UIMin = "1", ClampMin = "1"))
	int32 MaxAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recoil", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UCameraShake> RecoilCameraShake;

	FScriptDelegate ReceiveAttackStartedDelegate;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	UAnimMontage *OwnerReloadAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect", meta = (AllowPrivateAccess = "true"))
	TArray<class UEffect*> FireEffects;

private:

	UFUNCTION()
	void ReceiveAttackStarted(FAttackEventParams const& InParams);

protected:

	virtual void Event_OnReloadStarted_Implementation() { }
	virtual void Event_OnReloadCompleted_Implementation() { }
	
};
