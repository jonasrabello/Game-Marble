// Fill out your copyright notice in the Description page of Project Settings.

#include "Marble.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Components/AudioComponent.h"
#include "Components/InputComponent.h"
#include "EngineUtils.h"
#include "Engine.h"
#include "Engine/TargetPoint.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Components/SkeletalMeshComponent.h"

// Sets default values
AMarble::AMarble() {
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// ============================= Arin Begin =============================
	// Set this pawn to be controlled by the lowest numbered player.	
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
	RootComponent = SphereComponent;
	// ============================= Arin End ===============================

	// ============================= Jonas Begin =============================
	// Collision setup
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SphereComponent->SetSimulatePhysics(true);

	// Create a Static Mesh Component
	OurVisibleComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OurVisibleComponent"));
	OurVisibleComponent->SetupAttachment(RootComponent);

	// Get a mesh for the sphere
	OurVisibleComponent->SetStaticMesh((ConstructorHelpers::FObjectFinderOptional<UStaticMesh>
		(TEXT("/Game/StarterContent/Props/MaterialSphere.MaterialSphere"))).Get());

	OurVisibleComponent->SetMaterial(0,(ConstructorHelpers::FObjectFinderOptional<UMaterialInterface>
		(TEXT("/Game/StarterContent/Materials/M_SimpleMetal.M_SimpleMetal"))).Get());

	// Set this pawn to be controlled by the lowest numbered player.
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	// Create a camera
	OurCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("OurCamera"));

	// Insert a spring arm component
	OurSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("OurSpringArm"));

	// Load the Sound Cue for background
	static ConstructorHelpers::FObjectFinder<USoundCue> soundCue1(TEXT("/Game/ProjectAssets/Sounds/backgroundCue.backgroundCue"));
	// Load the soudn for colision
	static ConstructorHelpers::FObjectFinder<USoundCue> soundCue2(TEXT("/Game/ProjectAssets/Sounds/Pinball_Bell01_Cue.Pinball_Bell01_Cue"));

	// Store a reference to the Cue asset for later
	backgroundSoundCue = soundCue1.Object;
	colisionCue = soundCue2.Object;

	// Create the Audio Component that wraps our Cue and allows us to access and interact with it trouhg code
	backgroundSound = CreateDefaultSubobject<UAudioComponent>(TEXT("BackgroundSound"));
	collisionSound = CreateDefaultSubobject<UAudioComponent>(TEXT("collisionSound"));

	// Stop sound from playing when created
	backgroundSound->bAutoActivate = false;
	collisionSound->bAutoActivate = false;

	// Attach Audio Component so it follows player around
	backgroundSound->SetupAttachment(RootComponent);
	collisionSound->SetupAttachment(RootComponent);

	// Position Audio Component in front of player
	backgroundSound->SetRelativeLocation(FVector(100.0f, 0.0f, 0.0f));
	collisionSound->SetRelativeLocation(FVector(120.0f, 0.0f, 0.0f));
	// ============================= Jonas End ===============================

	// ============================= Arin Begin =============================
	// Set up ForceAnchor
	ForceAnchor = CreateDefaultSubobject<USceneComponent>(TEXT("ForceAnchor"));
	ForceAnchor->SetupAttachment(RootComponent);
	ForceAnchor->bAbsoluteRotation = true;
	// ============================= Arin End ===============================

	// ============================= Jonas Begin =============================
	// Attach the sprim arm to the force anchor component
	OurSpringArm->SetupAttachment(ForceAnchor);
	// Attach our camera and visual object to the root component. Offset and rotate the camera
	OurSpringArm->SetRelativeRotation(FRotator(-45.0f, 0.0f, 0.0f));
	OurSpringArm->TargetArmLength = 500.0f;
	//OurSpringArm->bAbsoluteRotation = false;
	OurCamera->SetupAttachment(OurSpringArm, USpringArmComponent::SocketName);
	// ============================= Jonas End ===============================

	// ============================= Arin Begin =============================
	// Initialize radius
	SphereComponent->InitSphereRadius(50.0f);

	// Assign tag
	SphereComponent->ComponentTags.Add(FName("Player"));

	// Collision setup
	//SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SphereComponent->SetSimulatePhysics(true);

	// Set up our visual component
	OurVisibleComponent->SetRelativeLocation(FVector().ZeroVector);
	// ============================= Arin End ===============================

	// ============================= Jonas Begin =============================
	// Control to play and to stop the sound
	playBackground = true;
	playCollision = true;

	// Initialize score
	score = 0;

	// Initialize check point
	checkPoint = 0;

	// Initilize health
	maxHealth = 100;
	health = maxHealth;

	// Initilize health
	maxLife = 10;
	life = maxLife;

	// Initialize volume of backgroud and collision sound
	backgroundSoundVolume = 1.0f;
	collisionSoundVolume = 1.0f;

	// Initialize can jump
	canJump = false;
	// ============================= Jonas End ===============================

}

