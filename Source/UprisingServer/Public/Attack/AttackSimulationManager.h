// Copyright 2017 Ground Breaking Games. All Rights Reserved.

#include "CoreMinimal.h"
#include "WorldCollision.h"

#include "Log.h"
#include "ServerTypes.h"
#include "Event/GameEventManager.h"

#include "AttackSimulationManager.generated.h"

UENUM(BlueprintType)
enum class EAttackSimulationMode : uint8
{
	RayTrace,
	PrimitiveTrace,
	Projectile,

	// Currently Not Supported
	PhysicsBased
};

UCLASS(Blueprintable, BlueprintType)
class UPRISINGSERVER_API UAttackSimulationManager : public UObject, public FTickableGameObject
{
	GENERATED_BODY()

public:

	UAttackSimulationManager(FObjectInitializer const& OI);

	/**
	* Starts an attack simulation with the given attack simulation mode
	*
	* @param Mode - attack simulation mode to use
	* @param Params - parameters to use for a new attack simulation
	*
	* @return FGameEventHandle - a simulation handle used to identify this attack simulation (invalid if there's any problem starting an attack simulation) 
	*/
	UFUNCTION(BlueprintCallable)
	FGameEventHandle StartSimulation(EAttackSimulationMode Mode, FAttackSimulationParams const& InParams, float SimulationTimeOut = -1.0f);

	/**
	* Starts an attack simulation with the given attack simulation mode
	*
	* @param Mode - attack simulation mode to use
	* @param Params - parameters to use for a new attack simulation
	*
	* @return FGameEventHandle - a simulation handle represents this attack simulation (invalid if there's any problem starting an attack simulation)
	*/
	UFUNCTION(BlueprintCallable)
	bool StopSimulation(FGameEventHandle const& InSimulationHandle);

	/**
	* Starts an attack simulation with the given attack simulation mode
	*
	* @param InSimulationHandle - attack simulation mode to use
	* @param OutInfo - parameters to use for a new attack simulation
	*
	*/
	UFUNCTION(BlueprintCallable)
	bool QuerySimulationInfo(FGameEventHandle const& InSimulationHandle, FAttackSimulationInfo &OutInfo) const;

	/**
	* Sets the specified damage processor as a new damage processor for this
	*
	* @param InProcessor - a damage processor used for applying damages for every single attack
	*
	*/
	UFUNCTION(BlueprintSetter)
	void SetDamageProcessor(class UAttackDamageProcessor *InProcessor);

	UFUNCTION(BlueprintGetter)
	class UAttackDamageProcessor* GetDamageProcessor() const;

	UFUNCTION(BlueprintCallable)
	bool IsSimulationPaused(FGameEventHandle const& InSimulationHandle) const;

	UFUNCTION(BlueprintCallable)
	void Pause(FGameEventHandle const& InSimulationHandle);

	UFUNCTION(BlueprintCallable)
	void PauseAll();

	UFUNCTION(BlueprintCallable)
	void Unpause(FGameEventHandle const& InSimulationHandle);

	UFUNCTION(BlueprintCallable)
	void UnpauseAll();

protected:

	UFUNCTION(BlueprintNativeEvent)
	void ReceiveSimulationCompleted(FAttackSimulationInfo const& SimulationInfo, FAttackSimulationResult const& SimulationResult);
	virtual void ReceiveSimulationCompleted_Implementation(FAttackSimulationInfo const& SimulationInfo, FAttackSimulationResult const& SimulationResult) { }

	UFUNCTION(BlueprintNativeEvent)
	void ReceiveSimulationFailed(ESimulationFailureReason::Type FailureReason, FAttackSimulationInfo const& SimulationInfo);
	virtual void ReceiveSimulationFailed_Implementation(ESimulationFailureReason::Type FailureReason, FAttackSimulationInfo const& SimulationInfo) { }

	UFUNCTION(BlueprintNativeEvent)
	void ReceiveSimulationPaused(FGameEventHandle const& InSimulationHandle, bool bPaused);
	virtual void ReceiveSimulationPaused_Implementation(FGameEventHandle const& InSimulationHandle, bool bPaused) { }

	UFUNCTION(BlueprintNativeEvent)
	void ReceiveAllSimulationPaused(bool bPaused);
	virtual void ReceiveAllSimulationPaused_Implementation(bool bPaused) { }

protected:

	/**
	* Notifies that a queued simulation has been completed
	*
	* @param InSimulationHandle - the simulation handle used to identify this attack simulation
	* @param InTotalHits - parameters to use for a new attack simulation
	*
	*/
	UFUNCTION(BlueprintCallable)
	void NotifySimulationCompleted(FGameEventHandle &InSimulationHandle, TArray<FHitResult> const& InTotalHits);

	UPROPERTY()
	virtual void PostSimulationCompleted(FGameEventHandle &InSimulationHandle, float SimulationTime);

	UFUNCTION(BlueprintCallable)
	void NotifySimulationFailed(FGameEventHandle &InSimulationHandle, ESimulationFailureReason::Type FailureReason);

	virtual void ConvertSimulationQueryParams(FAttackSimulationParams const& InSimulationParams, FCollisionObjectQueryParams &OutObjQueryParams, FCollisionQueryParams &OutQueryParams);

	UFUNCTION()
	virtual void ReceiveProjectileHit(AActor *Projectile, AActor *HitActor, FVector NormalImpulse, FHitResult const& Hit);

	UFUNCTION()
	virtual void ReceiveSimulationCallback(FGameEventHandle &InSimulationHandle, bool bCompleted, float ElapsedTime);

protected:

	virtual void PostSimulationProcess(FAttackSimulationInfo &InSimulationInfo, FAttackSimulationResult &InSimulationResult);

	virtual void StartRayTraceSimulation(FGameEventHandle &InSimulationHandle, FAttackSimulationParams const& InParams);

	virtual void StartPrimitiveTraceSimulation(FGameEventHandle &InSimulationHandle, FAttackSimulationParams const& InParams);

	virtual void StartProjectileSimulation(FGameEventHandle &InSimulationHandle, FAttackSimulationParams const& InParams);

	virtual void StartPhysicsBasedSimulation(FGameEventHandle &InSimulationHandle, FAttackSimulationParams const& InParams);

	/* FTickableGameObject Interface */
	virtual void Tick(float DeltaTime) override;
	virtual bool IsTickable() const override;
	virtual TStatId GetStatId() const override;
	virtual UWorld* GetTickableGameObjectWorld() const override;
	/* ~FTickableGameObject Interface */

private:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<ECollisionChannel> DamageableTypes;

	UPROPERTY(EditDefaultsOnly, Instanced)
	UGameEventManager *SimulationEventManager;

	UPROPERTY(BlueprintSetter=SetDamageProcessor)
	class UAttackDamageProcessor *DamageProcessor;

	UPROPERTY(Transient)
	TMap<FGameEventHandle, FAttackSimulationInfo> StoredSimulationInfo;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	uint8 bPaused : 1;
	
public:

	FORCEINLINE bool IsPaused() const { return bPaused; }

};