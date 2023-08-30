// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/SBTService_CheckAttackRange.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

void USBTService_CheckAttackRange::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
        Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

        // Check distance between AI pawn and target actor

        UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
        if (ensure(BlackboardComp))
        {
                const AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject("TargetActor"));
                if (IsValid(TargetActor))
                {
                        const AAIController* MyController = OwnerComp.GetAIOwner();
                        if (ensure(MyController))
                        {
                                const APawn* AIPawn = MyController->GetPawn();
                                if (ensure(AIPawn))
                                {
                                        const float DistanceTo = FVector::Distance(TargetActor->GetActorLocation(), AIPawn->GetActorLocation());
                                        const bool bWithinRange = DistanceTo < 2000.0f;

                                        bool bHasLOS = false;
                                        if (bWithinRange)
                                        {
                                                bHasLOS = MyController->LineOfSightTo(TargetActor);
                                        }

                                        BlackboardComp->SetValueAsBool(AttackRangeKey.SelectedKeyName, (bWithinRange && bHasLOS));
                                }
                        }
                }
        }
}
