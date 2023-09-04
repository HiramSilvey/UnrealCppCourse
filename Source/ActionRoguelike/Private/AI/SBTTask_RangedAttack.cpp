// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/SBTTask_RangedAttack.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "SAttributeComponent.h"

USBTTask_RangedAttack::USBTTask_RangedAttack()
{
	MaxBulletSpread = 2.0f;
}

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
		AActor* TargetActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject("TargetActor"));
		if (!IsValid(TargetActor))
		{
			UE_LOG(LogTemp, Log, TEXT("TargetActor is null"));
			return EBTNodeResult::Failed;
		}

		if (!USAttributeComponent::IsActorAlive(TargetActor))
		{
			UE_LOG(LogTemp, Log, TEXT("TargetActor is already dead"));
			return EBTNodeResult::Failed;
		}

		const FVector Direction = TargetActor->GetActorLocation() - MuzzleLocation;
		FRotator MuzzleRotation = Direction.Rotation();

		MuzzleRotation.Pitch += FMath::RandRange(0.0f, MaxBulletSpread);
		MuzzleRotation.Yaw += FMath::RandRange(-MaxBulletSpread, MaxBulletSpread);

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
