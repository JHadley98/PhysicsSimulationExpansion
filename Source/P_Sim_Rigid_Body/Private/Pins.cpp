// Fill out your copyright notice in the Description page of Project Settings.

#include "Pins.h"
#include "Bowling_Ball.h"
#include "Runtime/Engine/Classes/Components/InputComponent.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Actor.h"
#include "Engine.h"
#include <stdlib.h>

// Sets default values
APins::APins()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set Root object to scene component and label it root
	Root = CreateDefaultSubobject<USceneComponent>("Root");
	// Root component is then equal to root
	RootComponent = Root;
	// Create object for PinsMesh to be set to Static Mesh Component, labelling it PinsMesh
	PinsMesh = CreateDefaultSubobject<UStaticMeshComponent>("PinsMesh");
	// Attacj root to PinsMesh, to make the Mesh the root object
	PinsMesh->SetupAttachment(Root);
	// Set collision response for the pins to be overlap
	PinsMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);

	
}

// Called when the game starts or when spawned
void APins::BeginPlay()
{
	Super::BeginPlay();
}



// Called every frame
void APins::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Update velocity and then reset deltas
	VelocityX += deltavx;
	VelocityY += deltavy;
	VelocityZ += deltavz;

	deltavx = 0;
	deltavy = 0;
	deltavz = 0;


	FVector MidPoint;
	FVector DistToMidPoint;
	GetActorBounds(false, MidPoint, DistToMidPoint);

	if (potted == 0) {
		// Collide with walls
		if (MidPoint.X - DistToMidPoint.X <= 60 && VelocityX < 0)
		{
			VelocityX = 0 - VelocityX * Restitution;
		}
		if (MidPoint.X + DistToMidPoint.X >= 2330 && VelocityX > 0)
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
		if (MidPoint.Z - DistToMidPoint.Z > 1)  //Only apply if above the floor, otherwise gravity is cancelled by the floor supporting the item
		{
			VelocityZ = VelocityZ - 1;
		}
		else if (MidPoint.Z - DistToMidPoint.Z > 0 && VelocityZ == 0) //If item near floor and not moving then accelerate to floor
		{
			VelocityZ = 0 - (MidPoint.Z - DistToMidPoint.Z);
		}
		else if (MidPoint.Z - DistToMidPoint.Z > 0 && VelocityZ > 0) //If item near floor and going up then slow down with normal gravity
		{
			VelocityZ = VelocityZ - 1;
		}


		// Set velocity to zero if very close to zero (Static Friction)
		if (abs(VelocityX) + abs(VelocityY) + abs(VelocityZ) < 0.2)
		{
			VelocityX = 0;
			VelocityY = 0;
			VelocityZ = 0;
		}
		else if (VelocityZ <= -1) { VelocityZ = 0; }

		// Create pin vector, set velocity for X, Y, Z and set location for pin
		FVector PinLocationVector = GetActorLocation();
		PinLocationVector.X = PinLocationVector.X + VelocityX + PushBackX;
		PinLocationVector.Y = PinLocationVector.Y + VelocityY + PushBackY;
		PinLocationVector.Z = PinLocationVector.Z + VelocityZ + PushBackZ;
		if (PinLocationVector.Z < 0) { PinLocationVector.Z = 0; }//Ensure object does not go through floor
		if (PinLocationVector.Z > 0 && PinLocationVector.Z <= 1) { PinLocationVector.Z = 0; }//Gravity effect when just above floor
		SetActorLocation(PinLocationVector);


		FString FilePath2 = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir()) + TEXT("/Debug_Log.txt");
		FString FileContent2 = TEXT("Offset update Pin PushBackX = ") + FString::SanitizeFloat(PushBackX)
			+ TEXT(" PushBackY = ") + FString::SanitizeFloat(PushBackY)
			+ TEXT(" PushBackZ = ") + FString::SanitizeFloat(PushBackZ)

			+ TEXT("\n")
			+ TEXT("\n");
		if (debuglog == 1) {
			if (PushBackX != 0 || PushBackY != 0) {
				FFileHelper::SaveStringToFile(FileContent2, *FilePath2, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), EFileWrite::FILEWRITE_Append);
			}
		}


		//reset offsets to 0 now that they have been used.
		PushBackX = 0;
		PushBackY = 0;
		PushBackZ = 0;


		//Pot ball in pocket
		if (PinLocationVector.X >= 2280 && PinLocationVector.Y <= 190) { potted = 1; }
		else if (PinLocationVector.X >= 2280 && PinLocationVector.Y >= 1440) { potted = 2; }
		else if (PinLocationVector.X <= 110 && PinLocationVector.Y <= 180) { potted = 3; }
		else if (PinLocationVector.X <= 110 && PinLocationVector.Y >= 1440) { potted = 4; }
		else if (PinLocationVector.X <= 1260 && PinLocationVector.X >= 1100 && PinLocationVector.Y <= 180) { potted = 5; }
		else if (PinLocationVector.X <= 1260 && PinLocationVector.X >= 1100 && PinLocationVector.Y >= 1440) { potted = 6; }
	}
	else {
		// move towards centre of pocket, lower ball down, then destroy at -50
		FVector PottedLocationVector = GetActorLocation();
		if (potted == 1) {
			if (PottedLocationVector.X < 2330) {
				PottedLocationVector.X = PottedLocationVector.X + 5;
			}
			if (PottedLocationVector.Y > 130) {
				PottedLocationVector.Y = PottedLocationVector.Y - 5;
			}
		}
		if (potted == 2) {
			if (PottedLocationVector.X < 2330) {
				PottedLocationVector.X = PottedLocationVector.X + 5;
			}
			if (PottedLocationVector.Y < 1465) {
				PottedLocationVector.Y = PottedLocationVector.Y + 5;
			}
		}
		if (potted == 3) {
			if (PottedLocationVector.X < 130) {
				PottedLocationVector.X = PottedLocationVector.X - 5;
			}
			if (PottedLocationVector.Y < 1465) {
				PottedLocationVector.Y = PottedLocationVector.Y + 5;
			}
		}
		if (potted == 4) {
			if (PottedLocationVector.X > 130) {
				PottedLocationVector.X = PottedLocationVector.X - 5;
			}
			if (PottedLocationVector.Y < 1465) {
				PottedLocationVector.Y = PottedLocationVector.Y + 5;
			}
		}
		if (potted == 5) {
			if (PottedLocationVector.X > 1220) {
				PottedLocationVector.X = PottedLocationVector.X - 5;
			}
			else if (PottedLocationVector.X < 1150) {
				PottedLocationVector.X = PottedLocationVector.X + 5;
			}
			if (PottedLocationVector.Y < 1465) {
				PottedLocationVector.Y = PottedLocationVector.Y + 5;
			}
		}
		if (potted == 6) {
			if (PottedLocationVector.X > 1220) {
				PottedLocationVector.X = PottedLocationVector.X - 5;
			}
			else if (PottedLocationVector.X < 1150) {
				PottedLocationVector.X = PottedLocationVector.X + 5;
			}
			if (PottedLocationVector.Y < 1465) {
				PottedLocationVector.Y = PottedLocationVector.Y + 5;
			}
		}
		
		PottedLocationVector.Z = PottedLocationVector.Z - 5;
		SetActorLocation(PottedLocationVector);
		if (PottedLocationVector.Z <= -70) { Destroy(); }
	}
}




