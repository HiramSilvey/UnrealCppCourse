// Fill out your copyright notice in the Description page of Project Settings.

#include "SCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "SAttributeComponent.h"
#include "SInteractionComponent.h"

// Sets default values
ASCharacter::ASCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationYaw = false;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>("SpringArmComp");
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->SetRelativeLocation(FVector(0.0f, 0.0f, 60.0f));
	SpringArmComp->TargetArmLength = 250.0f;
	SpringArmComp->SocketOffset = FVector(0.0f, 90.0f, 0.0f);
	SpringArmComp->SetupAttachment(RootComponent);

	CameraComp = CreateDefaultSubobject<UCameraComponent>("CameraComp");
	CameraComp->SetupAttachment(SpringArmComp);

	InteractionComp = CreateDefaultSubobject<USInteractionComponent>("InteractionComp");

	AttributeComp = CreateDefaultSubobject<USAttributeComponent>("AttributeComp");

	GetCharacterMovement()->bOrientRotationToMovement = true;
}

void ASCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	AttributeComp->OnHealthChanged.AddDynamic(this, &ASCharacter::OnHealthChanged);
}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(SCharacterMappingContext, 0);
		}
	}
}

void ASCharacter::OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth, float Delta)
{
	if (NewHealth <= 0.0f && Delta < 0.0f)
	{
		APlayerController* PC = Cast<APlayerController>(GetController());
		DisableInput(PC);
	}
}

void ASCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D MoveAxisValue = Value.Get<FVector2D>();
	FRotator ControlRotation = GetControlRotation();
	ControlRotation.Pitch = 0.0f;
	ControlRotation.Roll = 0.0f;

	AddMovementInput(FRotationMatrix(ControlRotation).GetScaledAxis(EAxis::Y), MoveAxisValue.X);
	AddMovementInput(ControlRotation.Vector(), MoveAxisValue.Y);
}

void ASCharacter::Look(const FInputActionValue& Value)
{
	if (GetController())
	{
		const FVector2D LookAxisValue = Value.Get<FVector2D>();
		AddControllerYawInput(LookAxisValue.X);
		AddControllerPitchInput(LookAxisValue.Y);
	}
}

void ASCharacter::Jump(const FInputActionValue& Value)
{
	if (Value.Get<bool>())
	{
		Super::Jump();
	}
}

void ASCharacter::Attack_TimeElapsed(TSubclassOf<AActor> ProjectileClass)
{
	const FVector CameraLocation = CameraComp->GetComponentLocation();
	const FRotator CameraRotation = CameraComp->GetComponentRotation();
	const FVector End = CameraLocation + (CameraRotation.Vector() * 5000);
	DrawDebugLine(GetWorld(), CameraLocation, End, FColor::Red, false, 2.0f, 0, 2.0f);

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_PhysicsBody);

	FHitResult Target;
	const bool bBlockingHit = GetWorld()->LineTraceSingleByObjectType(Target, CameraLocation, End, ObjectQueryParams);
	const FVector TargetLocation = bBlockingHit ? Target.ImpactPoint : End;

	const FVector HandLocation = GetMesh()->GetSocketLocation("Muzzle_01");

	DrawDebugLine(GetWorld(), HandLocation, TargetLocation, FColor::Green, false, 2.0f, 0, 2.0f);
	UE_LOG(LogTemp, Log, TEXT("Object hit? %d"), bBlockingHit);
	if (bBlockingHit)
	{
		UE_LOG(LogTemp, Log, TEXT("Object = %s"), *Target.GetComponent()->GetName());
	}

	const FRotator AimRotation = UKismetMathLibrary::FindLookAtRotation(HandLocation, TargetLocation);
	const FTransform SpawnTM = FTransform(AimRotation, HandLocation);
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Instigator = this;
	GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnTM, SpawnParams);
}

void ASCharacter::Attack(typename FTimerDelegate::TMethodPtr<ASCharacter> InTimerMethod)
{
	PlayAnimMontage(AttackAnim);
	GetWorldTimerManager().SetTimer(
		TimerHandle_Attack, this, InTimerMethod, 0.2f);
}

void ASCharacter::PrimaryAttack_TimeElapsed()
{
	Attack_TimeElapsed(PrimaryProjectileClass);
}

void ASCharacter::PrimaryAttack()
{
	Attack(&ASCharacter::PrimaryAttack_TimeElapsed);
}

void ASCharacter::DashAttack_TimeElapsed()
{
	Attack_TimeElapsed(DashProjectileClass);
}

void ASCharacter::DashAttack()
{
	Attack(&ASCharacter::DashAttack_TimeElapsed);
}

void ASCharacter::UltimateAttack_TimeElapsed()
{
	Attack_TimeElapsed(UltimateProjectileClass);
}

void ASCharacter::UltimateAttack()
{
	Attack(&ASCharacter::UltimateAttack_TimeElapsed);
}

void ASCharacter::PrimaryInteract()
{
	if (InteractionComp)
	{
		InteractionComp->PrimaryInteract();
	}
}

// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASCharacter::Look);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ASCharacter::Jump);
		EnhancedInputComponent->BindAction(PrimaryAttackAction, ETriggerEvent::Started, this, &ASCharacter::PrimaryAttack);
		EnhancedInputComponent->BindAction(DashAttackAction, ETriggerEvent::Started, this, &ASCharacter::DashAttack);
		EnhancedInputComponent->BindAction(UltimateAttackAction, ETriggerEvent::Started, this, &ASCharacter::UltimateAttack);
		EnhancedInputComponent->BindAction(PrimaryInteractAction, ETriggerEvent::Started, this, &ASCharacter::PrimaryInteract);
	}
}
