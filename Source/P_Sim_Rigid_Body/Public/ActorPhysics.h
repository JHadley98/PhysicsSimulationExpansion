// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ActorPhysics.generated.h"

UCLASS()
class P_SIM_RIGID_BODY_API AActorPhysics : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AActorPhysics();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = Location)
		FVector NewLocation;

	// Quat = Quaternion used for actors 3D rotation
	UPROPERTY(EditAnywhere, Category = Location)
		FQuat NewRotation;
};