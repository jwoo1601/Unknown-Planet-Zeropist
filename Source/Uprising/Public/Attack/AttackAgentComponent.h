// Copyright 2017 Ground Breaking Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Attack/AttackType.h"
#include "GameTypes.h"
#include "Weapon/Weapon.h"

#include "AttackAgentComponent.generated.h"

USTRUCT(BlueprintType)
struct UPRISING_API FAttackEventParams
{
	GENERATED_USTRUCT_BODY()

public:

	FAttackEventParams() { }

	FAttackEventParams(class UAttackComponent *InComponent, APawn *InInstigator, class AWeapon *InWeapon, class UAttackType *InAttackType) : Component(InComponent), Instigator(InInstigator), Weapon(InWeapon), AttackType(InAttackType) { }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Transient)
	class UAttackComponent *Component;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Transient)
	APawn *Instigator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Transient)
	class AWeapon *Weapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Transient)
	class UAttackType *AttackType;

};

USTRUCT(BlueprintType)
struct UPRISING_API FAttackResult
{
	GENERATED_USTRUCT_BODY()

	FAttackResult() { }

	FAttackResult(TSubclassOf<UDamageType> InDamageTypeClass, FHitResult const& InHitInfo) : bHit(InHitInfo.bBlockingHit && InHitInfo.GetActor()), DamageTypeClass(InDamageTypeClass), HitInfo(InHitInfo) { }

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 bHit : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 bHeadShot : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UDamageType> DamageTypeClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FHitResult HitInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CalculatedDamage;

};

USTRUCT(BlueprintType)
struct UPRISING_API FAttackSimulationResult
{
	GENERATED_USTRUCT_BODY()

	FAttackSimulationResult() { }

	FAttackSimulationResult(bool bInAnyHit, class UAttackType *InAttackType);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 bAnyHit : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UAttackType *AttackType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FAttackResult> AttackResults;

};

UENUM(BlueprintType)
enum class EAttackStatus : uint8
{
	Idle,
	Attacking,
	CoolingDown
};

USTRUCT()
struct FWeaponContext
{
	GENERATED_USTRUCT_BODY()

public:

	FWeaponContext(AWeapon *InWeapon);

	UPROPERTY()
	TWeakObjPtr<class AWeapon> OwningWeapon;

	UPROPERTY()
	TWeakObjPtr<APawn> WeaponOwner;

	bool IsAttackAllowed(UAttackType *AttackType);

	bool IsAttackSimulationAllowed(UAttackType *AttackType, int SimulationCount);

	bool AttackAnimationExists(FName AttackTypeName, EAttackAnimation::Target AnimTarget);

	void PlayAttackAnimationOn(FName AttackTypeName, EAttackAnimation::Target AnimTarget);

	void AnimJumpToSection(EAttackAnimation::SectionType SectionType);

	void OnAttackProcessCompleted();

};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPreAttackSignature, FAttackEventParams const&, InParams);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAttackStartedSignature, FAttackEventParams const&, InParams);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAttackCompletedSignature, FAttackEventParams const&, InParams);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FAttackAnimationStartedSignature, FAttackEventParams const&, InParams, bool, bUseCustomAnimation, UAnimMontage*, InAnimMontage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FAttackAnimationEndedSignature, FAttackEventParams const&, InParams, bool, bUseCustomAnimation, UAnimMontage*, InAnimMontage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FBurstChargedAttackSignature, FAttackEventParams const&, InParams, float, ElapsedTime, float, MaxChargingTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAttackSimulationStartedSignature, FAttackEventParams const&, InParams, int32, SimulationCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAttackSimulationCompletedSignature, FAttackEventParams const&, InParams, FAttackSimulationResult const&, InSimulationResult);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAttackHitSignature, FAttackEventParams const&, InParams, FHitResult const&, InHitInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAttackCooldownResolvedSignature, FAttackEventParams const&, InParams);

