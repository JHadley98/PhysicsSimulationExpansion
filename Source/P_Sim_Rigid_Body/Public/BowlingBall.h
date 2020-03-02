// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "BowlingBall.generated.h"

class APins;

UCLASS()
class P_SIM_RIGID_BODY_API ABowlingBall : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABowlingBall();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* Mesh;

	// Movement
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PositionX;
	float VelocityX;
	float VelocityY;
	float VelocityZ;
	float Drag;

	// Getters and Setters
	UFUNCTION()
		float getVelocityX();
		float getVelocitY();
		float getVelocityZ();
		void setVelocity(float setX, float setY, float setZ);
	
		UFUNCTION()
			void OnOverlap(APawn* OverlappedActor, APins* OtherActor);
};
