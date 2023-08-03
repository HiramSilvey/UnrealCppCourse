// Fill out your copyright notice in the Description page of Project Settings.

#include "SDashProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleEmitter.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
ASDashProjectile::ASDashProjectile()
{
	MovementComp->InitialSpeed = 2000.0f;
}

// Called when the game starts or when spawned
void ASDashProjectile::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(TimerHandle, this, &ASDashProjectile::Activate, 0.2f);
	SphereComp->OnComponentHit.AddDynamic(this, &ASDashProjectile::OnHit);
}

void ASDashProjectile::TeleportAndDestroy()
{
	if (APawn* MyInstigator = this->GetInstigator(); MyInstigator != nullptr)
	{
		MyInstigator->TeleportTo(this->GetActorLocation(), MyInstigator->GetActorRotation());
	}
	Destroy();
}

void ASDashProjectile::Activate()
{
	MovementComp->StopMovementImmediately();
	SphereComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SphereComp->SetVisibility(false);
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ParticleSystem, this->GetActorTransform());
	GetWorldTimerManager()
		.SetTimer(TimerHandle, this, &ASDashProjectile::TeleportAndDestroy, 0.2f);
}

void ASDashProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	GetWorldTimerManager().ClearTimer(TimerHandle);
	Activate();
}

// Called every frame
void ASDashProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
