// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/SBTTask_Heal.h"
#include "AI/SAICharacter.h"
#include "AIController.h"
#include "SAttributeComponent.h"

USBTTask_Heal::USBTTask_Heal()
{
	HealPct = 1.0f;
}

EBTNodeResult::Type USBTTask_Heal::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UE_LOG(LogTemp, Log, TEXT("Healing!"));
	const AAIController* MyController = OwnerComp.GetAIOwner();
	if (!ensure(MyController))
	{
		UE_LOG(LogTemp, Log, TEXT("MyController is null"));
		return EBTNodeResult::Failed;
	}

	APawn* AIPawn = MyController->GetPawn();
	if (!IsValid(AIPawn))
	{
		UE_LOG(LogTemp, Log, TEXT("AIPawn is null"));
		return EBTNodeResult::Failed;
	}

	USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(Cast<AActor>(AIPawn));
	if (!ensure(AttributeComp))
	{
		UE_LOG(LogTemp, Log, TEXT("AIPawn has no attribute component"));
		return EBTNodeResult::Failed;
	}

	const float HealthToAdd = AttributeComp->GetMaxHealth() * HealPct;
	UE_LOG(LogTemp, Log, TEXT("Healing %f points"), HealthToAdd)
	return AttributeComp->ApplyHealthChange(AIPawn, HealthToAdd) ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
}
