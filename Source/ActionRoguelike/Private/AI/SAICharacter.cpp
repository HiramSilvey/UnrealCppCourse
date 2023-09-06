// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/SAICharacter.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BrainComponent.h"
#include "DrawDebugHelpers.h"
#include "Perception/PawnSensingComponent.h"
#include "SAttributeComponent.h"
#include "SWorldUserWidget.h"

// Sets default values
ASAICharacter::ASAICharacter()
{
	TimeToHitParamName = "TimeToHit";
	AttributeComp = CreateDefaultSubobject<USAttributeComponent>("AttributeComp");
	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>("PawnSensingComp");
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void ASAICharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	AttributeComp->OnHealthChanged.AddDynamic(this, &ASAICharacter::OnHealthChanged);
	PawnSensingComp->OnSeePawn.AddDynamic(this, &ASAICharacter::OnPawnSeen);
}

void ASAICharacter::SetTargetActor(AActor* NewTarget)
{
	AAIController* AIC = Cast<AAIController>(GetController());
	if (IsValid(AIC))
	{
		AIC->GetBlackboardComponent()->SetValueAsObject("TargetActor", NewTarget);
	}
}

void ASAICharacter::OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth, float Delta)
{
	UE_LOG(LogTemp, Log, TEXT("Bot took %f damage! NewHealth: %f"), Delta, NewHealth);
	if (Delta >= 0.0f)
	{
		return;
	}

	if (InstigatorActor != this)
	{
		SetTargetActor(InstigatorActor);
	}

	if (!IsValid(ActiveHealthBar))
	{
		if (ActiveHealthBar = CreateWidget<USWorldUserWidget>(GetWorld(), HealthBarWidgetClass); IsValid(ActiveHealthBar))
		{
			ActiveHealthBar->AttachedActor = this;
			ActiveHealthBar->AddToViewport();
		}
	}

	GetMesh()->SetScalarParameterValueOnMaterials(TimeToHitParamName, GetWorld()->TimeSeconds);

	if (NewHealth <= 0.0f)
	{
		// This character just died.

		// Stop BT
		AAIController* AIC = Cast<AAIController>(GetController());
		if (IsValid(AIC))
		{
			AIC->GetBrainComponent()->StopLogic("Killed");
		}

		// Ragdoll
		GetMesh()->SetCollisionProfileName("Ragdoll");
		GetMesh()->SetAllBodiesSimulatePhysics(true);

		// Set lifespan
		SetLifeSpan(10.0f);
	}
}

void ASAICharacter::OnPawnSeen(APawn* Pawn)
{
	SetTargetActor(Pawn);
	DrawDebugString(GetWorld(), GetActorLocation(), "PLAYER SPOTTED", nullptr, FColor::White, 4.0f, true);
}