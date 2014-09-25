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

/*Blueprint callable functions implementation passthrough, required*/

void AMyoPlayerController::VibrateDevice(int32 myoId, int32 type)
{
	MyoDelegateBlueprint::VibrateDevice(myoId, type);
}
bool AMyoPlayerController::IsHubEnabled()
{
	return MyoDelegateBlueprint::IsHubEnabled();
}
void AMyoPlayerController::LatestData(int32 myoId, int32& Pose, FVector& Acceleration, FRotator& Orientation, FVector& Gyro,
	int32& Arm, int32& xDirection,
	FVector& ArmAcceleration, FRotator& ArmOrientation, FVector& ArmGyro, FRotator& ArmCorrection,
	FVector& BodySpaceAcceleration)
{
	MyoDelegateBlueprint::LatestData(myoId, Pose, Acceleration, Orientation, Gyro, Arm, xDirection, ArmAcceleration, ArmOrientation, ArmGyro, ArmCorrection, BodySpaceAcceleration);
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
void AMyoPlayerController::ConvertToMyoOrientationSpace(FRotator orientation, FRotator& converted)
{
	MyoDelegateBlueprint::ConvertToMyoOrientationSpace(orientation, converted);
}
void AMyoPlayerController::CalibrateArmOrientation(int32 myoId)
{
	MyoDelegateBlueprint::CalibrateArmOrientation(myoId);
}