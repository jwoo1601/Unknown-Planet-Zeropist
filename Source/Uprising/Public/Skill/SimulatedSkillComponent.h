// Copyright 2017 Ground Breaking Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "SkillComponent.h"

#include "SimulatedSkillComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPreSimulateSkillSignature, FSkillEventParams const&, InParams);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPostSimulateSkillSignature, FSkillEventParams const&, InParams);

UCLASS(abstract, Blueprintable, ClassGroup=(Skill))
class CLIENT_API USimulatedSkillComponent : public USkillComponent
{
	GENERATED_BODY()

public:

	USimulatedSkillComponent(FObjectInitializer const& ObjectInitializer);
	
	UFUNCTION(BlueprintCallable, BlueprintGetter, Category = "Simulation")
	bool IsSimulating() const;

	UFUNCTION(BlueprintCallable, Category = "Simulation")
	void NotifySimulationCompleted(FGameEventHandle const& InHandle);
	
public:

	UPROPERTY(BlueprintAssignable, Category = "Event")
	FPreSimulateSkillSignature OnPreSimulate;

	UPROPERTY(BlueprintAssignable, Category = "Event")
	FPostSimulateSkillSignature OnPostSimulate;

protected:

	UFUNCTION(BlueprintNativeEvent, Category = "Simulation")
	bool CanStartSimulation() const;

	UFUNCTION(BlueprintNativeEvent, Category = "Simulation")
	void SimulateSkill(struct FGameEventHandle const& InHandle, int32 SimulationCount);

	UFUNCTION(BlueprintNativeEvent, Category = "Simulation")
	void StopSimulation(bool &bOutCanceled, FSkillUseResult &InOutResult);

	UFUNCTION(BlueprintNativeEvent, Category = "Simulation")
	void PostSimulationProcess(FSkillUseResult &InOutResult);


	UFUNCTION(BlueprintNativeEvent, Category = "Event|Simulation", meta = (DisplayName = "OnPreSimulateSkill"))
	void Event_OnPreSimulate();

private:

	UPROPERTY(BlueprintReadOnly, Category = "Content", meta = (AllowPrivateAccess = "true"))
	class USimulatedSkillContent *SimulatedSkillContent;

	UPROPERTY()
	struct FGameEventTrigger SimulationTrigger;

	UPROPERTY()
	class UGameEventManager *EventManager;

	UPROPERTY(BlueprintGetter = IsSimulating)
	uint8 bSimulating : 1;

	UPROPERTY()
	uint8 bShouldStartSimulation : 1;

	UPROPERTY()
	uint8 bTickSimulationEnded : 1;

	UPROPERTY()
	int32 SimulationCount;

	UPROPERTY()
	float SimulationStartTime;

	UPROPERTY()
	float LastSimulationTime;

public:

	/* UActorComponent Interface */
	virtual void InitializeComponent() override;
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	/* ~UActorComponent Interface */

protected:

	/* UActorComponent Interface */
	virtual void OnRegister() override;
	virtual void OnUnregister() override;
	/* ~UActorComponent Interface */

	/* USkillComponent Interface */
	virtual void StartSkillProcess_Implementation() override;
	virtual void CancelSkillProcess_Implementation(bool &bOutCanceled, FSkillUseResult &InOutResult) override;
	virtual void InternalResetProperties_Implementation() override;
	/* ~USkillComponent Interface */

private:

	void StartSimulation(struct FGameEventTrigger const& InTrigger);

	UFUNCTION()
	void ProcessSimulationCompleted(FGameEventHandle const& InHandle);

public:

	FORCEINLINE class USimulatedSkillContent* GetSimulatedSkillContent() const { return SimulatedSkillContent; }

private:

	virtual bool CanStartSimulation_Implementation() { return true; }
	virtual void SimulateSkill_Implementation(struct FGameEventHandle const& InHandle, int32 SimulationCount) { }

	virtual void Event_OnPreSimulate_Implementation() { }

};
