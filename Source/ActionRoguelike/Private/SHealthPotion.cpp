// Fill out your copyright notice in the Description page of Project Settings.

#include "SHealthPotion.h"
#include "SAttributeComponent.h"
#include "Components/SphereComponent.h"

// Sets default values
ASHealthPotion::ASHealthPotion()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComp = CreateDefaultSubobject<USphereComponent>("SphereComp");
	SphereComp->SetCollisionProfileName("BlockAllDynamic");
	RootComponent = SphereComp;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	MeshComp->SetupAttachment(SphereComp);
}

void ASHealthPotion::Interact_Implementation(APawn* InstigatorPawn)
{
	UE_LOG(LogTemp, Log, TEXT("Interaction by %s"), *InstigatorPawn->GetName());
	if (!MeshComp->IsVisible() || !IsValid(InstigatorPawn))
	{
		UE_LOG(LogTemp, Log, TEXT("MeshComp visible? %d"), MeshComp->IsVisible());
		return;
	}
	if (USAttributeComponent* AttributeComp = InstigatorPawn->GetComponentByClass<USAttributeComponent>(); IsValid(AttributeComp))
	{
		if (AttributeComp->IsMaxHealth())
		{
			UE_LOG(LogTemp, Log, TEXT("Instigator %s is already at max health!"), *InstigatorPawn->GetName());
			return;
		}

		UE_LOG(LogTemp, Log, TEXT("Applying health delta of %f"), HealthDelta);
		AttributeComp->ApplyHealthChange(HealthDelta);

		MeshComp->SetVisibility(false);
		SetActorEnableCollision(false);
		GetWorldTimerManager()
			.SetTimer(TimerHandle, this, &ASHealthPotion::Reactivate, Cooldown);
	}
}

void ASHealthPotion::Reactivate()
{
	GetWorldTimerManager().ClearTimer(TimerHandle);
	MeshComp->SetVisibility(true);
	SetActorEnableCollision(true);
}

// Called when the game starts or when spawned
void ASHealthPotion::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ASHealthPotion::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
