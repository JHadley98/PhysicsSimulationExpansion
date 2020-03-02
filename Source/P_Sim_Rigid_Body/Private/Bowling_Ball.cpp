// Fill out your copyright notice in the Description page of Project Settings.

#include "Bowling_Ball.h"
#include "Pins.h"
#include "Runtime/Engine/Classes/Components/InputComponent.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Actor.h"
#include "Engine.h"

// Sets default values
ABowling_Ball::ABowling_Ball() {
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// CreateDefault Mesh componet, to allow a mesh to be added using blueprint
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	// Set Mesh to root component
	RootComponent = Mesh;

	}

// Called when the game starts or when spawned
void ABowling_Ball::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ABowling_Ball::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Update velocity and then reset deltas
	VelocityX += deltavx;
	VelocityY += deltavy;
	VelocityZ += deltavz;

	float deltavx1 = deltavx;
	float deltavy1 = deltavy;
	float deltavz1 = deltavz;
	deltavx = 0;
	deltavy = 0;
	deltavz = 0;


	FVector MidPoint;
	FVector DistToMidPoint;
	GetActorBounds(false, MidPoint, DistToMidPoint);

	// Collide with walls
	if (MidPoint.X - DistToMidPoint.X <= 60 && VelocityX < 0)
	{
		VelocityX = 0 - VelocityX * Restitution;
	}
	if (MidPoint.X + DistToMidPoint.X >= 2370 && VelocityX > 0)
	{
		VelocityX = 0 - VelocityX * Restitution;
	}
	if (MidPoint.Y - DistToMidPoint.Y <= 100 && VelocityY < 0)
	{
		VelocityY = 0 - VelocityY * Restitution;
	}
	if (MidPoint.Y + DistToMidPoint.Y >= 1510 && VelocityY > 0)
	{
		VelocityY = 0 - VelocityY * Restitution;
	}

	//Collide with floor (no ceiling)
	if (MidPoint.Z - DistToMidPoint.Z <= 0 && VelocityZ < -1) //Bounce back but lose some speed from collision
	{
		VelocityZ = 0 - VelocityZ * Restitution;
	}
	else if (MidPoint.Z - DistToMidPoint.Z <= 0 && VelocityZ < 0) //travelling too slow to bounce back so stop and ensure not below floor.
	{
		VelocityZ = 0;
		FVector AttachtoFloorVector = GetActorLocation();
		AttachtoFloorVector.Z = 0;
		SetActorLocation(AttachtoFloorVector);
	}

	// Apply drag to slow down bowling ball (Dynamic Friction)
	if (VelocityX != 0) {
		VelocityX = VelocityX * (sqrt(VelocityX*VelocityX + VelocityY * VelocityY + VelocityZ * VelocityZ) - Drag)
			/ (sqrt(VelocityX*VelocityX + VelocityY * VelocityY + VelocityZ * VelocityZ));
	}
	if (VelocityY != 0) {
		VelocityY = VelocityY * (sqrt(VelocityX*VelocityX + VelocityY * VelocityY + VelocityZ * VelocityZ) - Drag)
			/ (sqrt(VelocityX*VelocityX + VelocityY * VelocityY + VelocityZ * VelocityZ));
	}
	if (VelocityZ != 0) {
		VelocityZ = VelocityZ * (sqrt(VelocityX*VelocityX + VelocityY * VelocityY + VelocityZ * VelocityZ) - Drag)
			/ (sqrt(VelocityX*VelocityX + VelocityY * VelocityY + VelocityZ * VelocityZ));
	}

	
	//Add calculation for gravity
	if (MidPoint.Z - DistToMidPoint.Z > 2)  //Only apply if above the floor, otherwise gravity is cancelled by the floor supporting the item
	{
		VelocityZ = VelocityZ - 2;
	}
	else if (MidPoint.Z - DistToMidPoint.Z > 0 && VelocityZ == 0) //If item near floor and not moving then accelerate to floor
	{
		VelocityZ = MidPoint.Z - DistToMidPoint.Z;
	}
	else if (MidPoint.Z - DistToMidPoint.Z > 0 && VelocityZ > 0) //If item near floor and going up then slow down with normal gravity
	{
		VelocityZ = VelocityZ - 2;
	}

	// Set velocity to zero if very close to zero (Static Friction)
	if (abs(VelocityX) + abs(VelocityY) + abs(VelocityZ) < 0.2)
	{
		VelocityX = 0;
		VelocityY = 0;
		VelocityZ = 0;
	}
	else if (VelocityZ <= -1) { VelocityZ = 0; }

	FString FilePath2 = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir()) + TEXT("/Debug_Log.txt");
	FString FileContent2 = TEXT("Offset update  PushBackX = ") + FString::SanitizeFloat(PushBackX)
		+ TEXT(" PushBackY = ") + FString::SanitizeFloat(PushBackY)
		+ TEXT(" PushBackZ = ") + FString::SanitizeFloat(PushBackZ)
		+ TEXT(" VelocityX = ") + FString::SanitizeFloat(VelocityX)
		+ TEXT(" VelocityY = ") + FString::SanitizeFloat(VelocityY)
		+ TEXT(" VelocityZ = ") + FString::SanitizeFloat(VelocityZ)
		+ TEXT("\n")
		+ TEXT(" deltavx = ") + FString::SanitizeFloat(deltavx1)
		+ TEXT(" deltavy = ") + FString::SanitizeFloat(deltavy1)
		+ TEXT(" deltavz = ") + FString::SanitizeFloat(deltavz1)
		
		+ TEXT(" Name = ") + GetName()
		+ TEXT("\n")
		+ TEXT("\n");
	if (debuglog == 1) {
		FFileHelper::SaveStringToFile(FileContent2, *FilePath2, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), EFileWrite::FILEWRITE_Append);
	}
	// Create bowlingball vector, set velocity for X, Y, Z and set location for bowlingball
	FVector BowlingBall = GetActorLocation();
	BowlingBall.X = BowlingBall.X + VelocityX + PushBackX;
	BowlingBall.Y = BowlingBall.Y + VelocityY + PushBackY;
	BowlingBall.Z = BowlingBall.Z + VelocityZ + PushBackZ;
	if (BowlingBall.Z < 0) { BowlingBall.Z = 0; }//Ensure object does not go through floor
	if (BowlingBall.Z > 0 && BowlingBall.Z <= 1) { BowlingBall.Z = 0; }//Gravity effect when just above floor
	SetActorLocation(BowlingBall);

	//reset offsets to 0 now that they have been used.
	PushBackX = 0;
	PushBackY = 0;
	PushBackZ = 0;

	//Pot ball in pocket
	if (BowlingBall.X >= 2300 && BowlingBall.Y <= 190) { Destroy(); }
	else if (BowlingBall.X >= 2300 && BowlingBall.Y >= 1440) { Destroy(); }
	else if (BowlingBall.X <= 110 && BowlingBall.Y <= 190) { Destroy(); }
	else if (BowlingBall.X <= 110 && BowlingBall.Y >= 1440) { Destroy(); }
	else if (BowlingBall.X <= 1260 && BowlingBall.X >= 1100 && BowlingBall.Y <= 190) { Destroy(); }
	else if (BowlingBall.X <= 1260 && BowlingBall.X >= 1100 && BowlingBall.Y >= 1440) { Destroy(); }


	if (GEngine)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Player Location: %s"),
		//	*MyCharacter.ToString()));
	}
}

