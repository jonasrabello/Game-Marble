// Fill out your copyright notice in the Description page of Project Settings.

#include "Coin.h"
#include "Marble.h"
#include "Components/StaticMeshComponent.h"
#include "Components/AudioComponent.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
ACoin::ACoin() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create a root component to attach other components
	CoinStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	CoinStaticMesh->SetStaticMesh((ConstructorHelpers::FObjectFinderOptional<UStaticMesh>(TEXT("/Game/StarterContent/Shapes/SM_Coin.SM_Coin"))).Get());
	RootComponent = CoinStaticMesh;

	// Collision
	CoinStaticMesh->OnComponentBeginOverlap.AddDynamic(this, &ACoin::OnOverlapBegin);

	///////////////////////////////////////////// Sound ////////////////////////////////////////////////
	//// Load the soudn for colision
	//static ConstructorHelpers::FObjectFinder<USoundCue> soundCue(TEXT("/Game/ProjectAssets/Sounds/Pinball_Bell01_Cue.Pinball_Bell01_Cue"));

	//// Store a reference to the Cue asset for later
	//colisionCue = soundCue.Object;

	//// Create the Audio Component that wraps our Cue and allows us to access and interact with it trouhg code
	//colisionSound = CreateDefaultSubobject<UAudioComponent>(TEXT("ColisionSound"));

	//// Stop sound from playing when created
	//colisionSound->bAutoActivate = false;

	//// Attach Audio Component so it follows player around
	//colisionSound->SetupAttachment(RootComponent);

	//// Position Audio Component in front of player
	//colisionSound->SetRelativeLocation(FVector(100.0f, 50.0f, 0.0f));
	///////////////////////////////////////////// Sound ////////////////////////////////////////////////

	// Set speed rotation around Y axis
	rot = FRotator(0.0f, 50.0f, 0.0f);

	coinPoints = 10;
}

void ACoin::PostInitializeComponents() {
	Super::PostInitializeComponents();

	///////////////////////////////////////////// Sound ////////////////////////////////////////////////
	//// Set sound 
	//if (colisionCue->IsValidLowLevelFast()) {
	//	colisionSound->SetSound(colisionCue);		
	//}
	///////////////////////////////////////////// Sound ////////////////////////////////////////////////
}

// Called when the game starts or when spawned
void ACoin::BeginPlay() {
	Super::BeginPlay();
}

// Called every frame
void ACoin::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	// Rotate around Y axis with speed 50
	FRotator newrot = GetActorRotation() + (rot * DeltaTime);
	SetActorRotation(newrot);
}

void ACoin::OnOverlapBegin(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) {

	// If the sphere colides to the coin, plays a sound and destroy the coin
	AMarble* marble = Cast<AMarble>(OtherActor);
	if (marble) {
		marble->PlayCollisionSound();
		marble->UpdateScore(coinPoints);
		this->Destroy();
		//UE_LOG(LogTemp, Log, TEXT("Derp"));
	}
}
