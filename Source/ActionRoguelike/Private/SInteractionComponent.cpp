// Fill out your copyright notice in the Description page of Project Settings.

#include "SInteractionComponent.h"
#include "SGameplayInterface.h"
#include "DrawDebugHelpers.h"

// Sets default values for this component's properties
USInteractionComponent::USInteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void USInteractionComponent::PrimaryInteract()
{
	FVector EyeLocation;
	FRotator EyeRotation;
	AActor* Owner = GetOwner();
	Owner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

	const FVector End = EyeLocation + (EyeRotation.Vector() * 80);

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);

	FCollisionShape Shape;
	const float Radius = 100.0f;
	Shape.SetSphere(Radius);

	TArray<FHitResult> Hits;
	const bool bBlockingHit = GetWorld()->SweepMultiByObjectType(Hits, EyeLocation, End, FQuat::Identity, ObjectQueryParams, Shape);
	const FColor LineColor = bBlockingHit ? FColor::Blue : FColor::Red;

	for (const FHitResult& Hit : Hits)
	{
		DrawDebugSphere(GetWorld(), Hit.ImpactPoint, Radius, 32, LineColor, false, 2.0f);
		if (AActor* HitActor = Hit.GetActor())
		{
			if (HitActor->Implements<USGameplayInterface>())
			{
				ISGameplayInterface::Execute_Interact(HitActor, Cast<APawn>(Owner));
				break;
			}
		}
	}
	DrawDebugLine(GetWorld(), EyeLocation, End, LineColor, false, 2.0f, 0, 2.0f);
}

// Called when the game starts
void USInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}

// Called every frame
void USInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}
