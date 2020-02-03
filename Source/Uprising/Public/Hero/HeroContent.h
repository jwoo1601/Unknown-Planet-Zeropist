// Copyright 2017 Ground Breaking Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Character/UCharacter.h"

#include "HeroContent.generated.h"

USTRUCT(BlueprintType)
struct FHeroMovementAttribute {
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PhysicsInteraction")
	uint8 bEnablePhysicsInteraction : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Walking", meta = (ClampMin="0", UIMin="0"))
	float MaxWalkSpeed = 600.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Walking")
	uint8 bEnableCrouching : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Walking", meta = (ClampMin = "0", UIMin = "0", EditCondition = "bEnableCrouching"))
	float MaxWalkSpeedCrouched = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprinting")
	uint8 bEnableSprinting : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprinting", meta = (ClampMin = "0", UIMin = "0", EditCondition = "bEnableSprinting"))
	float MaxSprintSpeed = 1000.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jumpping/Falling")
	uint8 bEnableJumping : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jumping/Falling", meta = (DisplayName = "Jump Z Velocity", ClampMin = "0", UIMin = "0", EditCondition = "bEnableJumping"))
	float JumpZVelocity = 420.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jumping/Falling", meta = (DisplayName = "Jump Z Velocity Falloff", ClampMin = "0", UIMin = "0", EditCondition = "bEnableJumping"))
	float JumpZVelocityFalloff = 40.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jumping/Falling", meta = (ClampMin = "1", UIMin = "1", EditCondition = "bEnableJumping"))
	int32 MaxJumpCount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jumping/Falling", meta = (ClampMin = "0.0", UIMin = "0.0", EditCondition = "bEnableJumping"))
	float MaxJumpKeyHoldTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Swimming")
	uint8 bEnableSwimming : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Swimming", meta = (ClampMin = "0", UIMin = "0", EditCondition = "bEnableSwimming"))
	float MaxSwimSpeed = 400.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flying")
	uint8 bEnableFlying : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flying", meta = (ClampMin = "0", UIMin = "0", EditCondition = "bEnableFlying"))
	float MaxFlySpeed = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General", meta = (ClampMin = "0", UIMin = "0"))
	float Mass = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General", meta = (ClampMin = "0", UIMin = "0"))
	float CrouchedHalfHeight = 40.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General", meta = (ClampMin = "0.0", ClampMax = "90.0", UIMin = "0.0", UIMax = "90.0"))
	float WalkableFloorAngle = 44.765083f;

};

UCLASS(abstract, BlueprintType, Blueprintable)
class UPRISING_API UHeroContent : public UCharacterContent
{
	GENERATED_BODY()
	
public:	

	AHeroContent(const FObjectInitializer &ObjectInitializer);

	/* DEV ONLY */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hero|Weapon")
	TSubclassOf<class AWeapon> DefaultMainWeaponClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hero|Weapon")
	TSubclassOf<class AWeapon> DefaultSubWeaponClass;
	/* ~DEV ONLY */

	UFUNCTION(BlueprintNativeEvent, Category = "Attack|Animation")
	void PlayCustomAttackAnimation(USkeletalMeshComponent *InCharacterMesh, class UAttackType *InType);

	UFUNCTION(BlueprintNativeEvent, Category = "Attack|Animation")
	void StopCustomAttackAnimation(USkeletalMeshComponent *InCharacterMesh);

	UFUNCTION(BlueprintNativeEvent, Category = "Attack|Animation")
	void PlayCustomBurstAttackAnimation(USkeletalMeshComponent *InCharacterMesh, class UAttackType *InType);

public:

	virtual void Tick(float DeltaTime) override;

#if WITH_EDITOR
	virtual void OnConstruction(const FTransform &Transform) override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent &e) override;
#endif

protected:

	virtual void BeginPlay() override;

private:


	TMap<FName, TSubclassOf<class USkillComponent>> SkillMap;


	UPROPERTY(BlueprintReadOnly, Category = "Prop", meta = (AllowPrivateAccess = "true"))
	EHeroRole Role;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hero|Component", meta = (AllowPrivateAccess = "true"))
	uint8 bOverrideTransform : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USphereComponent *HeadVolume;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCapsuleComponent *CollisionVolume;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent *HeroMesh;

#if WITH_EDITORONLY_DATA
	UPROPERTY()
	UChildActorComponent *PreviewWeapon;
#endif

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hero|Skill", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class USkillComponent> HeroSkill1Class;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hero|Skill", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class USkillComponent> HeroSkill2Class;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hero|Skill", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class USkillComponent> WeaponSkillClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hero|Skill", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class USkillComponent> ClassSkillClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hero|Skill", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class USkillComponent> UltimateSkillClass;



	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FHeroMovementAttribute MovementAttribute;

public:

	FORCEINLINE FName GetHeroID() const { return ID; }
	FORCEINLINE FText GetHeroDisplayName() const { return DisplayName; }
	FORCEINLINE bool ShouldOverrideTransform() const { return bOverrideTransform; }
	FORCEINLINE USphereComponent* GetHeadVolume() const { return HeadVolume; }
	FORCEINLINE UCapsuleComponent* GetCollisionVolume() const { return CollisionVolume; }
	FORCEINLINE USkeletalMeshComponent* GetHeroMesh() const { return HeroMesh; }
	FORCEINLINE TSubclassOf<class USkillComponent> GetHeroSkill1Class() const { return HeroSkill1Class; }
	FORCEINLINE TSubclassOf<class USkillComponent> GetHeroSkill2Class() const { return HeroSkill2Class; }
	FORCEINLINE TSubclassOf<class USkillComponent> GetWeaponSkillClass() const { return WeaponSkillClass; }
	FORCEINLINE TSubclassOf<class USkillComponent> GetClassSkillClass() const { return ClassSkillClass; }
	FORCEINLINE TSubclassOf<class USkillComponent> GetUltimateSkillClass() const { return UltimateSkillClass; }
	FORCEINLINE TSubclassOf<class USkillComponent> GetSkillClass(int32 InSkillIndex) const;
	FORCEINLINE int32 GetHP() const { return HP; }
	FORCEINLINE int32 GetShield() const { return Shield; }
	FORCEINLINE FHeroMovementAttribute const& GetMovementAttribute() const { return MovementAttribute; }

private:
	
	virtual void PlayCustomAttackAnimation_Implementation(USkeletalMeshComponent *InCharacterMesh, class UAttackType *InType) { }
	virtual void StopCustomAttackAnimation_Implementation(USkeletalMeshComponent *InCharacterMesh) { }
	virtual void PlayCustomBurstAttackAnimation_Implementation(USkeletalMeshComponent *InCharacterMesh, class UAttackType *InType) { }

};