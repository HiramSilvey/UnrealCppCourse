// Fill out your copyright notice in the Description page of Project Settings.

#include "SPowerUp.h"
#include "Components/SphereComponent.h"

// Sets default values
ASPowerUp::ASPowerUp()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComp = CreateDefaultSubobject<USphereComponent>("SphereComp");
	SphereComp->SetCollisionProfileName("BlockAllDynamic");
	RootComponent = SphereComp;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	MeshComp->SetupAttachment(SphereComp);
}

bool ASPowerUp::CanUse(APawn* InstigatorPawn)
{
	return MeshComp->IsVisible() && IsValid(InstigatorPawn);
}

void ASPowerUp::DisableForCooldown()
{
	MeshComp->SetVisibility(false);
	SetActorEnableCollision(false);
	GetWorldTimerManager()
		.SetTimer(TimerHandle, this, &ASPowerUp::Enable, Cooldown);
}

void ASPowerUp::Enable()
{
	GetWorldTimerManager().ClearTimer(TimerHandle);
	MeshComp->SetVisibility(true);
	SetActorEnableCollision(true);
}

// Called when the game starts or when spawned
void ASPowerUp::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ASPowerUp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
