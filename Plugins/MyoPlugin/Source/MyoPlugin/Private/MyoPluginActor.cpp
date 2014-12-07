#include "MyoPluginPrivatePCH.h"

#include "Engine.h"
#include "GameFramework/Actor.h"
#include "MyoPluginActor.h"

//Constructor/Initializer
AMyoPluginActor::AMyoPluginActor(const FObjectInitializer& PCIP)
: Super(PCIP)
{
	PrimaryActorTick.bCanEverTick = true;
}

//Three mandatory overrides
void AMyoPluginActor::BeginPlay()
{
	Super::BeginPlay();

	//Actors by default aren't attached to the input chain, so we enable input for this actor to forward Key and Gamepad Events
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	EnableInput(PC);

	//Required Pointer and Startup
	ValidSelfPointer = this;
	MyoStartup();
}

void AMyoPluginActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	//Required Shutdown
	MyoShutdown();
}

void AMyoPluginActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Required Tick
	MyoTick(DeltaTime);
}


//Functions forwards, required implementations
bool AMyoPluginActor::IsHubEnabled()
{
	return MyoIsHubEnabled();
}

void AMyoPluginActor::SetLockingPolicy(MyoLockingPolicy policy)
{
	MyoSetLockingPolicy(policy);
}


UMyoController* AMyoPluginActor::LeftMyo()
{
	return MyoLeftMyo();
}

UMyoController* AMyoPluginActor::RightMyo()
{
	return MyoRightMyo();
}

UMyoController* AMyoPluginActor::PrimaryMyo()
{
	return MyoPrimaryMyo();
}

void AMyoPluginActor::ConvertToMyoOrientationSpace(FRotator orientation, FRotator& converted)
{
	return MyoConvertToMyoOrientationSpace(orientation, converted);
}


