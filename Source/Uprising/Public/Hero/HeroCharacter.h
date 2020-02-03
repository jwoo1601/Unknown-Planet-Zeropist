// Copyright 2017 Ground Breaking Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/SphereComponent.h"

#include "Attack/AttackComponent.h"
#include "Event/ActorEvents.h"
#include "Event/GameEventTrigger.h"
#include "Skill/SkillComponent.h"
#include "Skill/SkillOwnerInterface.h"
#include "Weapon/GunWeapon.h"
#include "Weapon/WeaponOwnerInterface.h"

#include "HeroCharacter.generated.h"

UENUM()
enum class EHeroStats : uint8
{
	Invalid = 0,

	HP = 2,
	Shield,

	XP,
	Level
};

USTRUCT()
struct FHeroStatsRestorationData
{
	GENERATED_USTRUCT_BODY()

public:

	FHeroStatsRestorationData() : Type(EHeroStats::Invalid) { }

	FHeroStatsRestorationData(EHeroStats InType, uint32 InRestorationCount, int32 InRestorationAmount) : Type(InType), RestorationCount(InRestorationCount), RestorationAmount(InRestorationAmount) { }

	UPROPERTY()
	EHeroStats Type;

	UPROPERTY()
	FTimerHandle TimerHandle;

/*	UPROPERTY()
	uint8 bDelayResolved : 1;

	UPROPERTY()
	int32 Delay; */

	UPROPERTY()
	uint32 RestorationCount;

	UPROPERTY()
	int32 RestorationAmount;

/*	FORCEINLINE void DecreaseDelay(int32 Amount = 1)
	{
		if (!bDelayResolved && Delay > 0)
		{
			Delay -= Amount;
			if (Delay < 0)
			{
				Delay = 0;
			}

			if (Delay == 0)
			{
				bDelayResolved = true;
			}
		}
	} */
	
	FORCEINLINE void Count()
	{
		if (Type != EHeroStats::Invalid && RestorationCount > 0)
		{
			--RestorationCount;
			if (RestorationCount == 0)
			{
				Type = EHeroStats::Invalid;
			}
		}
	}

};

UENUM(BlueprintType)
enum class ECharacterStatus : uint8
{
	Idle,
	Crouching,
	Jumping,
	UsingSkill,
	Attacking,
	Equipping,
	Reloading,
};

UENUM()
enum ESkillIndex
{
	HERO1 = 0,
	HERO2 = 1,
	WEAPON = 2,
	CLASS = 3,
	ULTIMATE = 4,
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHeroXPChangedSignature, FActorXPChangedEvent const&, InEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHeroLevelUpSignature, FActorLevelUpEvent const&, InEvent);

UCLASS(BlueprintType, Blueprintable)
class CLIENT_API AHeroCharacter : public ACharacter, public IWeaponOwnerInterface, public ISkillOwnerInterface
{
	GENERATED_BODY()

public:

	AHeroCharacter(const FObjectInitializer &ObjectInitializer);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Collision")
	bool IsHeadVolume(UPrimitiveComponent *InComponent) const;
	virtual bool IsHeadVolume_Implementation(UPrimitiveComponent *InComponent) const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void EquipMainWeapon();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void EquipSubWeapon();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void UnequipWeapon();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void SwapEquippedWeapon();

/*	UFUNCTION(BlueprintCallable, Category = "Stats|HP")
	void StartRestoreHP(int32 TotalHPAmount, float InDuration, float InFirstDelay=-1.0f); 

	UFUNCTION(BlueprintCallable, Category = "Stats|HP")
	void RestoreHP(int32 HPAmount, float InFirstDelay=-1.0f); */

	UFUNCTION(BlueprintCallable, Category = "Stats|HP")
	void SetMaxHP(int32 NewHP);

	UFUNCTION(BlueprintCallable, Category = "Stats|HP")
	void SetCurrentHP(int32 NewHP);

	UFUNCTION(BlueprintCallable, Category = "Stats|HP")
	void AddCurrentHP(int32 DeltaHP);
	
	UFUNCTION(BlueprintCallable, Category = "Stats|Shield")
	void SetMaxShield(int32 NewShield);

	UFUNCTION(BlueprintCallable, Category = "Stats|Shield")
	void SetCurrentShield(int32 NewShield);

