// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
#include "Sound/SoundCue.h"
#include "Components/SphereComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Marble.generated.h"

UCLASS()
class PROJECT2_API AMarble : public APawn {
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AMarble();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// ============================= Arin Begin =============================
	UPROPERTY(EditAnyWhere)
		USphereComponent * SphereComponent;
	/*UPROPERTY(EditAnywhere)
	UStaticMeshComponent* SphereVisual;*/
	UPROPERTY(EditAnywhere)
		USceneComponent* ForceAnchor;
	UPROPERTY(EditAnywhere)
		USpringArmComponent* OurSpringArm;
	// ============================= Arin End ===============================

	// ============================= Jonas Begin =============================
	FVector velocity;				// current velocity	
	FRotator rotation;				// current rotation
	FVector xVel;					// vector of velocity X
	FVector yVel;					// vector of velocity Y
	FVector direction;				// vector of direction X and Y
	FVector startPosition;			// holds the start position of the marble
	TArray<AActor*> actors;			// holds the the actors in the scene
	int maxHealth;					// holds the Max health	
	bool pause;						// controls if pause the game or not
	bool canJump;					// controls if the marble can jump
	// ============================= Jonas End ===============================

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// ============================= Jonas Begin =============================

	virtual void PostInitializeComponents() override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UCameraComponent* OurCamera;
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* OurVisibleComponent;
	UPROPERTY(BlueprintReadOnly, Category = "Audio")
		USoundCue *backgroundSoundCue;
	UPROPERTY(BlueprintReadOnly, Category = "Audio")
		UAudioComponent *backgroundSound;
	UPROPERTY(BlueprintReadOnly, Category = "Audio")
		USoundCue *colisionCue;
	UPROPERTY(BlueprintReadOnly, Category = "Audio")
		UAudioComponent *collisionSound;
	UPROPERTY(EditAnywhere)
		float RotationSpeed = 0.5f;
	UPROPERTY(EditAnywhere)
		float MarbleBoost = 1.0f;
	UPROPERTY(EditAnywhere)
		float MaxBoost = 50.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int checkPoint;					// holds the last check point
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int score;						// Score of points of each coin taken
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int health;						// Health of points
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int maxLife;					// Max life
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int life;						// Life
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector previousV;				// previous velocity
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float collisionSoundVolume;		// Change the collision sound volume
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float backgroundSoundVolume;	// Change the background sound volume
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool playBackground;			// Pause/Play background sound
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool playCollision;				// Play or not collision sound
	UPROPERTY(EditAnywhere, BlueprintReadWrite)		
		TArray<AActor*> resetPoints;	// Holds the target position in the scene
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<AActor*> boxes;			// Holds the the boxes in the scene	
	// ============================= Jonas End ===============================

	// ============================= Arin Begin =============================
	UPROPERTY(EditAnywhere)
		float Speed = 100.0f;
	// ============================= Arin End ===============================

	// ============================= Jonas Begin =============================	
	UFUNCTION(BlueprintCallable)
		void MarbleControl(float deltaTime);
	UFUNCTION(BlueprintCallable)
		void DirectionXAxis(float AxisValue);
	UFUNCTION(BlueprintCallable)
		void AddForceAxisX(float deltaTime);
	UFUNCTION(BlueprintCallable)
		void DirectionYAxis(float AxisValue);
	UFUNCTION(BlueprintCallable)
		void AddForceAxisY(float deltaTime);
	UFUNCTION(BlueprintCallable)
		void SetResetPosition(int position);
	UFUNCTION(BlueprintCallable)
		void MarbleTakeDamage(int points);
	UFUNCTION(BlueprintCallable)
		void PauseGame();
	UFUNCTION(BlueprintCallable)
		void ContinueGame();
	UFUNCTION(BlueprintCallable)
		void RestoreMovement();
	UFUNCTION(BlueprintCallable)
		void SetMaxLife(int number);
	UFUNCTION(BlueprintCallable)
		void SetBackgroundSoundVolume(float multiplier);
	UFUNCTION(BlueprintCallable)
		void SetBackgroundSound(bool play);
	UFUNCTION(BlueprintCallable)
		void SetCollisionSound(bool play);
	UFUNCTION(BlueprintCallable)
		void SetCollisionSoundVolume(float multiplier);
	UFUNCTION(BlueprintCallable)
		void PlaySound();

	// Turn
	void Turn(float direction);
	// Update score
	void UpdateScore(int points);
	// Play Collision Sound
	void PlayCollisionSound();
	// Jump
	void Jump();
	// Boost
	void Boost();
	// Check Position to reset
	void CheckPosition();
	// Check Health
	void CheckHealth();
	// Check Life
	void CheckLife();
	// Filter Boxes
	void FilterBoxes();
	// Reset marble position
	void ResetPosition();
	// Reset boxes in case reset marble position
	void ResetBoxes();	
	// Take one life at time the position is reset
	void TakeLife();
	// Switch true / false when keyboard key B
	void  SwitchPlayBackground();
	// Switch true / false when keyboard key E
	void SwitchPlayCollision();
	// ============================= Jonas End ===============================


};
