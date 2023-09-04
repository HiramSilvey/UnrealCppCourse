// Fill out your copyright notice in the Description page of Project Settings.

#include "SHealthPotion.h"
#include "SAttributeComponent.h"

// Sets default values
ASHealthPotion::ASHealthPotion()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ASHealthPotion::Interact_Implementation(APawn* InstigatorPawn)
{
	if (!CanUse(InstigatorPawn))
	{
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
		AttributeComp->ApplyHealthChange(this, HealthDelta);

		DisableForCooldown();
	}
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