	UFUNCTION(BlueprintCallable, Category = "Stats|Shield")
	void AddCurrentShield(int32 DeltaShield);

	/**
	* Increase the hero's level by 1.
	*/
/*	UFUNCTION(BlueprintCallable, Category = "Stats|Level")
	void LevelUp();

	UFUNCTION(BlueprintCallable, Category = "Stats|Level")
	void SetLevel(int32 NewLevel);

	UFUNCTION(BlueprintCallable, Category = "Stats|XP")
	void SetXP(int32 NewXP);

	UFUNCTION(BlueprintCallable, Category = "Stats|XP")
	void AddXP(int32 DeltaXP); */

	UFUNCTION(BlueprintCallable, Category = "Skill")
	FORCEINLINE class USkillComponent* GetSkillComponent(ESkillIndex InIndex) const;


	UPROPERTY(BlueprintAssignable, Category = "Event|Stats")
	FHeroXPChangedSignature OnHeroXPChanged;

	UPROPERTY(BlueprintAssignable, Category = "Event|Stats")
	FHeroLevelUpSignature OnHeroLevelUp;

protected:

	UFUNCTION(BlueprintCallable, Category = "Damage")
	int32 ApplyDamage(int32 InitialDamage, FDamageEvent const& DamageEvent);

	UFUNCTION(BlueprintNativeEvent, Category = "Event", meta = (DisplayName = "HeroXPChanged"))
	void Event_OnHeroXPChanged(const FActorXPChangedEvent &InEvent);

