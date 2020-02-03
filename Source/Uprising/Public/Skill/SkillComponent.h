// Copyright 2017 Ground Breaking Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Event/ActorEvents.h"

#include "SkillComponent.generated.h"

USTRUCT(BlueprintType)
struct CLIENT_API FSkillEventParams
{
	GENERATED_BODY()

public:

	FSkillEventParams() { }

	FSkillEventParams(class APawn *InOwner, class USkillComponent *InComponent, class USkillContent *InContent) : Owner(InOwner) { }

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class APawn *Owner;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USkillComponent *Component;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USkillContent *Content;

};

USTRUCT(BlueprintType)
struct CLIENT_API FSkillUseResult
{
	GENERATED_USTRUCT_BODY()

	FSkillUseResult() { }

	FSkillUseResult(bool bInSucceeded, TArray<AActor*> const& InTargets) : bSucceeded(bInSucceeded), Targets(InTargets) { }

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 bSucceeded : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 bSkipCooldown : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AActor*> Targets;

	/* UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UProperty *Payload; */

};

UENUM(BlueprintType)
enum class ESkillStatus : uint8
{
	// State_Unbound UMETA(DisplayName = "Unbound"),
	Idle,
	Using,
	CoolingDown
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSkillUseStartedSignature, FSkillEventParams const&, InParams);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSkillUseCanceledSignature, FSkillEventParams const&, InParams, FSkillUseResult const&, InResult);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSkillUseCompletedSignature, FSkillEventParams const&, InParams, FSkillUseResult const&, InResult);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInterceptedAttackInputSignature, FSkillEventParams const&, InParams, EInputEvent, InputType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSkillCooldownResolvedSignature, FSkillEventParams const&, InParams);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSkillPassiveEffectAppliedSignature, FSkillEventParams const&, InParams);

/** Derived class should call NotifyUseCompleted()*/
UCLASS(Blueprintable, ClassGroup=(Skill), meta=(BlueprintSpawnableComponent) )
class UPRISING_API USkillComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	USkillComponent(const FObjectInitializer &ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "Usage")
	FORCEINLINE bool IsUsable() const;

	UFUNCTION(BlueprintCallable, Category = "Usage")
	FORCEINLINE bool CanUse() const;

	UFUNCTION(BlueprintCallable, Category = "Usage")
	void StartUse();

	UFUNCTION(BlueprintCallable, Category = "Usage")
	void StopUse();

	// Resets current cooldown
	UFUNCTION(BlueprintCallable, Category = "Cooldown")
	void ResetCooldown();

public:

	UPROPERTY(BlueprintAssignable, Category = "Event|Usage")
	FSkillUseStartedSignature OnUseStarted;

	UPROPERTY(BlueprintAssignable, Category = "Event|Usage")
	FSkillUseCanceledSignature OnUseCanceled;

	UPROPERTY(BlueprintAssignable, Category = "Event|Usage")
	FSkillUseCompletedSignature OnUseCompleted;

	UPROPERTY(BlueprintAssignable, Category = "Event|Input")
	FInterceptedAttackInputSignature OnInterceptedAttackInput;
	
	UPROPERTY(BlueprintAssignable, Category = "Event|Tick")
	FSkillCooldownResolvedSignature OnCooldownResolved;

	UPROPERTY(BlueprintAssignable, Category = "Event|Passive")
	FSkillPassiveEffectAppliedSignature OnPassiveEffectApplied;

protected:

	UFUNCTION(BlueprintCallable, Category = "Misc")
	void NotifyUseCompleted(const FSkillUseResult &InResult);

	UFUNCTION(BlueprintNativeEvent, Category = "Internal", meta = (DisplayName = "CanUse"))
	bool InternalCanUse() const;

	UFUNCTION(BlueprintNativeEvent, Category = "Internal")
	void StartSkillProcess();

	UFUNCTION(BlueprintNativeEvent, Category = "Internal")
	void CancelSkillProcess(bool &bOutCanceled, FSkillUseResult &InOutResult);

	UFUNCTION(BlueprintNativeEvent, Category = "Internal", meta = (DisplayName = "PostSkillProcess"))
	void InternalPostSkillProcess(FSkillUseResult const& InResult);

	UFUNCTION(BlueprintNativeEvent, Category = "Internal", meta = (DisplayName = "ResetProperties"))
	void InternalResetProperties();

	UFUNCTION(BlueprintNativeEvent, Category = "Internal")
	void HandleInterceptedAttackInput(EInputEvent InputType);

	UFUNCTION(BlueprintNativeEvent, Category = "Passive")
	void TickPassiveEffect(float DeltaTime);


	UFUNCTION(BlueprintNativeEvent, Category = "Event", meta = (DisplayName = "UseStarted"))
	void Event_OnUseStarted();

	UFUNCTION(BlueprintNativeEvent, Category = "Event", meta = (DisplayName = "UseCanceled"))
	void Event_OnUseCanceled(FSkillUseResult const& InResult);

	UFUNCTION(BlueprintNativeEvent, Category = "Event", meta = (DisplayName = "UseCompleted"))
	void Event_OnUseCompleted(FSkillUseResult const& InResult);

	UFUNCTION(BlueprintNativeEvent, Category = "Event", meta = (DisplayName = "CooldownResolved"))
	void Event_OnCooldownResolved();

	UFUNCTION(BlueprintNativeEvent, Category = "Event", meta = (DisplayName = "PassiveEffectApplied"))
	void Event_OnPassiveEffectApplied();

	UFUNCTION(BlueprintNativeEvent, Category = "Event")
	void OnSkillActivated();

	UFUNCTION(BlueprintNativeEvent, Category = "Event")
	void OnSkillDeactivated();