// Physics - Collisions Code:
// OverlappedActor is the pins
// OtherActor is the bowling ball class and allows for pins to hit other pins
void APins::OnOverlap(AActor* OverlappedActor, ABowling_Ball* OtherActor)
//Other actor refers to bowlingball class, but as both classes are actors then the calls below will work for both.
{
	float m1 = Mass;
	float m2 = OtherActor->getMass();

	//check for name so that object 1 can interact with object 2, but not 2 with 1
	//if (Cast<ABowling_Ball>(OtherActor) != nullptr ) {

		//Functions and values for OverlappedActor are within the current class and accessed direct
		//Functions and values for OtherActor are in Bowling_Balls class or another Pin and so need Getters and Setters.
		// Set X, Y, Z positions for pins and bowling ball labelling them px1 standing for position X object 1 and px2 for position X object 2

	FVector MidPoint1;
	FVector DistToMidPoint1;
	GetActorBounds(false, MidPoint1, DistToMidPoint1);

	FVector MidPoint2;
	FVector DistToMidPoint2;
	OtherActor->GetActorBounds(false, MidPoint2, DistToMidPoint2);

	float px1 = GetActorLocation().X;
	float px2 = OtherActor->GetActorLocation().X;
	float py1 = GetActorLocation().Y;
	float py2 = OtherActor->GetActorLocation().Y;
	float pz1 = GetActorLocation().Z + DistToMidPoint1.Z;
	float pz2 = OtherActor->GetActorLocation().Z + DistToMidPoint2.Z;

	// Set X, Y, Z velocity for pins and bowling ball labelling them vx1 standing for velocity X object 1 and vx2 for velocity X object 2
	float vx1 = VelocityX;
	float vx2 = OtherActor->getVelocityX();
	float vy1 = VelocityY;
	float vy2 = OtherActor->getVelocityY();
	float vz1 = VelocityZ;
	float vz2 = OtherActor->getVelocityZ();

	//Debug to file
	FString FilePath = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir()) + TEXT("/Debug_Log.txt");
	FString FileContent = TEXT("Collision m1 = ") + FString::SanitizeFloat(m1)
		+ TEXT(" m2 = ") + FString::SanitizeFloat(m2)
		+ TEXT(" name1 = ") + GetName()
		+ TEXT(" name2 = ") + OtherActor->GetName()
		+ TEXT("\n")

		+ TEXT(" starting px1 = ") + FString::SanitizeFloat(px1)
		+ TEXT(" py1 = ") + FString::SanitizeFloat(py1)
		+ TEXT(" pz1 = ") + FString::SanitizeFloat(pz1)
		+ TEXT(" px2 = ") + FString::SanitizeFloat(px2)
		+ TEXT(" py2 = ") + FString::SanitizeFloat(py2)
		+ TEXT(" pz2 = ") + FString::SanitizeFloat(pz2)
		+ TEXT("\n")
		+ TEXT(" starting px1 = ") + FString::SanitizeFloat(MidPoint1.X)
		+ TEXT(" py1 = ") + FString::SanitizeFloat(MidPoint1.Y)
		+ TEXT(" pz1 = ") + FString::SanitizeFloat(MidPoint1.Z)
		+ TEXT(" px2 = ") + FString::SanitizeFloat(MidPoint2.X)
		+ TEXT(" py2 = ") + FString::SanitizeFloat(MidPoint2.Y)
		+ TEXT(" pz2 = ") + FString::SanitizeFloat(MidPoint2.Z)
		+ TEXT("\n")

		+ TEXT(" starting vx1 = ") + FString::SanitizeFloat(vx1)
		+ TEXT(" vy1 = ") + FString::SanitizeFloat(vy1)
		+ TEXT(" vz1 = ") + FString::SanitizeFloat(vz1)
		+ TEXT(" vx2 = ") + FString::SanitizeFloat(vx2)
		+ TEXT(" vy2 = ") + FString::SanitizeFloat(vy2)
		+ TEXT(" vz2 = ") + FString::SanitizeFloat(vz2)


		+ TEXT("\n");
	if (debuglog == 1) {
		FFileHelper::SaveStringToFile(FileContent, *FilePath, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), EFileWrite::FILEWRITE_Append);
	}

	//Move both items so item 1 is at 0,0,0
	px2 = px2 - px1;
	py2 = py2 - py1;
	pz2 = pz2 - pz1;
	px1 = 0;
	py1 = 0;
	pz1 = 0;

	//Get scalar size of velocities (vs) in x and y plane only
	float vs1 = sqrt((vx1 * vx1) + (vy1 * vy1));
	float vs2 = sqrt((vx2 * vx2) + (vy2 * vy2));



	//Looking down z axis work out theta for each velocity (angle to x axis)
	// and phi for the coordinate system (angle to rotate to get item 2 to the x axis)
	//Add 180 degrees (PI radians) if vector is on other side of y axis
	float theta1 = 0;
	if (vx1 < 0) {
		theta1 = PI + atan(vy1 / vx1);
	}
	else if (vx1 == 0) {
		if (vy1 > 0) {
			theta1 = PI / 2;
		}
		else if (vy1 < 0) {
			theta1 = PI * 3 / 2;
		}
		else { theta1 = 0; }
	}
	else {
		if (vy1 < 0) {
			theta1 = (2 * PI) + atan(vy1 / vx1);
		}
		else {
			theta1 = atan(vy1 / vx1);
		}
	}
	float theta2 = 0;
	if (vx2 < 0) {
		theta2 = PI + atan(vy2 / vx2);
	}
	else if (vx2 == 0) {
		if (vy2 > 0) {
			theta2 = PI / 2;
		}
		else if (vy2 < 0) {
			theta2 = PI * 3 / 2;
		}
		else { theta2 = 0; }
	}
	else {
		if (vy2 < 0) {
			theta2 = (2 * PI) + atan(vy2 / vx2);
		}
		else {
			theta2 = atan(vy2 / vx2);
		}
	}
	float phi = 0;
	if (px2 < 0) {
		phi = PI + atan(py2 / px2);
	}
	else if (px2 == 0) {
		if (py2 > 0) {
			phi = PI / 2;
		}
		else if (py2 < 0) {
			phi = PI * 3 / 2;
		}
		else { phi = 0; }
	}
	else {
		if (py2 < 0) {
			phi = (2 * PI) + atan(py2 / px2);
		}
		else {
			phi = atan(py2 / px2);
		}
	}


	//Rotate the coordinate system. Original angle for velocity theta become theta minus phi
	//Apply to work out new velocities (nv)
	//C++ struggles to get the difference between 2 numbers.
	//relaced with more complex version using if statements.

	float angle1 = 0;
	float angle2 = 0;
	if (theta1 != 0 && phi != 0) { angle1 = theta1 - phi; }
	else if (theta1 != 0) { angle1 = theta1; }
	else if (phi != 0) { angle1 = 0 - phi; }
	if (theta2 != 0 && phi != 0) { angle2 = theta2 - phi; }
	else if (theta2 != 0) { angle2 = theta2; }
	else if (phi != 0) { angle2 = 0 - phi; }



	float nvx1 = 0;
	float nvy1 = 0;
	float nvx2 = 0;
	float nvy2 = 0;

	//sector is the quadrant of the axis that the angle relates to
	//divide angle by 90 degrees (PI/2)
	float sector1 = floor(angle1 / (PI / 2));
	float sector2 = floor(angle2 / (PI / 2));


	if (sector1 == 0 || sector1 == 4 || sector1 == -4) {
		nvx1 = vs1 * cos(angle1);
		nvy1 = vs1 * sin(angle1);
	}
	else if (sector1 == 1 || sector1 == 5 || sector1 == -3) {
		nvx1 = -1 * vs1 * cos(PI - angle1);
		nvy1 = vs1 * sin(PI - angle1);
	}
	else if (sector1 == 2 || sector1 == 6 || sector1 == -2) {
		nvx1 = -1 * vs1 * cos(angle1 - PI);
		nvy1 = -1 * vs1 * sin(angle1 - PI);
	}
	else if (sector1 == 3 || sector1 == 7 || sector1 == -1) {
		nvx1 = vs1 * cos((2 * PI) - angle1);
		nvy1 = -1 * vs1 * sin((2 * PI) - angle1);
	}

	if (sector2 == 0 || sector2 == 4 || sector2 == -4) {
		nvx2 = vs2 * cos(angle2);
		nvy2 = vs2 * sin(angle2);
	}
	else if (sector2 == 1 || sector2 == 5 || sector2 == -3) {
		nvx2 = -1 * vs2 * cos(PI - angle2);
		nvy2 = vs2 * sin(PI - angle2);
	}
	else if (sector2 == 2 || sector2 == 6 || sector2 == -2) {
		nvx2 = -1 * vs2 * cos(angle2 - PI);
		nvy2 = -1 * vs2 * sin(angle2 - PI);
	}
	else if (sector2 == 3 || sector2 == 7 || sector2 == -1) {
		nvx2 = vs2 * cos((2 * PI) - angle2);
		nvy2 = -1 * vs2 * sin((2 * PI) - angle2);
	}


	float sectorphi = floor(phi / (PI / 2));
	float npx2 = 0;
	float s2 = sqrt((px2 * px2) + (py2 * py2));

	if (sectorphi == 0 || sectorphi == 4 || sectorphi == -4) {
		npx2 = s2 * cos(phi);
	}
	else if (sectorphi == 1 || sectorphi == 5 || sectorphi == -3) {
		npx2 = s2 * cos(PI - phi);
	}
	else if (sectorphi == 2 || sectorphi == 6 || sectorphi == -2) {
		npx2 = s2 * cos(phi - PI);
	}
	else if (sectorphi == 3 || sectorphi == 7 || sectorphi == -1) {
		npx2 = s2 * cos((2 * PI) - phi);
	}



	//Now look down Y axis and workout atheta and aphi for angles to x axis
	//Add 180 degrees (PI radians) if vector is on other side of z axis
	float atheta1 = 0;
	if (nvx1 < 0) {
		atheta1 = PI + atan(vz1 / nvx1);
	}
	else if (nvx1 == 0) {
		if (vz1 > 0) {
			atheta1 = PI / 2;
		}
		else if (vz1 < 0) {
			atheta1 = PI * 3 / 2;
		}
		else { atheta1 = 0; }
	}
	else {
		if (vz1 < 0) {
			atheta1 = (2 * PI) + atan(vz1 / nvx1);
		}
		else {
			atheta1 = atan(vz1 / nvx1);
		}
	}

	float atheta2 = 0;
	if (nvx2 < 0) {
		atheta2 = PI + atan(vz2 / nvx2);
	}
	else if (nvx2 == 0) {
		if (vz2 > 0) {
			atheta2 = PI / 2;
		}
		else if (vz2 < 0) {
			atheta2 = PI * 3 / 2;
		}
		else { atheta2 = 0; }
	}
	else {
		atheta2 = atan(vz2 / nvx2);
	}

	float aphi = 0;
	if (npx2 < 0) {
		aphi = PI + atan(pz2 / npx2);
	}
	else if (npx2 == 0) {
		if (pz2 > 0) {
			aphi = PI / 2;
		}
		else if (pz2 > 0) {
			aphi = PI * 3 / 2;
		}
		else {
			aphi = 0;
		}
	}
	else {
		aphi = atan(pz2 / npx2);
	}

	//code above does not work as C++ struggles to get the difference between 2 numbers.
	//relaced with more complex version using if statements.

	float aangle1 = 0;
	float aangle2 = 0;
	if (atheta1 != 0 && aphi != 0) { aangle1 = atheta1 - aphi; }
	else if (atheta1 != 0) { aangle1 = atheta1; }
	else if (aphi != 0) { aangle1 = 0 - aphi; }
	if (atheta2 != 0 && aphi != 0) { aangle2 = atheta2 - aphi; }
	else if (atheta2 != 0) { aangle2 = atheta2; }
	else if (aphi != 0) { aangle2 = 0 - aphi; }


	// work out scalar velocities looking down y axis (new x velocity and z velocity)

	float vsz1 = sqrt((nvx1 * nvx1) + (vz1 * vz1));
	float vsz2 = sqrt((nvx2 * nvx2) + (vz2 * vz2));


	float nnvx1 = 0;
	float nnvz1 = 0;
	float nnvx2 = 0;
	float nnvz2 = 0;

	//sector is the quadrant of the axis that the angle relates to
	//divide angle by 90 degrees (PI/2)
	float asector1 = floor(aangle1 / (PI / 2));
	float asector2 = floor(aangle2 / (PI / 2));

	if (asector1 == 0 || asector1 == 4 || asector1 == -4) {
		nnvx1 = vsz1 * cos(aangle1);
		nnvz1 = vsz1 * sin(aangle1);
	}
	else if (asector1 == 1 || asector1 == 5 || asector1 == -3) {
		nnvx1 = -1 * vsz1 * cos(PI - aangle1);
		nnvz1 = vsz1 * sin(PI - aangle1);
	}
	else if (asector1 == 2 || asector1 == 6 || asector1 == -2) {
		nnvx1 = -1 * vsz1 * cos(aangle1 - PI);
		nnvz1 = -1 * vsz1 * sin(aangle1 - PI);
	}
	else if (asector1 == 3 || asector1 == 7 || asector1 == -1) {
		nnvx1 = vsz1 * cos((2 * PI) - aangle1);
		nnvz1 = -1 * vsz1 * sin((2 * PI) - aangle1);
	}

	if (asector2 == 0 || asector2 == 4 || asector2 == -4) {
		nnvx2 = vsz2 * cos(aangle2);
		nnvz2 = vsz2 * sin(aangle2);
	}
	else if (asector2 == 1 || asector2 == 5 || asector2 == -3) {
		nnvx2 = -1 * vsz2 * cos(PI - aangle2);
		nnvz2 = vsz2 * sin(PI - aangle2);
	}
	else if (asector2 == 2 || asector2 == 6 || asector2 == -2) {
		nnvx2 = -1 * vsz2 * cos(aangle2 - PI);
		nnvz2 = -1 * vsz2 * sin(aangle2 - PI);
	}
	else if (asector2 == 3 || asector2 == 7 || asector2 == -1) {
		nnvx2 = vsz2 * cos((2 * PI) - aangle2);
		nnvz2 = -1 * vsz2 * sin((2 * PI) - aangle2);
	}


	FString FilePath6 = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir()) + TEXT("/Debug_Log.txt");
	FString FileContent6 = TEXT(" Bounce  theta1 = ") + FString::SanitizeFloat(theta1)
		+ TEXT(" theta2 = ") + FString::SanitizeFloat(theta2)
		+ TEXT(" phi = ") + FString::SanitizeFloat(phi)
		+ TEXT(" nvx1 = ") + FString::SanitizeFloat(nvx1)
		+ TEXT(" nvy1 = ") + FString::SanitizeFloat(nvy1)
		+ TEXT(" nvx2 = ") + FString::SanitizeFloat(nvx2)
		+ TEXT(" nvy2 = ") + FString::SanitizeFloat(nvy2)
		+ TEXT(" cos(angle1) = ") + FString::SanitizeFloat(cos(angle1))
		+ TEXT(" sin(angle1) = ") + FString::SanitizeFloat(sin(angle1))
		+ TEXT(" cos(angle2) = ") + FString::SanitizeFloat(cos(angle2))
		+ TEXT(" sin(angle2) = ") + FString::SanitizeFloat(sin(angle2))
		+ TEXT("\n");
	if (debuglog == 1) {
		FFileHelper::SaveStringToFile(FileContent6, *FilePath6, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), EFileWrite::FILEWRITE_Append);
	}

	FString FilePath7 = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir()) + TEXT("/Debug_Log.txt");
	FString FileContent7 = TEXT(" BounceZ  atheta1 = ") + FString::SanitizeFloat(atheta1)
		+ TEXT(" atheta2 = ") + FString::SanitizeFloat(atheta2)
		+ TEXT(" npx2 = ") + FString::SanitizeFloat(npx2)
		+ TEXT(" aphi = ") + FString::SanitizeFloat(aphi)
		+ TEXT(" nnvx1 = ") + FString::SanitizeFloat(nnvx1)
		+ TEXT(" nnvz1 = ") + FString::SanitizeFloat(nnvz1)
		+ TEXT(" nnvx2 = ") + FString::SanitizeFloat(nnvx2)
		+ TEXT(" nnvz2 = ") + FString::SanitizeFloat(nnvz2)
		+ TEXT(" cos(aangle1) = ") + FString::SanitizeFloat(cos(aangle1))
		+ TEXT(" sin(aangle1) = ") + FString::SanitizeFloat(sin(aangle1))
		+ TEXT(" cos(aangle2) = ") + FString::SanitizeFloat(cos(aangle2))
		+ TEXT(" sin(aangle2) = ") + FString::SanitizeFloat(sin(aangle2))

		+ TEXT("\n");
	if (debuglog == 1) {
		FFileHelper::SaveStringToFile(FileContent7, *FilePath7, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), EFileWrite::FILEWRITE_Append);
	}

	//Collision is now on the x axis only so x and y velocities do not change
	//calculate end vel;ocity (ev)
	float evx1 = (nnvx1 * (m1 - m2) + (2 * m2 * nnvx2)) / (m1 + m2);
	float evx2 = (nnvx2 * (m2 - m1) + (2 * m1 * nnvx1)) / (m1 + m2);

	//round down to make sure rounding does not increase momentum
	if (evx1 > 0) {
		evx1 = floorf(evx1 * 100000) / 100000;
	}
	else {
		evx1 = ceilf(evx1 * 100000) / 100000;
	}
	if (evx2 > 0) {
		evx2 = floorf(evx2 * 100000) / 100000;
	}
	else {
		evx2 = ceilf(evx2 * 100000) / 100000;
	}


		//Rotate velocity back around y axis to give final velocity (fv)
		//This is done on component velocity vectors rotated by the coordinate system rotation only
		
		//sector is the quadrant of the axis that the angle relates to
		//divide angle by 90 degrees (PI/2)
		float naangle = (2 * PI) - aphi;
		float nasector = floor(naangle / (PI / 2));

		float fvx1 = 0;
		float fvx2 = 0;
		float fvz1 = 0;
		float fvz2 = 0;

		if (nasector == 0 || nasector == 4 || nasector == -4) {
			fvx1 = evx1 * cos(naangle)  +  nnvz1 * sin(naangle);
			fvz1 = -1 * evx1 * sin(naangle)  +  nnvz1 * cos(naangle);
			fvx2 = evx2 * cos(naangle)  +  nnvz2 * sin(naangle);
			fvz2 = -1 * evx2 * sin(naangle)  +  nnvz2 * cos(naangle);
		}
		else if (nasector == 1 || nasector == 5 || nasector == -3) {
			fvx1 = -1 * evx1 * cos(PI - naangle)  + nnvz1 * sin(PI - naangle);
			fvz1 = -1 * evx1 * sin(PI - naangle) - nnvz1 * cos(PI - naangle);
			fvx2 = -1 * evx2 * cos(PI - naangle) + nnvz2 * sin(PI - naangle);
			fvz2 = -1 * evx2 * sin(PI - naangle) - nnvz2 * cos(PI - naangle);
		}
		else if (nasector == 2 || nasector == 6 || nasector == -2) {
			fvx1 = -1 * evx1 * cos(naangle - PI) - nnvz1 * sin(naangle - PI);
			fvz1 = evx1 * sin(naangle - PI) - nnvz1 * cos(naangle - PI); 
			fvx2 = -1 * evx2 * cos(naangle - PI) - nnvz2 * sin(naangle - PI);
			fvz2 = evx2 * sin(naangle - PI) - nnvz2 * cos(naangle - PI);
		}
		else if (nasector == 3 || nasector == 7 || nasector == -1) {
			fvx1 = evx1 * cos((2 * PI) - naangle) - nnvz1 * sin((2 * PI) - naangle);
			fvz1 = evx1 * sin((2 * PI) - naangle) + nnvz1 * cos((2 * PI) - naangle);
			fvx2 = evx2 * cos((2 * PI) - naangle) - nnvz2 * sin((2 * PI) - naangle);
			fvz2 = evx2 * sin((2 * PI) - naangle) + nnvz2 * cos((2 * PI) - naangle);
		}



		//Rotate velocity back around z axis to give final final velocity (ffv)
		//This is done on component velocity vectors rotated by the coordinate system rotation only

		//sector is the quadrant of the axis that the angle relates to
		//divide angle by 90 degrees (PI/2)
		float nangle = (2 * PI) - phi;
		float nsector = floor(nangle / (PI / 2));
		
		float ffvx1 = 0;
		float ffvx2 = 0;
		float ffvy1 = 0;
		float ffvy2 = 0;

		if (nsector == 0 || nsector == 4 || nsector == -4) {
			ffvx1 = fvx1 * cos(nangle) + nvy1 * sin(nangle);
			ffvy1 = -1 * fvx1 * sin(nangle) + nvy1 * cos(nangle);
			ffvx2 = fvx2 * cos(nangle) + nvy2 * sin(nangle);
			ffvy2 = -1 * fvx2 * sin(nangle) + nvy2 * cos(nangle);
		}
		else if (nsector == 1 || nsector == 5 || nsector == -3) {
			ffvx1 = -1 * fvx1 * cos(PI - nangle) + nvy1 * sin(PI - nangle);
			ffvy1 = -1 * fvx1 * sin(PI - nangle) - nvy1 * cos(PI - nangle);
			ffvx2 = -1 * fvx2 * cos(PI - nangle) + nvy2 * sin(PI - nangle);
			ffvy2 = -1 * fvx2 * sin(PI - nangle) - nvy2 * cos(PI - nangle);
		}
		else if (nsector == 2 || nsector == 6 || nsector == -2) {
			ffvx1 = -1 * fvx1 * cos(nangle - PI) - nvy1 * sin(nangle - PI);
			ffvy1 = fvx1 * sin(nangle - PI) - nvy1 * cos(nangle - PI);
			ffvx2 = -1 * fvx2 * cos(nangle - PI) - nvy2 * sin(nangle - PI);
			ffvy2 = fvx2 * sin(nangle - PI) - nvy2 * cos(nangle - PI);
		}
		else if (nsector == 3 || nsector == 7 || nsector == -1) {
			ffvx1 = fvx1 * cos((2 * PI) - nangle) - nvy1 * sin((2 * PI) - nangle);
			ffvy1 = fvx1 * sin((2 * PI) - nangle) + nvy1 * cos((2 * PI) - nangle);
			ffvx2 = fvx2 * cos((2 * PI) - nangle) - nvy2 * sin((2 * PI) - nangle);
			ffvy2 = fvx2 * sin((2 * PI) - nangle) + nvy2 * cos((2 * PI) - nangle);
		}

		float ffvz1 = fvz1; //z velocity not changed, but added to keep same naming convention
		float ffvz2 = fvz2; //z velocity not changed, but added to keep same naming convention

		

		FString FilePath8 = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir()) + TEXT("/Debug_Log.txt");
		FString FileContent8 = TEXT(" ReverseZ  naangle = ") + FString::SanitizeFloat(naangle)
			+ TEXT(" aphi = ") + FString::SanitizeFloat(aphi)
			+ TEXT(" fvx1 = ") + FString::SanitizeFloat(fvx1)
			+ TEXT(" fvz1 = ") + FString::SanitizeFloat(fvz1)
			+ TEXT(" fvx2 = ") + FString::SanitizeFloat(fvx2)
			+ TEXT(" fvz2 = ") + FString::SanitizeFloat(fvz2)
			+ TEXT(" cos(naangle) = ") + FString::SanitizeFloat(cos(naangle))
			+ TEXT(" sin(naangle) = ") + FString::SanitizeFloat(sin(naangle))
			+ TEXT("\n");
		if (debuglog == 1) {
			FFileHelper::SaveStringToFile(FileContent8, *FilePath8, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), EFileWrite::FILEWRITE_Append);
		}

		FString FilePath9 = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir()) + TEXT("/Debug_Log.txt");
		FString FileContent9 = TEXT(" Reverse  nangle = ") + FString::SanitizeFloat(nangle)
			+ TEXT(" aphi = ") + FString::SanitizeFloat(aphi)
			+ TEXT(" ffvx1 = ") + FString::SanitizeFloat(ffvx1)
			+ TEXT(" ffvy1 = ") + FString::SanitizeFloat(ffvy1)
			+ TEXT(" ffvx2 = ") + FString::SanitizeFloat(ffvx2)
			+ TEXT(" ffvz2 = ") + FString::SanitizeFloat(ffvz2)
			+ TEXT(" cos(nangle) = ") + FString::SanitizeFloat(cos(nangle))
			+ TEXT(" sin(nangle) = ") + FString::SanitizeFloat(sin(nangle))
			
			+ TEXT("\n");
		if (debuglog == 1) {
			FFileHelper::SaveStringToFile(FileContent9, *FilePath9, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), EFileWrite::FILEWRITE_Append);
		}

		// Set delta velocity for this actor
		deltavx = deltavx + ffvx1 - VelocityX;
		deltavy = deltavy + ffvy1 - VelocityY;
		deltavz = deltavz + ffvz1 - VelocityZ;
		
		if (Cast<ABowling_Ball>(OtherActor) != nullptr) {
			OtherActor->setdeltav(ffvx2 - vx2, ffvy2 - vy2, ffvz2 - vz2);
		}

		FString FilePath5 = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir()) + TEXT("/Debug_Log.txt");
		FString FileContent5 = TEXT("Collision  nnvx1 = ") + FString::SanitizeFloat(nnvx1)
			+ TEXT(" nnvx2 = ") + FString::SanitizeFloat(nnvx2)
			+ TEXT(" evx1 = ") + FString::SanitizeFloat(evx1)
			+ TEXT(" evx2 = ") + FString::SanitizeFloat(evx2)
			+ TEXT(" startmoment = ") + FString::SanitizeFloat((vx1*m1) + (vx2*m2))
			+ TEXT(" endmoment = ") + FString::SanitizeFloat((ffvx1*m1) + (ffvx2*m2))
			+ TEXT(" sector1 = ") + FString::SanitizeFloat(sector1)
			+ TEXT(" sector2 = ") + FString::SanitizeFloat(sector2)

			+ TEXT("\n");
		if (debuglog == 1) {
			FFileHelper::SaveStringToFile(FileContent5, *FilePath5, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), EFileWrite::FILEWRITE_Append);
		}


		//Resolve overlap
		float scalarOverlap = sqrt((px2 * px2) + (py2 * py2) + (pz2 * pz2)) - abs(DistToMidPoint1.X + DistToMidPoint2.X);
		scalarOverlap = scalarOverlap + 0.001;
		//need to move items to total overlap, based on difference in initial velocities


		//Work out amount of overlap on different axis
		float Xoverlap = abs(DistToMidPoint1.X + DistToMidPoint2.X) - abs(px1 - px2);
		float Yoverlap = abs(DistToMidPoint1.Y + DistToMidPoint2.Y) - abs(py1 - py2);
		// Origin is middle of x and y coordinates, but bottom of z coordinates
		float Zoverlap = 0;
		if (pz2 > 0) {
			Zoverlap = abs(DistToMidPoint1.Z * 2) - abs(pz1 - pz2);
		}
		else{
			Zoverlap = abs(DistToMidPoint2.Z * 2) - abs(pz1 - pz2);
		}
		

		//Work out differences initial velocities
		float vxdiff = vx1 - vx2;
		float vydiff = vy1 - vy2;
		float vzdiff = vz1 - vz2;

		//Work out by backing out objects along direction of velocity which axis will be unoverlapped first
		// x=1, y=2, z=3
		int first_axis = 0;
		if (vxdiff != 0) {
			if (vydiff != 0) {
				if (vzdiff != 0) {
					//compare all 3
					if (Xoverlap / abs(vxdiff) < Yoverlap / abs(vydiff)) {
						if (Xoverlap / abs(vxdiff) < Zoverlap / abs(vzdiff)) {
							first_axis = 1;
						}
						else {
							first_axis = 3;
						}
					}
					else {
						if (Yoverlap / abs(vydiff) < Zoverlap / abs(vzdiff)) {
							first_axis = 2;
						}
						else {
							first_axis = 3;
						}
					}
				}
				else {
					//Compare x and y only
					if (Xoverlap / abs(vxdiff) < Yoverlap / abs(vydiff)) {
						first_axis = 1;
					}
					else {
						first_axis = 2;
					}
				}
			}
			else {
				if (vzdiff != 0) {
					//compare x and z
					if (Xoverlap / abs(vxdiff) < Zoverlap / abs(vzdiff)) {
						first_axis = 1;
					}
					else {
						first_axis = 3;
					}
				}
				else {
					//overlap is x only
					first_axis = 1;
				}
			}
		}
		else {
			if (vydiff != 0) {
				if (vzdiff != 0) {
					//compare y and z
					if (Yoverlap / abs(vydiff) < Zoverlap / abs(vzdiff)) {
						first_axis = 2;
					}
					else {
						first_axis = 3;
					}
				}
				else {
					//overlap is y only
					first_axis = 2;
				}
			}
			else {
				//overlap is z only
				first_axis = 3;
			}
		}

		//proportion of overall pushback and proportion of pushback split across both objects
		float proptotal = 0;
		float prop1 = 0;
		float prop2 = 0;
		if (first_axis == 1) {
			proptotal = (Xoverlap + 0.0001) / abs(vxdiff);
			prop1 = -1 * abs(vx1) / (abs(vx1) + abs(vx2));
			prop2 = -1 * abs(vx2) / (abs(vx1) + abs(vx2));
		}
		else if (first_axis == 2) {
			proptotal = (Yoverlap + 0.0001) / abs(vydiff);
			prop1 = -1 * abs(vy1) / (abs(vy1) + abs(vy2));
			prop2 = -1 * abs(vy2) / (abs(vy1) + abs(vy2));
		}
		else {
			proptotal = (Zoverlap + 0.0001) / abs(vzdiff);
			prop1 = -1 * abs(vz1) / (abs(vz1) + abs(vz2));
			prop2 = -1 * abs(vz2) / (abs(vz1) + abs(vz2));
		}


		FString FilePath3 = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir()) + TEXT("/Debug_Log.txt");
		FString FileContent3 = TEXT("Overlap  vxdiff = ") + FString::SanitizeFloat(vxdiff)
			+ TEXT(" vydiff = ") + FString::SanitizeFloat(vydiff)
			+ TEXT(" vzdiff = ") + FString::SanitizeFloat(vzdiff)
			+ TEXT(" proptotal = ") + FString::SanitizeFloat(proptotal)
			+ TEXT(" prop1 = ") + FString::SanitizeFloat(prop1)
			+ TEXT(" prop2 = ") + FString::SanitizeFloat(prop2)
			+ TEXT(" Xoverlap = ") + FString::SanitizeFloat(Xoverlap)
			+ TEXT(" Yoverlap = ") + FString::SanitizeFloat(Yoverlap)
			+ TEXT(" Zoverlap = ") + FString::SanitizeFloat(Zoverlap)

			+ TEXT("\n");
		if (debuglog == 1) {
			FFileHelper::SaveStringToFile(FileContent3, *FilePath3, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), EFileWrite::FILEWRITE_Append);
		}
		

		proptotal = scalarOverlap / abs(nnvx1 - nnvx2);
		
		PushBackX = PushBackX + (vx1 * proptotal * -1);
		PushBackY = PushBackY + (vy1 * proptotal * -1);
		PushBackZ = PushBackZ + (vz1 * proptotal * -1);

		if (Cast<ABowling_Ball>(OtherActor) != nullptr) {
			float updX = vx2 * proptotal * -1;
			float updY = vy2 * proptotal * -1;
			float updZ = vz2 * proptotal * -1;
			OtherActor->setOffset(updX, updY, updZ);
		}

		//FVector ItemLocation1 = GetActorLocation();
		//ItemLocation1.X = ItemLocation1.X + (vx1 * proptotal * prop1);
		//ItemLocation1.Y = ItemLocation1.Y + (vy1 * proptotal * prop1);
		//ItemLocation1.Z = ItemLocation1.Z + (vz1 * proptotal * prop1);
		//SetActorLocation(ItemLocation1);
		
		//FVector ItemLocation2 = OtherActor->GetActorLocation();
		//ItemLocation2.X = ItemLocation2.X + (vx2 * proptotal * prop2);
		//ItemLocation2.Y = ItemLocation2.Y + (vy2 * proptotal * prop2);
		//ItemLocation2.Z = ItemLocation2.Z + (vz2 * proptotal * prop2);
		//OtherActor->SetActorLocation(ItemLocation2);
		

		//Debug to file
		FString FilePath2 = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir()) + TEXT("/Debug_Log.txt");
		FString FileContent2 = TEXT("End  ffvx1= ") + FString::SanitizeFloat(ffvx1)
			+ TEXT(" ffvy1 = ") + FString::SanitizeFloat(ffvy1)
			+ TEXT(" ffvz1 = ") + FString::SanitizeFloat(ffvz1)
			+ TEXT(" ffvx2 = ") + FString::SanitizeFloat(ffvx2)
			+ TEXT(" ffvy2 = ") + FString::SanitizeFloat(ffvy2)
			+ TEXT(" ffvz2 = ") + FString::SanitizeFloat(ffvz2)

			+ TEXT("\n")
			+ TEXT("\n");
		if (debuglog == 1) {
			FFileHelper::SaveStringToFile(FileContent2, *FilePath2, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), EFileWrite::FILEWRITE_Append);
		}

		FString FilePath12 = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir()) + TEXT("/Debug_Log.txt");
		FString FileContent12 = TEXT("Offset set PushBackX = ") + FString::SanitizeFloat(vx1 * proptotal * -1)
			+ TEXT(" PushBackY = ") + FString::SanitizeFloat(vy1 * proptotal * -1)
			+ TEXT(" PushBackZ = ") + FString::SanitizeFloat(vz1 * proptotal * -1)
			+ TEXT(" Name = ") + GetName()
			+ TEXT("\n")
			+ TEXT("\n");
		if (debuglog == 1) {
			//if (OtherActor->GetName() == "Bowling_Ball_3") {
			FFileHelper::SaveStringToFile(FileContent12, *FilePath12, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), EFileWrite::FILEWRITE_Append);
			//}
		}
		// Debug checks
		/*
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Other location: %s"), *OtherLocation.ToString()));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("OverLappedLocation : %s"), *OverLappedLocation.ToString()));
		*/
