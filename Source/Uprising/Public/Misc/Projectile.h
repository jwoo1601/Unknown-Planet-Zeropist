// Copyright 2017 Ground Breaking Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/MeshComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"

#include "Projectile.generated.h"

UCLASS(abstract, BlueprintType, Blueprintable)
class UPRISING_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	

	AProjectile(FObjectInitializer const& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void SetVelocity(FVector const& InVelocity, bool bApplyInLocalSpace = true);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void AddVelocity(FVector const& Delta, bool bApplyInLocalSpace = true);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void SetDirection(FVector const& InDirection, bool bApplyInLocalSpace = true);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void SetSpeed(float InSpeed);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void AddSpeed(float Delta);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void SetMaxSpeed(float InMaxSpeed);

	UFUNCTION(BlueprintCallable, Category = "Launch")
	bool CanLaunch() const;

	UFUNCTION(BlueprintCallable, Category = "Launch")
	void Launch();

	UFUNCTION(BlueprintCallable, Category = "Launch")
	bool HasLaunched() const;

protected:

	// AActor Interface
	virtual void BeginPlay() override;
	// ~AActor Interface

public:	

	// AActor Interface
	virtual void Tick(float DeltaTime) override;
	// ~AActor Interface

private:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UPrimitiveComponent *CollisionVolume;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UMeshComponent *MeshComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent *MovementComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	uint8 bLaunchWhenSpawned : 1;

	UPROPERTY()
	uint8 bLaunched : 1;

};