private:
	
	UPROPERTY(BlueprintReadOnly, Category = "Owner", meta = (AllowPrivateAccess = "true"))
	class APawn *PawnOwner;

	UPROPERTY(EditAnywhere, Category = "Content", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class USkillContent> SkillContentClass;

	UPROPERTY(BlueprintReadOnly, Category = "Content", meta = (AllowPrivateAccess = "true"))
	class USkillContent *SkillContent;
	
	UPROPERTY(BlueprintReadOnly, Category = "Status", meta = (AllowPrivateAccess = "true"))
	ESkillStatus CurrentStatus;

	UPROPERTY(BlueprintReadOnly, Category = "Cooldown", meta = (AllowPrivateAccess = "true"))
	float CurrentCooldown;

	UPROPERTY(BlueprintReadOnly, Category = "Cooldown", meta = (AllowPrivateAccess = "true"))
	float ActualCooldown;

	FScriptDelegate ProcessOwnerLevelUpDelegate;

public:

	void HandleInterceptedAttackInput_Impl(EInputEvent InputType);

	virtual void SetupInputComponent(UInputComponent *InInputComponent) { }

	/* UActorComponent Interface */
	virtual void InitializeComponent() override;
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void Activate(/* Deprecated */ bool bReset) override;
	virtual void Deactivate() override;
	/* ~UActorComponent Interface */

protected:

	/* UActorComponent Interface */
	virtual void OnRegister() override;
	virtual void OnUnregister() override;

	DEPRECATED(PreAlpha, "USkillComponent::ShouldActivate() is no longer used. Instead, call CanActivate() or CanDeactivate() to do the same action")
	virtual bool ShouldActivate() const override final { return false; }
	/* ~UActorComponent Interface */

	virtual bool CanActivate() const { return true; }

	virtual bool CanDeactivate() const;

public:

	FORCEINLINE APawn* GetOwnerAsPawn() const { return PawnOwner; }
	FORCEINLINE class ISkillOwnerInterface* GetOwnerAsSkillOwner() const { return CastChecked<ISkillOwnerInterface>(GetOwner()); }
	FORCEINLINE class USkillContent* GetSkillContent() const { return SkillContent; }
	FORCEINLINE ESkillStatus GetSkillStatus() const { return CurrentStatus; }
	FORCEINLINE float GetCurrentCooldown() const { return CurrentCooldown; }
	FORCEINLINE float GetActualCooldown() const { return ActualCooldown; }

private:
	
	void PostSkillProcess(FSkillUseResult const& InResult);
	FORCEINLINE void ResetProperties();

	UFUNCTION()
	void ProcessOwnerLevelUp(FActorLevelUpEvent const &e);

protected:

	virtual bool InternalCanUse_Implementation() const { return true; }
	virtual void StartSkillProcess_Implementation() { }
	virtual void CancelSkillProcess_Implementation(bool &bOutCanceled, FSkillUseResult &InOutResult) { }
	virtual void InternalPostSkillProcess_Implementation(FSkillUseResult const& InResult) { }
	virtual void InternalResetProperties_Implementation() { }
	virtual void HandleInterceptedAttackInput_Implementation(EInputEvent InputType) { }
	virtual void TickPassiveEffect_Implementation(float DeltaTime) { }
	virtual void Event_OnUseStarted_Implementation() { }
	virtual void Event_OnUseCanceled_Implementation(FSkillUseResult const& InResult) { }
	virtual void Event_OnUseCompleted_Implementation(FSkillUseResult const& InResult) { }
	virtual void Event_OnCooldownResolved_Implementation() { }
	virtual void Event_OnPassiveEffectApplied_Implementation() { }
	virtual void OnSkillActivated_Implementation() { }
	virtual void OnSkillDeactivated_Implementation() { }

};
