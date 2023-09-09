// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/SBTService_CheckHealth.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "SAttributeComponent.h"

void USBTService_CheckHealth::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!ensure(BlackboardComp))
	{
		return;
	}

	AActor* SelfActor = Cast<AActor>(BlackboardComp->GetValueAsObject(SelfActorKey.SelectedKeyName));
	if (!IsValid(SelfActor))
	{
		return;
	}

	USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(SelfActor);
	if (!ensure(AttributeComp))
	{
		return;
	}

	BlackboardComp->SetValueAsBool(IsLowHealthKey.SelectedKeyName, AttributeComp->GetHealth() <= AttributeComp->GetMaxHealth() * LowHealthPct);
}