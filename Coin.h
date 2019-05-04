// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sound/SoundCue.h"
#include "GameFramework/Actor.h"
#include "Coin.generated.h"

UCLASS()
class PROJECT2_API ACoin : public AActor {
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACoin();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// ============================= Jonas Begin =============================
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* CoinStaticMesh;
	//UPROPERTY(BlueprintReadOnly, Category = "Audio")
	//	USoundCue *colisionCue;
	//UPROPERTY(BlueprintReadOnly, Category = "Audio")
	//	UAudioComponent *colisionSound;

	FRotator rot;
	// ============================= Jonas End ===============================

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// ============================= Jonas Begin =============================
	virtual void PostInitializeComponents() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int coinPoints;

	//Collision
	UFUNCTION()
		virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
			UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
			const FHitResult& SweepResult);
	// ============================= Jonas End ===============================
};

