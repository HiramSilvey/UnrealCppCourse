// Fill out your copyright notice in the Description page of Project Settings.

#include "SDashProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleEmitter.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
ASDashProjectile::ASDashProjectile()
	: DetonateDelay(0.2f), TeleportDelay(0.2f) {}

// Called when the game starts or when spawned
void ASDashProjectile::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(TimerHandle, this, &ASDashProjectile::Explode, DetonateDelay);
}

void ASDashProjectile::TeleportAndDestroy()
{
	if (APawn* MyInstigator = GetInstigator(); IsValid(MyInstigator))
	{
		if (!MyInstigator->TeleportTo(GetActorLocation(), MyInstigator->GetActorRotation()))
		{
			FVector AdjustedLocation(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z + (MyInstigator->GetSimpleCollisionHalfHeight() / 2.0f));
			DrawDebugCapsule(GetWorld(), AdjustedLocation, MyInstigator->GetSimpleCollisionHalfHeight(), MyInstigator->GetSimpleCollisionRadius(), MyInstigator->GetActorQuat(), FColor::Red, false, 2.0f, 0, 1.0f);
			if (!MyInstigator->TeleportTo(AdjustedLocation, MyInstigator->GetActorRotation()))
			{
				UE_LOG(LogTemp, Log, TEXT("Failed to teleport after moving to %s!"), AdjustedLocation);
			}
		}
		UE_LOG(LogTemp, Log, TEXT("Attempted to teleport!"));
	}
	Destroy();
}

void ASDashProjectile::Activate()
{
	MovementComp->StopMovementImmediately();
	EffectComp->DeactivateSystem();
	SetActorEnableCollision(false);
	UGameplayStatics::SpawnEmitterAtLocation(this, ImpactVFX, GetActorLocation(), GetActorRotation());
	GetWorldTimerManager()
		.SetTimer(TimerHandle, this, &ASDashProjectile::TeleportAndDestroy, TeleportDelay);
}

void ASDashProjectile::Explode_Implementation()
{
	GetWorldTimerManager().ClearTimer(TimerHandle);
	Activate();
}

// Called every frame
void ASDashProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