/*
* AWeapon::StartAttack -> UAttackComponent::StartAttack -> CanAttack -> PlayAttackAnimation -> NotifyAnimationStarted -> SimulateAttack -> InternalSimulateAttack -> NotifyAnimationEnded -> ApplyAttackDamage -> InternalApplyDamage -> PostAttackProcess -> InternalPostAttackProcess
*/
UCLASS(abstract, Blueprintable, ClassGroup = (Attack), meta = (BlueprintSpawnableComponent))
class UPRISING_API UAttackAgentComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UAttackAgentComponent(const FObjectInitializer &OI);

	UFUNCTION(BlueprintCallable, Category = "Attack|Damage")
	void AddAttackDamage(int32 DeltaDamage);

	UFUNCTION(BlueprintCallable, Category = "Attack|Damage")
	void AddDamageMultiplier(float DeltaMultiplier);

	UFUNCTION(BlueprintCallable, Server, Category = "Attack")
	bool CanStartAttack() const;

	void SetRelevantAttackComponent(class UAttackComponent *NewAttackComponent);

	/* */
	UFUNCTION(BlueprintCallable)
	bool QueryAttackCondition() const;

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void BeginAttack();

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Burst();

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "Attack")
	void StopAttack();

	void OnBeginAttack(EAttackMode Mode);

	// check if new attack simulation can start
	bool CheckForAttackMode(EAttackMode Mode);

	void OnTickAttackMode(EAttackMode Mode);

	void OnSimulationCompleted(EAttackMode Mode);

	void OnStopAttack(EAttackMode Mode);


	UFUNCTION(BlueprintCallable, Category = "Attack|Charge")
	bool IsFullyCharged() const;

	UFUNCTION(BlueprintCallable, Category = "Attack|Simulation")
	void NotifySimulationCompleted(FGameEventHandle const& Handle, FAttackSimulationResult &InResult);

public:

	UPROPERTY(BlueprintAssignable, Category = "Event")
	FPreAttackSignature OnPreAttack;

	UPROPERTY(BlueprintAssignable, Category = "Event")
	FAttackStartedSignature OnAttackStarted;

	UPROPERTY(BlueprintAssignable, Category = "Event")
	FAttackCompletedSignature OnAttackCompleted;

	UPROPERTY(BlueprintAssignable, Category = "Event")
	FAttackAnimationStartedSignature OnAnimationStarted;

	UPROPERTY(BlueprintAssignable, Category = "Event")
	FAttackAnimationEndedSignature OnAnimationEnded;

	UPROPERTY(BlueprintAssignable, Category = "Event")
	FBurstChargedAttackSignature OnBurstChargedAttack;

	UPROPERTY(BlueprintAssignable, Category = "Event")
	FAttackSimulationStartedSignature OnSimulationStarted;

	UPROPERTY(BlueprintAssignable, Category = "Event")
	FAttackSimulationCompletedSignature OnSimulationCompleted;

	UPROPERTY(BlueprintAssignable, Category = "Event")
	FAttackHitSignature OnAttackHit;

	UPROPERTY(BlueprintAssignable, Category = "Event")
	FAttackCooldownResolvedSignature OnAttackCooldownResolved;