	UFUNCTION(BlueprintNativeEvent, Category = "Event", meta = (DisplayName = "HeroLevelUp"))
	void Event_OnHeroLevelUp(const FActorLevelUpEvent &InEvent);

private:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USphereComponent *HeadVolume;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent *CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent *ViewCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Asset", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AHeroContent> ContentClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Asset", meta = (AllowPrivateAccess = "true"))
	class AHeroContent *HeroContent;

	UPROPERTY(BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
//	UChildActorComponent *MainWeaponProxy;
	class AWeapon *MainWeapon;

	UPROPERTY(BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
//	UChildActorComponent *SubWeaponProxy;
	class AWeapon *SubWeapon;

	UPROPERTY(BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	class AWeapon *PreviousWeapon;

	UPROPERTY()
	class AWeapon *WeaponToEquip;

	UPROPERTY(BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	class AWeapon *EquippedWeapon;

	UPROPERTY(BlueprintReadOnly, Transient, Category = "Skill", meta = (AllowPrivateAccess = "true"))
	TArray<class USkillComponent*> RegisteredSkills;

	UPROPERTY(BlueprintReadOnly, Transient, Category = "Skill", meta = (AllowPrivateAccess = "true"))
	class USkillComponent *SkillInUse;


	UPROPERTY(BlueprintReadOnly, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	uint8 bCombatMode : 1;

	UPROPERTY(BlueprintReadOnly, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	uint8 bFreeLookMode : 1;

	UPROPERTY(BlueprintReadOnly, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	ECharacterStatus CharacterStatus;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	int32 MaxHP;

	UPROPERTY(BlueprintReadOnly, Transient, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	int32 CurrentHP;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	int32 MaxShield;

	UPROPERTY(BlueprintReadOnly, Transient, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	int32 CurrentShield;

	UPROPERTY(BlueprintReadOnly, Transient, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	int32 CurrentLevel;

	UPROPERTY(BlueprintReadOnly, Transient, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	int32 CurrentXP;


	UPROPERTY(BlueprintReadOnly, Transient, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	uint8 bSwappingWeapon : 1;

	UPROPERTY()
	uint8 bAimOffsetRotation : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Transient, Category = "Jump", meta = (AllowPrivateAccess = "true"))
	uint8 bJumpInput : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Transient, Category = "Crouch", meta = (AllowPrivateAccess = "true"))
	uint8 bCrouchInput : 1;


	FScriptDelegate SkillUseStartedDelegate;
	FScriptDelegate SkillUseCanceledDelegate;
	FScriptDelegate SkillUseCompletedDelegate;
	FScriptDelegate WeaponAttackStartedDelegate;
	FScriptDelegate WeaponAttackCooldownResolvedDelegate;
	FScriptDelegate WeaponReloadStartedDelegate;
	FScriptDelegate WeaponReloadCompletedDelegate;

public:

	void HandleMoveInput_Forward(float InValue);
	void HandleMoveInput_Right(float InValue);

	void HandleSprintInput_Pressed();
	void HandleSprintInput_Released();

	void HandleJumpInput_Pressed();
	void HandleJumpInput_Released();

	void HandleCrouchInput_Pressed();
	void HandleCrouchInput_Released();

	void HandleCameraInput_Yaw(float InValue);
	void HandleCameraInput_Pitch(float InValue);

	void HandleAttackInput_Pressed();
	void HandleAttackInput_Released();

	void HandleReloadInput();

	void HandleHeroSkill1UseInput();
	void HandleHeroSkill2UseInput();
	void HandleWeaponSkillUseInput();
	void HandleClassSkillUseInput();
	void HandleUltimateSkillUseInput();

	void HandleEquipWeaponInput();
	void HandleSwapWeaponInput();

	void HandleFreeLookInput_Pressed();
	void HandleFreeLookInput_Released();

	static TArray<FName, TFixedAllocator<5>> RegisteredSkillNameArray;

	enum MiscSkillInfo { NUM_TOTAL_SKILLS = 5 };

protected:

	virtual void InitializeCharacterStats();

	/** This function does not update any physical state because it's supposed to be called during initialization process */
	virtual void ApplyContentData(class AHeroContent* InContent);
	virtual void ApplyMovementAttribute(struct FHeroMovementAttribute const &InAttribute);

	virtual void EquipWeapon_Impl(AWeapon *WeaponToEquip);
	virtual void UseSkill_Impl(USkillComponent *InTargetSkill);

	virtual bool CanUseSkill(USkillComponent *InSkill) const;
	virtual bool CanAttackWithWeapon(AWeapon *InWeapon) const;
	virtual bool CanReloadWeapon(IReloadable *InReloadableWeapon) const;
	virtual bool CanEquipWeapon(AWeapon *InWeapon) const;
	virtual bool CanUnequipWeapon(AWeapon *InWeapon) const;

	virtual void TickStats(float DeltaTime) { }
	
private:

	UChildActorComponent* ConstructWeaponProxyComponent(TSubclassOf<AWeapon> InWeaponClass, FName Name);
	USkillComponent* ConstructSkillComponent(TSubclassOf<USkillComponent> InAssetClass, FName Name);
	AWeapon* ConstructWeaponActor(TSubclassOf<AWeapon> InWeaponClass, FName Name);

	UFUNCTION() void ReceiveSkillUseStarted(FSkillEventParams const& InParams);
	UFUNCTION()	void ReceiveSkillUseCanceled(FSkillEventParams const &InParams, FSkillUseResult const& InResult);
	UFUNCTION()	void ReceiveSkillUseCompleted(FSkillEventParams const &InParams, FSkillUseResult const& InResult);
	UFUNCTION() void ReceiveWeaponAttackStarted(FAttackEventParams const& InParams);
	UFUNCTION() void ReceiveWeaponAttackCooldownResolved(FAttackEventParams const& InParams);
	UFUNCTION() void ReceiveWeaponReloadStarted(FGunWeaponEventParams const& InParams);
	UFUNCTION() void ReceiveWeaponReloadCompleted(FGunWeaponEventParams const& InParams);
	UFUNCTION() void ReceiveEquipWeapon(FGameEventTrigger const& InTrigger);
	UFUNCTION()	void ReceiveEquipAnimationEnd(FGameEventTrigger const& InTrigger);
	UFUNCTION() void ReceiveUnequipWeapon(FGameEventTrigger const& InTrigger);
	UFUNCTION()	void ReceiveUnequipAnimationEnd(FGameEventTrigger const& InTrigger);

	void SwapWeapon_Unequip();
	void SwapWeapon_Equip();

public:

	/* IWeaponOwner Interface */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Attack|Animation")
	UAnimInstance* GetCharacterAnimInstance() const;
	virtual UAnimInstance* GetCharacterAnimInstance_Implementation() const override;

	UFUNCTION(BlueprintNativeEvent, Category = "Attack|Animation")
	void PlayCustomAttackAnimation(class UAttackType *InType);
	virtual void PlayCustomAttackAnimation_Implementation(class UAttackType *InType) override;

	UFUNCTION(BlueprintNativeEvent, Category = "Attack|Animation")
	void StopCustomAttackAnimation();
	virtual void StopCustomAttackAnimation_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, Category = "Attack|Animation")
	void PlayCustomBurstAttackAnimation(class UAttackType *InType);
	virtual void PlayCustomBurstAttackAnimation_Implementation(class UAttackType *InType) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Weapon")
	class AWeapon* GetWeapon() const;
	virtual class AWeapon* GetWeapon_Implementation() const override;
	/* ~IWeaponOwner Interface */

	/* ISkillOwner Interface */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Level")
	int32 GetOwnerLevel() const;
	virtual int32 GetOwnerLevel_Implementation() const override;

	virtual void AddLevelUpEvent(FScriptDelegate const& InDelegate) override;
	virtual void RemoveLevelUpEvent(FScriptDelegate const& InDelegate) override;
	/* ~ISkillOwner Interface*/

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent &e) override;
#endif

protected:

	/* AActor Interface */
	virtual void BeginPlay() override;
	virtual void PostInitProperties() override;
//	virtual float InternalTakeRadialDamage(float Damage, struct FRadialDamageEvent const& RadialDamageEvent, class AController* EventInstigator, AActor* DamageCauser);
//	virtual float InternalTakePointDamage(float Damage, struct FPointDamageEvent const& RadialDamageEvent, class AController* EventInstigator, AActor* DamageCauser);
	/* ~AActor Interface */

public:

	FORCEINLINE USphereComponent* GetHeadVolume() const { return HeadVolume; }
	FORCEINLINE class AHeroContent* GetHeroContent() const { return HeroContent; }
	FORCEINLINE USpringArmComponent* GetSpringArmComponent() const { return CameraBoom; }
	FORCEINLINE UCameraComponent* GetCameraComponent() const { return ViewCamera; }
	FORCEINLINE class AWeapon* GetMainWeapon() const { return MainWeapon; } //{ return MainWeaponProxy ? Cast<AWeapon>(MainWeaponProxy->GetChildActor()) : nullptr; }
	FORCEINLINE class AWeapon* GetSubWeapon() const { return SubWeapon; } //{ return SubWeaponProxy ? Cast<AWeapon>(SubWeaponProxy->GetChildActor()) : nullptr; }
	FORCEINLINE class AWeapon* GetEquippedWeapon() const { return EquippedWeapon; } //{ return EquippedWeapon.Get(); }
	FORCEINLINE class USkillComponent* GetHeroSkill1() const { return RegisteredSkills.IsValidIndex(HERO1) ? RegisteredSkills[HERO1] : nullptr; }
	FORCEINLINE class USkillComponent* GetHeroSkill2() const { return RegisteredSkills.IsValidIndex(HERO2) ? RegisteredSkills[HERO2] : nullptr; }
	FORCEINLINE class USkillComponent* GetWeaponSkill() const { return RegisteredSkills.IsValidIndex(WEAPON) ? RegisteredSkills[WEAPON] : nullptr; }
	FORCEINLINE class USkillComponent* GetClassSkill() const { return RegisteredSkills.IsValidIndex(CLASS) ? RegisteredSkills[CLASS] : nullptr; }
	FORCEINLINE class USkillComponent* GetUltimateSkill() const { return RegisteredSkills.IsValidIndex(ULTIMATE) ? RegisteredSkills[ULTIMATE] : nullptr; }
	FORCEINLINE int32 GetMaxHP() const { return MaxHP; }
	FORCEINLINE int32 GetCurrentHP() const { return CurrentHP; }
	FORCEINLINE int32 GetMaxShield() const { return MaxShield; }
	FORCEINLINE int32 GetCurrentShield() const { return CurrentShield; }
	FORCEINLINE int32 GetCurrentXP() const { return CurrentXP; }
	FORCEINLINE int32 GetCurrentLevel() const { return CurrentLevel; }

protected:

	virtual void Event_OnHeroXPChanged_Implementation(const FActorXPChangedEvent &InEvent) { }
	virtual void Event_OnHeroLevelUp_Implementation(const FActorLevelUpEvent &InEvent) { }
	
};