//	}

}

void APins::PhysicsCollision(APins* Pins, ABowling_Ball* BowlingBall)
{

	//Debug to file
	FString FilePath = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir()) + TEXT("/Debug_Log.txt");
	FString FileContent = TEXT("Collision decected =  name1 = ") + GetName()
		+ TEXT(" name2 = ") + BowlingBall->GetName()

		+ TEXT("\n");
	if (debuglog == 1) {
		FFileHelper::SaveStringToFile(FileContent, *FilePath, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), EFileWrite::FILEWRITE_Append);
	}
	Pins->GetVelocity();

	BowlingBall->getVelocityX();
}

// Getters
float APins::getVelocityX()
{
	return VelocityX;
}
float APins::getVelocityY()
{
	return VelocityY;
}
float APins::getVelocityZ()
{
	return VelocityZ;
}
int APins::getMass()
{
	return Mass;
}
// Set velocity function
void APins::setVelocity(float setX, float setY, float setZ)
{
	VelocityX = setX;
	VelocityY = setY;
	VelocityZ = setZ;
}
//Get overlap functions
float APins::getPushBackX()
{
	return PushBackX;
}
float APins::getPushBackY()
{
	return PushBackY;
}
float APins::getPushBackZ()
{
	return PushBackZ;
}
// Set overlap functions
void APins::setOffset(float setX, float setY, float setZ)
{
	PushBackX += setX;
	PushBackY += setY;
	PushBackZ += setZ;
}
