// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SProjectile.h"
#include "SDashProjectile.generated.h"

class UParticleSystem;

UCLASS()
class ACTIONROGUELIKE_API ASDashProjectile : public ASProjectile
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASDashProjectile();

protected:
	UPROPERTY(EditAnywhere)
	UParticleSystem* ParticleSystem;

	FTimerHandle TimerHandle;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void TeleportAndDestroy();

	void Activate();

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
