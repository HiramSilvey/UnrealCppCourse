// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/SBTTask_RangedAttack.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"

EBTNodeResult::Type USBTTask_RangedAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const AAIController* MyController = OwnerComp.GetAIOwner();
	if (ensure(MyController))
	{
		APawn* AIPawn = MyController->GetPawn();
		if (!IsValid(AIPawn))
		{
			UE_LOG(LogTemp, Log, TEXT("AIPawn is null"));
			return EBTNodeResult::Failed;
		}

		const FVector MuzzleLocation = Cast<ACharacter>(AIPawn)->GetMesh()->GetSocketLocation("Muzzle_01");
		const AActor* TargetActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject("TargetActor"));
		if (!IsValid(TargetActor))
		{
			UE_LOG(LogTemp, Log, TEXT("TargetActor is null"));
			return EBTNodeResult::Failed;
		}

		const FVector Direction = TargetActor->GetActorLocation() - MuzzleLocation;
		const FRotator MuzzleRotation = Direction.Rotation();

		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		Params.Instigator = AIPawn;

		const AActor* NewProj = GetWorld()->SpawnActor<AActor>(ProjectileClass, MuzzleLocation, MuzzleRotation, Params);

		UE_LOG(LogTemp, Log, TEXT("NewProj = %d"), IsValid(NewProj));
		return IsValid(NewProj) ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
	}

	UE_LOG(LogTemp, Log, TEXT("MyController is null"));
	return EBTNodeResult::Failed;
}
