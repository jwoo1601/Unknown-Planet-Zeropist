// Copyright 2017 Ground Breaking Games. All Rights Reserved.

#include "Projectile.h"

AProjectile::AProjectile(FObjectInitializer const& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	CollisionVolume = CreateAbstractDefaultSubobject<UPrimitiveComponent>(TEXT("CollisionVolume"));
	RootComponent = CollisionVolume;

	MeshComponent = CreateAbstractDefaultSubobject<UMeshComponent>(TEXT("Mesh"));
	if (RootComponent && MeshComponent)
	{
		MeshComponent->SetupAttachment(RootComponent);
	}

	MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement"));
}

void AProjectile::SetVelocity(FVector const& InVelocity, bool bApplyInLocalSpace)
{
	if (bApplyInLocalSpace)
	{
		MovementComponent->SetVelocityInLocalSpace(InVelocity);
	}

	else
	{
		MovementComponent->Velocity = InVelocity;
	}
}

void AProjectile::AddVelocity(FVector const& Delta, bool bApplyInLocalSpace = true)
{
	if (bApplyInLocalSpace && MovementComponent->UpdatedComponent)
	{
		MovementComponent->Velocity += MovementComponent->UpdatedComponent->GetComponentToWorld().TransformVectorNoScale(Delta);
	}

	else
	{
		MovementComponent->Velocity += Delta;
	}
}

void AProjectile::SetDirection(FVector const& InDirection, bool bApplyInLocalSpace)
{
	if (bApplyInLocalSpace)
	{
		MovementComponent->SetVelocityInLocalSpace(MovementComponent->Velocity.Size() * InDirection);
	}

	else
	{
		MovementComponent->Velocity = InDirection * MovementComponent->Velocity.Size();
	}
}

void AProjectile::SetSpeed(float InSpeed)
{
	MovementComponent->SetVelocityInLocalSpace(MovementComponent->Velocity.GetSafeNormal() * InSpeed);
}

void AProjectile::AddSpeed(float Delta)
{
	MovementComponent->SetVelocityInLocalSpace(MovementComponent->Velocity.GetSafeNormal() * (Delta + MovementComponent->Velocity.Size()));
}

void AProjectile::SetMaxSpeed(float InMaxSpeed)
{
	MovementComponent->MaxSpeed = InMaxSpeed;
}

bool AProjectile::CanLaunch() const
{
	return !bLaunched;
}

void AProjectile::Launch()
{
	if (!bLaunched && !MovementComponent->IsActive())
	{
		bLaunched = true;
		MovementComponent->SetActive(true);
	}
}

bool AProjectile::HasLaunched() const
{
	return bLaunched;
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	MovementComponent->SetActive(bLaunchWhenSpawned);
}

void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

