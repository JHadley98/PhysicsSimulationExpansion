// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Bowling_Ball.generated.h"

class APins;

UCLASS()
class P_SIM_RIGID_BODY_API ABowling_Ball : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABowling_Ball();

	// Set float: PositionX, Velocity X, Y, Z and Drag
private:
	float VelocityX = 10;
	float VelocityY = 0;
	float VelocityZ = 0;
	float Mass = 10;
	// Apply drag value to bowling bowl so eventually the velocity will get to zero stopping the movement of the bowling ball
	float Drag = 0.005;
	//Restitution is the restitution coefficient (this is between 0 and 1 with 1 being perfectly elastic collision.
	//As there will be some friction and some sound from the pins and bowl then this is just under 1
	float Restitution = 0.9;

	//flag to turn logging on and off
	int debuglog = 0;

	//Set up variables to pushback item in the case of overlap
	//This is calculated on overlap but executed when object naturally moves on tick process
	float PushBackX = 0;
	float PushBackY = 0;
	float PushBackZ = 0;

	//Variables to store change in velocity
	//Velocity not updated immediately to allow for collision with more than 1 item at the same time
	float deltavx = 0;
	float deltavy = 0;
	float deltavz = 0;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UStaticMeshComponent* Mesh;

	UFUNCTION()
		void OnOverlap(AActor* OverlappedActor, APins* OtherActor);
	// Getters and Setters
	UFUNCTION()
	float getVelocityX();
	float getVelocityY();
	float getVelocityZ();
	void setVelocity(float setX, float setY, float setZ);
	int getMass();
	float getPushBackX();
	float getPushBackY();
	float getPushBackZ();
	void setOffset(float setpbX, float setpbY, float setpbZ);

	float getdeltavx();
	float getdeltavy();
	float getdeltavz();
	void setdeltav(float setdvX, float setdvY, float setdvZ);
};