protected:

	virtual bool CheckAttackCondition() const;

	UFUNCTION(BlueprintNativeEvent, Category = "Internal", meta = (DisplayName = "CanAttack"))
	bool InternalCanAttack() const;

	UFUNCTION(BlueprintNativeEvent, Category = "Internal|Simulation")
	bool CanStartSimulation() const;

	UFUNCTION(BlueprintNativeEvent, Category = "Internal|Simulation")
	void SimulateAttack(FAttackSimulationHandle InHandle, UAttackType *InType, int32 SimulationCount);

	UFUNCTION(BlueprintNativeEvent, Category = "Internal", meta = (DisplayName = "Apply Attack Damage"))
	void InternalApplyAttackDamage(AActor *Target, FAttackResult const& InResult, AController *InInstigator, AActor *InDamageCauser);

	UFUNCTION(BlueprintNativeEvent, Category = "Internal", meta = (DisplayName = "Post Attack Process"))
	void InternalPostAttackProcess();

	UFUNCTION(BlueprintNativeEvent, Category = "Event", meta = (DisplayName = "PreAttack"))
	void Event_OnPreAttack();
	
	UFUNCTION(BlueprintNativeEvent, Category = "Event", meta = (DisplayName = "AttackStarted"))
	void Event_OnAttackStarted();

	UFUNCTION(BlueprintNativeEvent, Category = "Event", meta = (DisplayName = "AnimationStarted"))
	void Event_OnAnimationStarted(bool bUseCustomAnimation, UAnimMontage* InAnimMontage);

	UFUNCTION(BlueprintNativeEvent, Category = "Event", meta = (DisplayName = "AnimationEnded"))
	void Event_OnAnimationEnded(bool bUseCustomAnimation, UAnimMontage* InAnimMontage);

	UFUNCTION(BlueprintNativeEvent, Category = "Event", meta = (DisplayName = "BurstChargedAttack"))
	void Event_OnBurstChargedAttack(float ElapsedTime, float MaxChargingTime);
	
	UFUNCTION(BlueprintNativeEvent, Category = "Event", meta = (DisplayName = "SimulationStarted"))
	void Event_OnSimulationStarted(int32 SimulationCount);

	UFUNCTION(BlueprintNativeEvent, Category = "Event", meta = (DisplayName = "SimulationCompleted"))
	void Event_OnSimulationCompleted(FAttackSimulationResult const& InSimulationResult);

	UFUNCTION(BlueprintNativeEvent, Category = "Event", meta = (DisplayName = "AttackHit"))
	void Event_OnAttackHit(FHitResult const& InHitInfo);

	UFUNCTION(BlueprintNativeEvent, Category = "Event", meta = (DisplayName = "AttackCooldownResolved"))
	void Event_OnAttackCooldownResolved();


private:
	
	// StartAttack -> PlayAttackAnimation
	// [AnimNotify] NotifyAnimationStarted -> StartAttackProcess
	// [AnimNotify] NotifyStartSimulation {-> if (AttackMode != EAttackMode::Charged) => StartSimulation -> SimulateAttack
	// NotifySimulationCompleted -> ApplyAttackDamage {-> if (!bIsPlayingAnimation) => PostAttackProcess}
	// {if (AttackMode == EAttackMode::Continuos => StopAttack -> StopAttackAnimation -> {if (AttackMode == EAttackMode::Charged) => BurstChargedAttack}}
	// [AnimNotify] NotifyAnimationEnded {-> if (!bIsSimulatingAttack) => PostAttackProcess}

	UFUNCTION(Server)
	bool ServerQueryAttackCondition() const;

	UFUNCTION(NetMulticast, Reliable)
	void BeginAttackProcess();

#if WITH_SERVER_CODE
	void BeginAttackSimulation();
#endif

//	void PlayAttackAnimation();

	UFUNCTION(Client, Reliable)
	void ClientAnimJumpToSection(EAttackAnimation::SectionType SectionType);

	void StopAttackAnimation();

	void StartAttackSimulation(UAttackType *InAttackType);

//	void BurstChargedAttack();

	void ApplyAttackDamage(FAttackSimulationResult &InResult);

	void PostAttackProcess();

/* 	UFUNCTION()
	void HandleAttackCooldownResolved();
	UFUNCTION()	void ReceiveWeaponOwnerSetup(FWeaponEventParams const& InParams); */

	void NotifyAnimationStarted();
	void NotifyJumpToDest();
	void NotifyAnimationEnded();
	void NotifyStartSimulation();

	bool TryJumpToSection(FName const& SectionName);

	FORCEINLINE bool CheckValidState() const;
	FORCEINLINE bool CheckValidAnimState() const;

	void ResetStates();

public:

	// UActorComponent Interface
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	// ~UActorComponent Interface

protected:

	// UActorComponent Interface
	//virtual void OnRegister() override;
	virtual void InitializeComponent() override;
	virtual void UninitializeComponent() override;
	virtual void BeginPlay() override;
	// ~UActorComponent Interface

	virtual void ComputeInitialAttackDamage(class UAttackType *InType, FHitResult const& InHitInfo, int32 &OutDamage) const;

	virtual bool IsHeadShot(FHitResult const& InHitInfo) const;

private:

