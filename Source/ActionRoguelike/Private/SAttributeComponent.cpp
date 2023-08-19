// Fill out your copyright notice in the Description page of Project Settings.

#include "SAttributeComponent.h"

// Sets default values for this component's properties
USAttributeComponent::USAttributeComponent()
{
	Health = HealthMax;
}

bool USAttributeComponent::IsAlive() const
{
	return Health > 0.0f;
}

bool USAttributeComponent::ApplyHealthChange(float Delta)
{
	if (float NewHealth = FMath::Clamp(Health + Delta, HealthMin, HealthMax); Health != NewHealth)
	{
		Health = NewHealth;
		OnHealthChanged.Broadcast(nullptr, this, Health, Delta);
	}

	return true;
}
