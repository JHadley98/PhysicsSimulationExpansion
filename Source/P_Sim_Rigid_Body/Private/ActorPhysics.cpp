// Fill out your copyright notice in the Description page of Project Settings.

#include "ActorPhysics.h"
#include "Runtime/Engine/Classes/Components/InputComponent.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Actor.h"
#include "Engine.h"

// Sets default values
AActorPhysics::AActorPhysics()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AActorPhysics::BeginPlay()
{
	Super::BeginPlay();

	SetActorLocationAndRotation(NewLocation, NewRotation, false, 0, ETeleportType::None);
}

// Called every frame
void AActorPhysics::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//FVector MyCharacter = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
//	FVector MyCharacter = GetActorLocation();

//	MyCharacter.X += 1.0f;

//	SetActorLocation(MyCharacter);
//	if (GEngine) 
//	{
//		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Player Location: %s"), 
//			*MyCharacter.ToString()));
//	}
}