/*	UPROPERTY(BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	class APawn *AttackInstigator;

	UPROPERTY(BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	class AWeapon *Weapon; */

	FWeaponContext *WeaponContext;

	class UAttackComponent *RelevantAttackComponent;

	UPROPERTY(BlueprintReadOnly, Category = "Attack", meta = (AllowPrivateAccess = "true"))
	EAttackStatus Status;

	UPROPERTY(BlueprintReadOnly, Category = "Attack", meta = (AllowPrivateAccess = "true"))
	class UAttackType *CurrentType;

	UPROPERTY(BlueprintReadOnly, Category = "Time", meta = (AllowPrivateAccess = "true"))
	float ElapsedTime;

	UPROPERTY()
	uint8 bAttackProcessStarted : 1;

	UPROPERTY()
	uint8 bAnimShouldJumpToDest : 1;

	UPROPERTY(BlueprintReadOnly, Category = "Simulation", meta = (AllowPrivateAccess = "true"))
	int32 SimulationCount;

	UPROPERTY()
	class UGameEventManager *EventManager;

	UPROPERTY(BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	uint8 bIsPlayingAnimation : 1;

/*	UPROPERTY(BlueprintReadOnly, Category = "Attack", meta = (AllowPrivateAccess = "true"))
	EAttackMode Mode;

	UPROPERTY(Transient)
	uint32 LastAssignedSimulationHandle;
	
	UPROPERTY()
	TMap<FAttackSimulationHandle, float> PendingSimulationHandleMap;
	
	UPROPERTY(BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	UAnimInstance *CurrentAnimInstance;

	UPROPERTY(BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	UAnimMontage *CurrentAnimMontage; */
	
protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Simulation", meta = (AllowPrivateAccess = "true"))
	float SimulationTimeOut;

	UPROPERTY()
	int32 PendingDamageToApply;

	UPROPERTY()
	float PendingDamageMultiplierToApply;

public:

//	FORCEINLINE APawn* GetAttackInstigator() const;
//	FORCEINLINE class IWeaponOwnerInterface* GetWeaponOwner() const;
//	FORCEINLINE class AWeapon* GetWeapon() const;
	FORCEINLINE FWeaponContext* GetWeaponContext() const { return WeaponContext; }
	FORCEINLINE EAttackStatus GetStatus() const { return Status; }
	FORCEINLINE class UAttackType* GetCurrentAttackType() const;
	FORCEINLINE int32 GetSimulationCount() const { return SimulationCount; }
	FORCEINLINE bool IsPlayingAnimation() const { return bIsPlayingAnimation; }
	FORCEINLINE float GetAttackElapsedTime() const { return ElapsedTime; }
//	FORCEINLINE float GetSimulationTimeOut() const { return SimulationTimeOut; }

private:

	FTimerHandle AttackCooldownTimerHandle;

protected:

	virtual int32 CalculateInitialAttackDamage_Implementation(UAttackType *InType, FHitResult const& InHitInfo) const;
	virtual bool IsHeadShot_Implementation(FHitResult const& InHitInfo) const;

	virtual bool InternalCanAttack_Implementation() const { return true; }
	virtual bool CanStartSimulation_Implementation() const { return true; }
	virtual void SimulateAttack_Implementation(FAttackSimulationHandle InHandle, UAttackType *InType, int32 SimulationCount) { }
	virtual void InternalApplyAttackDamage_Implementation(AActor *Target, FAttackResult const& InResult, AController *InInstigator, AActor *InDamageCauser);
	virtual void InternalPostAttackProcess_Implementation() { }

	virtual void Event_OnPreAttack_Implementation() { }
	virtual void Event_OnAttackStarted_Implementation() { }
	virtual void Event_OnAnimationStarted_Implementation(bool bUseCustomAnimation, UAnimMontage* InAnimMontage) { }
	virtual void Event_OnAnimationEnded_Implementation(bool bUseCustomAnimation, UAnimMontage* InAnimMontage) { }
	virtual void Event_OnBurstChargedAttack_Implementation(float ElapsedTime, float MaxChargingTime) { }
	virtual void Event_OnSimulationStarted_Implementation(int32 SimulationCount) { }
	virtual void Event_OnSimulationCompleted_Implementation(FAttackSimulationResult const& InSimulationResult) { }
	virtual void Event_OnAttackHit_Implementation(FHitResult const& InHitInfo) { }
	virtual void Event_OnAttackCooldownResolved_Implementation() { }

/*	UFUNCTION()
	void HandlePendingDamageData(); */

//	FTimerHandle AttackDamageTimerHandle;

};
