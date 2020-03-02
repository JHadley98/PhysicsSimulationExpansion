// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Pins.generated.h"

class ABowling_Ball;

UCLASS()
class P_SIM_RIGID_BODY_API APins : public AActor
{
	GENERATED_BODY()


public:
	// Sets default values for this actor's properties
	APins();

private:
	// Float variables: Velocity X, Y, Z and Drag
	// Set velocity values for X, Y, Z to be zero so the pins don't initially move 
	float VelocityX = 0;
	float VelocityY = 0;
	float VelocityZ = 0;
	float Mass = 10;

	//flag to turn logging on and off
	int debuglog = 0;

	/*
	Apply drag value to pins so eventually the velocity of the pins aquired from
	the collision by the bowling ball can then reach zero making the pins movement stop
	*/
	float Drag = 0.005;
	//Restitution is the restitution coefficient (this is between 0 and 1 with 1 being perfectly elastic collision.
	//As there will be some friction and some sound from the pins and bowl then this is just under 1
	float Restitution = 0.9;

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

	float potted = 0;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		USceneComponent* Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UStaticMeshComponent* PinsMesh;

	UFUNCTION()
		void OnOverlap(AActor* OverlappedActor, ABowling_Ball* OtherActor);

	UFUNCTION()
		void PhysicsCollision(APins* Pins, ABowling_Ball* BowlingBall);

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
	void setOffset(float setX, float setY, float setZ);

	
};