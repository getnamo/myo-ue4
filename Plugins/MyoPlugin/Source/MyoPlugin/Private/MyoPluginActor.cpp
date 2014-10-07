#include "MyoPluginPrivatePCH.h"

#include "Engine.h"
#include "GameFramework/Actor.h"
#include "MyoPluginActor.h"

//Constructor/Initializer
AMyoPluginActor::AMyoPluginActor(const FPostConstructInitializeProperties& PCIP)
: Super(PCIP)
{
	PrimaryActorTick.bCanEverTick = true;
}

//Three mandatory overrides
void AMyoPluginActor::BeginPlay()
{
	Super::BeginPlay();
	MyoStartup();

	//Actors by default aren't attached to the input chain, so we enable input for this actor to forward Key and Gamepad Events
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	EnableInput(PC);
}

void AMyoPluginActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	MyoShutdown();
}

void AMyoPluginActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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
void AMyoPluginActor::LatestData(int32 myoId, int32& Pose, FVector& Acceleration, FRotator& Orientation, FVector& Gyro,
	int32& Arm, int32& xDirection,
	FVector& ArmAcceleration, FRotator& ArmOrientation, FVector& ArmGyro, FRotator& ArmCorrection,
	FVector& BodySpaceAcceleration)
{
	MyoDelegateBlueprint::LatestData(myoId, Pose, Acceleration, Orientation, Gyro, Arm, xDirection, ArmAcceleration, ArmOrientation, ArmGyro, ArmCorrection, BodySpaceAcceleration);
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
void AMyoPluginActor::ConvertToMyoOrientationSpace(FRotator orientation, FRotator& converted)
{
	MyoDelegateBlueprint::ConvertToMyoOrientationSpace(orientation, converted);
}
void AMyoPluginActor::CalibrateArmOrientation(int32 myoId, FRotator direction)
{
	MyoDelegateBlueprint::CalibrateArmOrientation(myoId, direction);
}