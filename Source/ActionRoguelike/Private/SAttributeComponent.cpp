// Fill out your copyright notice in the Description page of Project Settings.

#include "SAttributeComponent.h"

// Sets default values for this component's properties
USAttributeComponent::USAttributeComponent()
{
	Health = HealthMax;
}

USAttributeComponent* USAttributeComponent::GetAttributes(AActor* FromActor)
{
	if (IsValid(FromActor))
	{
		return Cast<USAttributeComponent>(FromActor->GetComponentByClass(USAttributeComponent::StaticClass()));
	}
	return nullptr;
}

bool USAttributeComponent::IsActorAlive(AActor* Actor)
{
	if (USAttributeComponent* AttributeComp = GetAttributes(Actor); IsValid(AttributeComp))
	{
		return AttributeComp->IsAlive();
	}
	return false;
}

float USAttributeComponent::GetHealth() const
{
	return Health;
}

float USAttributeComponent::GetMaxHealth() const
{
	return HealthMax;
}

bool USAttributeComponent::Kill(AActor* InstigatorActor)
{
	return ApplyHealthChange(InstigatorActor, -HealthMax);
}

bool USAttributeComponent::IsMaxHealth() const
{
	return Health >= HealthMax;
}

bool USAttributeComponent::IsAlive() const
{
	return Health > 0.0f;
}

bool USAttributeComponent::ApplyHealthChange(AActor* InstigatorActor, float Delta)
{
	if (!GetOwner()->CanBeDamaged())
	{
		return false;
	}

	if (float NewHealth = FMath::Clamp(Health + Delta, 0.0f, HealthMax); Health != NewHealth)
	{
		Health = NewHealth;
		OnHealthChanged.Broadcast(InstigatorActor, this, Health, Delta);
	}

	return true;
}
