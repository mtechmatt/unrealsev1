// Copyright Michael Bridges 2019

#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Grabber_AC.h"

#define OUT

// Sets default values for this component's properties
UGrabber_AC::UGrabber_AC()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UGrabber_AC::BeginPlay()
{
	Super::BeginPlay();

	FindPhysicsHandle();
	SetupInputComponent();
}

void UGrabber_AC::FindPhysicsHandle()
{
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (PhysicsHandle)
	{
		//Physics OK
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("No Physics Handle Found on %s!"), *GetOwner()->GetName());
	}
}

void UGrabber_AC::SetupInputComponent(){
	GameInputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (GameInputComponent)
	{
		GameInputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber_AC::Grab);
		GameInputComponent->BindAction("Grab", IE_Released, this, &UGrabber_AC::Release);
	}	
}

void UGrabber_AC::Grab()
{
	UE_LOG(LogTemp, Warning, TEXT("Grabber Pressed!"));

	// Get Player Viewpoint
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(OUT PlayerViewPointLocation, OUT PlayerViewPointRotation	);

	FVector LineTraceEnd = PlayerViewPointLocation + PlayerViewPointRotation.Vector() * Reach;

	FHitResult HitResult = GetFirstPhysicsBodyInReach();
	UPrimitiveComponent* ComponentToGrab = HitResult.GetComponent();

	if(HitResult.GetActor()){
		PhysicsHandle->GrabComponentAtLocation
			(
				ComponentToGrab,
				NAME_None,
				LineTraceEnd
			);
	}
}

void UGrabber_AC::Release()
{
	UE_LOG(LogTemp, Warning, TEXT("Grabber Released!"));
	PhysicsHandle->ReleaseComponent();
}

// Called every frame
void UGrabber_AC::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(OUT PlayerViewPointLocation, OUT PlayerViewPointRotation	);

	FVector LineTraceEnd = PlayerViewPointLocation + PlayerViewPointRotation.Vector() * Reach;

	if(PhysicsHandle->GrabbedComponent)
	{
		PhysicsHandle->SetTargetLocation(LineTraceEnd);
	}
}

FHitResult UGrabber_AC::GetFirstPhysicsBodyInReach() const
{
	// Get Player Viewpoint
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(OUT PlayerViewPointLocation, OUT PlayerViewPointRotation	);

	FVector LineTraceEnd = PlayerViewPointLocation + PlayerViewPointRotation.Vector() * Reach;

	FHitResult Hit;
	// Ray-cast from player out a certain distance (reach)

	FCollisionQueryParams TraceParams(FName(TEXT("")),false, GetOwner());

 	GetWorld()->LineTraceSingleByObjectType(
		OUT Hit,
		PlayerViewPointLocation,
		LineTraceEnd,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParams
	);

	AActor* ActorHit = Hit.GetActor();
	if (ActorHit){
		FString ActorTextName = ActorHit->GetName();
		UE_LOG(LogTemp, Warning, TEXT("Actor is: %s"), *ActorTextName);
	}

	return Hit;
}

