#include "MyoPluginPrivatePCH.h"

#include "MyoDelegate.h"
#include "MyoPlayerController.h"

//Constructor/Initializer
AMyoPlayerController::AMyoPlayerController(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	PrimaryActorTick.bCanEverTick = true;
}

//Three mandatory overrides
void AMyoPlayerController::BeginPlay()
{
	Super::BeginPlay();
	ValidSelfPointer = this;	//required from v0.7
	MyoStartup();
}

void AMyoPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	MyoShutdown();
}

void AMyoPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	MyoTick(DeltaTime);
}

//Functions forwards, required implementations
bool AMyoPlayerController::IsHubEnabled()
{
	return MyoIsHubEnabled();
}

UMyoController* AMyoPlayerController::LeftMyo()
{
	return MyoLeftMyo();
}

UMyoController* AMyoPlayerController::RightMyo()
{
	return MyoRightMyo();
}

UMyoController* AMyoPlayerController::PrimaryMyo()
{
	return MyoPrimaryMyo();
}

void AMyoPlayerController::ConvertToMyoOrientationSpace(FRotator orientation, FRotator& converted)
{
	return MyoConvertToMyoOrientationSpace(orientation, converted);
}