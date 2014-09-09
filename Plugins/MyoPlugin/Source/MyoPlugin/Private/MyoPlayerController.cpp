#include "MyoPluginPrivatePCH.h"

#include "MyoDelegate.h"
#include "MyoPlayerController.h"

//Constructor/Initializer
AMyoPlayerController::AMyoPlayerController(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	PrimaryActorTick.bCanEverTick = true;
}

//Test, can we get the plugin data?
void AMyoPlayerController::BeginPlay()
{
	MyoStartup();
}

void AMyoPlayerController::Tick(float DeltaTime)
{
	MyoTick(DeltaTime);
}

/*Blueprint callable functions implementation passthrough, required*/

void AMyoPlayerController::VibrateDevice(int32 myoId, int32 type)
{
	MyoDelegateBlueprint::VibrateDevice(myoId, type);
}
bool AMyoPlayerController::IsHubEnabled()
{
	return MyoDelegateBlueprint::IsHubEnabled();
}
void AMyoPlayerController::LatestData(int32 myoId, int32& pose, FVector& Acceleration, FRotator& Rotation, FVector& Gyro, int32& Arm)
{
	MyoDelegateBlueprint::LatestData(myoId, pose, Acceleration, Rotation, Gyro, Arm);
}
void AMyoPlayerController::WhichArm(int32 myoId, int32& Arm)
{
	MyoDelegateBlueprint::WhichArm(myoId, Arm);
}
void AMyoPlayerController::LeftMyoId(bool& available, int32& myoId)
{
	MyoDelegateBlueprint::LeftMyoId(available, myoId);
}
void AMyoPlayerController::RightMyoId(bool& available, int32& myoId)
{
	MyoDelegateBlueprint::RightMyoId(available, myoId);
}
void AMyoPlayerController::ConvertToRawOrientation(FRotator orientation, FRotator& converted)
{
	MyoDelegateBlueprint::ConvertToRawOrientation(orientation, converted);
}