// Called when the game starts or when spawned
void AMarble::BeginPlay() {
	Super::BeginPlay();

	// ============================= Jonas Begin =============================
	// Start background sound
	backgroundSound->Play();

	// Pause or play according to playBackground boolean
	PlaySound();

	// Get all target points in the scene and save them in resetPoints
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATargetPoint::StaticClass(), resetPoints);
	resetPoints.Sort();

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), actors);
	FilterBoxes();

	// Initilize start position
	startPosition = GetTransform().GetLocation();			// marble position
	// ============================= Jonas End ===============================
}

// Called every frame
void AMarble::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	// ============================= Jonas Begin ==============================
	if (!pause) {
		// Calculate the new velocity according to the input axis and addforce to the sphere
		MarbleControl(DeltaTime);
		// Check position in case the marble falls then reset position to the last check point
		CheckPosition();
		// Check health in case marble lost its health then reset position to the last check point
		CheckHealth();	
	}
	// ============================= Jonas End ===============================

}

// Called to bind functionality to input
void AMarble::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// ============================= Jonas Begin =============================
	// Respond every frame to the values of our two movement axes (W & S)
	PlayerInputComponent->BindAxis("MoveX", this, &AMarble::DirectionXAxis);
	PlayerInputComponent->BindAxis("MoveY", this, &AMarble::DirectionYAxis);

	// Turn marble right and left (Arrows)
	InputComponent->BindAxis("Turn", this, &AMarble::Turn);

	// Boost (Q)
	PlayerInputComponent->BindAction("Boost", IE_Pressed, this, &AMarble::Boost);

	// Control to play or stop play the sound background (Key B)
	PlayerInputComponent->BindAction("PlayBackground", IE_Pressed, this, &AMarble::SwitchPlayBackground);

	// Control to play or stop play the sound collision (Key E)
	PlayerInputComponent->BindAction("PlayCollision", IE_Pressed, this, &AMarble::SwitchPlayCollision);

	// Jump (Space Bar)
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMarble::Jump);
	// ============================= Jonas End ===============================
}

// ============================= Jonas Begin =============================

void AMarble::PostInitializeComponents() {
	Super::PostInitializeComponents();

	// Set sound 
	if (backgroundSoundCue->IsValidLowLevelFast()) {
		backgroundSound->SetSound(backgroundSoundCue);
	}
	if (colisionCue->IsValidLowLevelFast()) {
		collisionSound->SetSound(colisionCue);
	}
}

void AMarble::DirectionXAxis(float AxisValue) {

	// Calculate the direction X of the sphere and velocity X
	direction.X = FMath::Clamp(AxisValue, -1.0f, 1.0f) * 15000.0f;
}

void AMarble::DirectionYAxis(float AxisValue) {

	// Calculate the direction Y of the sphere and velocity Y
	direction.Y = FMath::Clamp(AxisValue, -1.0f, 1.0f) * 15000.0f;
}

void AMarble::AddForceAxisX(float deltaTime) {

	// Calculate the velocity of force anchor X and direction X
	xVel = (ForceAnchor->GetForwardVector().GetSafeNormal() * (Speed * MarbleBoost * deltaTime)) * direction.X;

	// Remove the boost
	if (MarbleBoost > 1.0f) MarbleBoost = 1.0f;
}

void AMarble::AddForceAxisY(float deltaTime) {

	// Calculate the velocity of force anchor Y and direction Y
	yVel = (ForceAnchor->GetRightVector().GetSafeNormal() * (Speed * deltaTime)) * direction.Y;
}

void AMarble::Turn(float direction) {
	rotation.Yaw += RotationSpeed * direction;
}

// Calculate the new velocity according to the input axis and addforce to the sphere
void AMarble::MarbleControl(float deltaTime) {

	// Calculate marble velocity
	AddForceAxisX(deltaTime);
	AddForceAxisY(deltaTime);
	velocity = xVel + yVel;

	// update previous velocity to restore marble status
	if(velocity != FVector().ZeroVector) previousV = velocity;	

	// Set rotation to the spring arm
	FRotator NewRotation = OurSpringArm->GetComponentRotation();
	NewRotation.Yaw = rotation.Yaw;
	OurSpringArm->SetWorldRotation(NewRotation);

	// Set rotantion to the force anchor
	ForceAnchor->SetWorldRotation(FRotator(0, NewRotation.Yaw, 0));

	// Add force to the marble directions X and Y only
	SphereComponent->AddForce(FVector(velocity.X, velocity.Y, 0.0f));
}

void AMarble::SwitchPlayBackground() {

	// Switch the bool variable
	playBackground = !playBackground;

	PlaySound();
}

void AMarble::SwitchPlayCollision() {

	playCollision = !playCollision;
}

void AMarble::SetBackgroundSound(bool play) {

	playBackground = play;	
}

void AMarble::SetCollisionSound(bool play) {

	playCollision = play;
}

void AMarble::SetCollisionSoundVolume(float multiplier) {

	collisionSoundVolume = multiplier;
}

void AMarble::PlaySound() {

	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("%d"), playBackground));

	if (playBackground)
		// Play Sound
		backgroundSound->SetPaused(false);		
	else
		// Stop playing sound
		backgroundSound->SetPaused(true);
}

