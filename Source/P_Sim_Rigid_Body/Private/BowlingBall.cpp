// Fill out your copyright notice in the Description page of Project Settings.

#include "BowlingBall.h"
#include "Pins.h"
#include "Runtime/Engine/Classes/Components/InputComponent.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Actor.h"
#include "Engine.h"

// Sets default values
ABowlingBall::ABowlingBall()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// CreateDefault Mesh componet, to allow a mesh to be added using blueprint
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	// Set Mesh to root component
	RootComponent = Mesh;

	// Set Velocity for X, Y, Z applying the value 5 to X to give the X axis a velocity of 5 so the bowling ball can move forward at a set velocity
	VelocityX = 5;
	VelocityY = 0;
	VelocityZ = 0;
	// Apply drag value to bowling bowl so eventually the velocity will get to zero stopping the movement of the bowling ball
	Drag = 0.001;
}

// Called when the game starts or when spawned
void ABowlingBall::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABowlingBall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	FVector MidPoint;
	FVector DistToMidPoint;
	GetActorBounds(false, MidPoint, DistToMidPoint);

	// Collide with walls
	if (MidPoint.X - DistToMidPoint.X <= 0 && VelocityX < 0)
	{
		VelocityX = 0 - VelocityX;
	}
	if (MidPoint.X + DistToMidPoint.X >= 2200 && VelocityX > 0)
	{
		VelocityX = 0 - VelocityX;
	}
	if (MidPoint.Y - DistToMidPoint.Y <= 0 && VelocityY < 0)
	{
		VelocityY = 0 - VelocityY;
	}
	if (MidPoint.Y + DistToMidPoint.Y >= 980 && VelocityY > 0)
	{
		VelocityY = 0 - VelocityY;
	}

	// Apply drag to slow down bowling ball (Dynamic Friction)
	if (VelocityX > 0.1)
	{
		VelocityX = VelocityX - Drag;
	}
	else if (VelocityX < -0.1)
	{
		VelocityX = VelocityX + Drag;
	}
	if (VelocityY > 0.1)
	{
		VelocityY = VelocityY - Drag;
	}
	else if (VelocityY < -0.1)
	{
		VelocityY = VelocityY + Drag;
	}
	if (VelocityZ > 0.1)
	{
		VelocityZ = VelocityZ - Drag;
	}
	else if (VelocityZ < -0.1)
	{
		VelocityZ = VelocityZ + Drag;
	}

	// Set velocity to zero if very close to zero (Static Friction)@
	if (VelocityX < 0.1 && VelocityX > -0.1)
	{
		VelocityX = 0;
	}
	if (VelocityY < 0.1 && VelocityY > -0.1)
	{
		VelocityY = 0;
	}
	if (VelocityZ < 0.1 && VelocityZ > -0.1)
	{
		VelocityZ = 0;
	}

	// Create bowlingball vector, set velocity for X, Y, Z and set location for bowlingball
	FVector BowlingBall = GetActorLocation();
	BowlingBall.X += VelocityX;
	BowlingBall.Y += VelocityY;
	BowlingBall.Z += VelocityZ;
	SetActorLocation(BowlingBall);

	if (GEngine)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Player Location: %s"),
		//	*MyCharacter.ToString()));
	}
}

// Called to bind functionality to input
void ABowlingBall::SetupPlayerInputComponent(UInputComponent * PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ABowlingBall::OnOverlap(APawn* OverlappedActor, APins* OtherActor)
{


	FVector OtherLocation = OtherActor->GetActorLocation();
	FVector OverLappedLocation = OverlappedActor->GetActorLocation();
	PositionX = OtherActor->getVelocityX();

	// Set X direction to be -1 when collision occurs
	VelocityX = VelocityX + OtherActor->getVelocityX();
	VelocityY = VelocityY + OtherActor->getVelocityX();
	//		VelocityZ = VelocityZ + OtherActor->getVelocityX();

			// Debug checks
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("POO HEAD")));
	/*GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("POSITION OF OverlappedActor")));
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("POSITION OF OtherActor")));
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Other location: %s"), *OtherLocation.ToString()));
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("OverLappedLocation : %s"), *OverLappedLocation.ToString()));
	*/
}

// Getters
float ABowlingBall::getVelocityX()
{
	return VelocityX;
}
float ABowlingBall::getVelocitY()
{
	return VelocityY;
}
float ABowlingBall::getVelocityZ()
{
	return VelocityZ;
}

// Set velocity function
void ABowlingBall::setVelocity(float setX, float setY, float setZ)
{
	VelocityX = setX;
	VelocityY = setY;
	VelocityZ = setZ;
}