void ABowling_Ball::OnOverlap(AActor* OverlappedActor, APins* OtherActor) {
		//Debug to file
		FString FilePath = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir()) + TEXT("/BBall_Log.txt");
		FString FileContent = TEXT("BB Collision name1 = ") + GetName()
			+ TEXT(" name2 = ") + OtherActor->GetName()

			+ TEXT("\n");
		FFileHelper::SaveStringToFile(FileContent, *FilePath, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), EFileWrite::FILEWRITE_Append);
		
}

// Getters
float ABowling_Ball::getVelocityX()
{
	return VelocityX;
}
float ABowling_Ball::getVelocityY()
{
	return VelocityY;
}
float ABowling_Ball::getVelocityZ()
{
	return VelocityZ;
}
int ABowling_Ball::getMass()
{
	return Mass;
}
// Set velocity function
void ABowling_Ball::setVelocity(float setX, float setY, float setZ)
{
	VelocityX = setX;
	VelocityY = setY;
	VelocityZ = setZ;
}
//Get overlap functions
float ABowling_Ball::getPushBackX()
{
	return PushBackX;
}
float ABowling_Ball::getPushBackY()
{
	return PushBackY;
}
float ABowling_Ball::getPushBackZ()
{
	return PushBackZ;
}
// Set overlap functions
void ABowling_Ball::setOffset(float setpbX, float setpbY, float setpbZ)
{
	PushBackX += setpbX;
	PushBackY += setpbY;
	PushBackZ += setpbZ;

	FString FilePath2 = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir()) + TEXT("/Offset_Log.txt");
	FString FileContent2 = TEXT("Offset Name = ") + GetName()
		+ TEXT(" setX = ") + FString::SanitizeFloat(setpbX)
		+ TEXT(" PushBackX = ") + FString::SanitizeFloat(PushBackX)
		+ TEXT(" setY = ") + FString::SanitizeFloat(setpbY)
		+ TEXT(" PushBackY = ") + FString::SanitizeFloat(PushBackY)
		+ TEXT(" setZ = ") + FString::SanitizeFloat(setpbZ)
		+ TEXT(" PushBackZ = ") + FString::SanitizeFloat(PushBackZ)
		+ TEXT("\n")
		+ TEXT("\n");
	if (debuglog == 1) {
		if (PushBackX != 0) {
			FFileHelper::SaveStringToFile(FileContent2, *FilePath2, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), EFileWrite::FILEWRITE_Append);
		}
	}
}
float ABowling_Ball::getdeltavx()
{
	return deltavx;
}
float ABowling_Ball::getdeltavy()
{
	return deltavy;
}
float ABowling_Ball::getdeltavz()
{
	return deltavz;
}
// Set velocity function
void ABowling_Ball::setdeltav(float setdvX, float setdvY, float setdvZ)
{
	deltavx += setdvX;
	deltavy += setdvY;
	deltavz += setdvZ;
}