#include "MyoPluginPrivatePCH.h"

#include "Engine.h"
#include "GameFramework/Actor.h"
#include "IMyoPlugin.h"
#include "MyoPluginActor.h"

//Constructor/Initializer
AMyoPluginActor::AMyoPluginActor(const FPostConstructInitializeProperties& PCIP)
: Super(PCIP)
{
	PrimaryActorTick.bCanEverTick = true;
}

//Test, can we get the plugin data?
void AMyoPluginActor::BeginPlay()
{
	Super::BeginPlay();

	MyoStartup();

	//Actors by default aren't attached to the input chain, so we enable input for this actor to forward Key and Gamepad Events
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	EnableInput(PC);
}

void AMyoPluginActor::Tick(float DeltaTime)
{
	MyoTick(DeltaTime);
}

/*Blueprint callable functions implementation passthrough, required*/

void AMyoPluginActor::VibrateDevice(int32 myoId, int32 type)
{
	MyoDelegateBlueprint::VibrateDevice(myoId, type);
}
bool AMyoPluginActor::IsHubEnabled()
{
	return MyoDelegateBlueprint::IsHubEnabled();
}
void AMyoPluginActor::LatestData(int32 myoId, int32& pose, FVector& Acceleration, FRotator& Rotation, FVector& Gyro, int32& Arm)
{
	MyoDelegateBlueprint::LatestData(myoId, pose, Acceleration, Rotation, Gyro, Arm);
}
void AMyoPluginActor::WhichArm(int32 myoId, int32& Arm)
{
	MyoDelegateBlueprint::WhichArm(myoId, Arm);
}
void AMyoPluginActor::LeftMyoId(bool& available, int32& myoId)
{
	MyoDelegateBlueprint::LeftMyoId(available, myoId);
}
void AMyoPluginActor::RightMyoId(bool& available, int32& myoId)
{
	MyoDelegateBlueprint::RightMyoId(available, myoId);
}
void AMyoPluginActor::ConvertToRawOrientation(FRotator orientation, FRotator& converted)
{
	MyoDelegateBlueprint::ConvertToRawOrientation(orientation, converted);
}