void AMarble::PlayCollisionSound() {	
	
	if (playCollision) {
		// Set collision sound volume
		collisionSound->SetVolumeMultiplier(collisionSoundVolume);
		// Play Collison Sound
		collisionSound->Play();
	}
}

void AMarble::Jump() {

	// Add force in Z direction
	if(canJump)	SphereComponent->AddForce(FVector(0.0f, 0.0f, (4000000.0f)));
}

void AMarble::UpdateScore(int points) {
	// Update Score
	score += points;
}

void AMarble::Boost() {
	// Set the boost
	MarbleBoost = MaxBoost;
}

void AMarble::CheckPosition() {

	FVector position = GetTransform().GetLocation();

	if (position.Z < -600.0f) {
		CheckLife();		// Check life in case marble lost all lifes then reset position to the first check point
		ResetPosition();
	}
	else if (checkPoint == 9) { // End of the game
		life = 0;				// Reset all lives
		CheckLife();			// Check life in case marble lost all lifes then reset position to the first check point
		ResetPosition();
	}
}

void AMarble::CheckHealth() {

	if (health <= 0) {
		CheckLife();		// Check life in case marble lost all lifes then reset position to the first check point
		ResetPosition();
	}
}

void AMarble::CheckLife() {

	// Update life
	TakeLife();

	if (life <= 0) {
		checkPoint = 0;		// Returns the to the begining
	}
}

void AMarble::FilterBoxes() {	

	// Get only the actors that are box floor
	for(int i = 0; i < actors.Num(); i++) {
		if (actors[i]->GetName().StartsWith("BP_WoodFloor")) {
			boxes.Add(actors[i]);	
		}
	}

	boxes.Sort();
}

void AMarble::ResetPosition() {

	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("%f, %f, %f"), position.X, position.Y, position.Z));

	if (resetPoints.Num() > checkPoint) {

		// Get target point
		AActor* resetPoint = resetPoints[checkPoint];

		// Get target point position
		FVector position = resetPoint->GetTransform().GetLocation();

		// Reset marble position
		SetActorLocation(position);
		
		// Reset all physics to make the marble stop and the return physics again
		PauseGame();
		ContinueGame();

		// Reset health
		health = maxHealth;

		// Reset boxes according with check point
		ResetBoxes();

		// Reset lifes
		if (life <= 0) {
			life = maxLife;		
		}
	}
}

void AMarble::ResetBoxes() {

	FOutputDeviceNull ar;
	
	// Reset box floor actor and continue moving, because they stops int the game
	if (checkPoint == 2 || life <= 0) {
		for (int i = 0; i < 3; i++) {
			boxes[i]->CallFunctionByNameWithArguments(TEXT("ResetBox"), ar, NULL, true);
		}
	}
	else if (checkPoint == 3 || life <= 0) {
		for (int i = 3; i < 6; i++) {
			boxes[i]->CallFunctionByNameWithArguments(TEXT("ResetBox"), ar, NULL, true);
		}
	}
}

void AMarble::MarbleTakeDamage(int points) {

	health -= points;
}

void AMarble::PauseGame() {

	SphereComponent->SetSimulatePhysics(false);
	pause = true;

	//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, FString::Printf(TEXT("%d"), pause));
}

void AMarble::ContinueGame() {

	SphereComponent->SetSimulatePhysics(true);
	pause = false;

	//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, FString::Printf(TEXT("%d"), pause));
}

void AMarble::RestoreMovement() {

	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("%f, %f, %f"), previousV.X, previousV.Y, previousV.Z));

	//Add impulse to the marble directions X and Y only after pausing game
	SphereComponent->AddImpulse(FVector(previousV.X*0.80f, previousV.Y*0.80f, 0.0f));
}

void AMarble::SetMaxLife(int number) {
	
	maxLife = number;

	// Update also the life in game if the player has not started playing the game yet
	if (resetPoints.Num() > 0) {

		FVector position = GetTransform().GetLocation();			// marble position
	
		// Take just the integer part and not z parte to avoid false negative,
		// Because startPosition.Z, without reason, changes from -23.000000 to -20.000019
		
		int pX = (int)position.X;
		int pY = (int)position.Y;

		int sX = (int)startPosition.X;
		int sY = (int)startPosition.Y;

		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("%f, %f, %f"), position.X, position.Y, position.Z));
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("%f, %f, %f"), startPosition.X, startPosition.Y, startPosition.Z));

		if (pX==sX && pY==sY) {
			life = maxLife;		// update marble life
		}
	}
}

void AMarble::SetBackgroundSoundVolume(float multiplier) {

	backgroundSound->SetVolumeMultiplier(multiplier);
	backgroundSoundVolume = multiplier;
}

void AMarble::TakeLife() {

	life -= 1;
}

void AMarble::SetResetPosition(int position) {

	// Can jump just in the coin area
	if (position == 3 || position == 5)	canJump = true;
	else canJump = false;

	if(checkPoint < position) checkPoint = position;	
}