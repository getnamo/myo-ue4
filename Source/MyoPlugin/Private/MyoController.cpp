#include "MyoPluginPrivatePCH.h"
#include "MyoEnum.h"
#include "MyoController.h"

UMyoController::UMyoController(const FObjectInitializer &init) : UObject(init)
{
}

bool UMyoController::isOnLeftArm()
{
	return Arm == EMyoArm::MYO_ARM_LEFT;
}

bool UMyoController::isOnRightArm()
{
	return Arm == EMyoArm::MYO_ARM_RIGHT;
}

void UMyoController::CalibrateArmOrientation(FRotator Direction)
{
	PrivateMyoDelegate->MyoCalibrateArmOrientation(this->MyoId, Direction);
}

void UMyoController::VibrateDevice(EMyoVibrationType Type)
{
	PrivateMyoDelegate->MyoVibrateDevice(this->MyoId, Type);
}

void UMyoController::Unlock(EMyoUnlockType Type)
{
	PrivateMyoDelegate->MyoUnlockMyo(this->MyoId, Type);
}
void UMyoController::Lock()
{
	PrivateMyoDelegate->MyoLockMyo(this->MyoId);
}

void UMyoController::SetStreamEmg(EMyoStreamEmgType StreamType)
{
	PrivateMyoDelegate->MyoSetStreamEmg(this->MyoId, StreamType);
}

void UMyoController::setFromMyoDeviceData(FMyoDeviceData* Data)
{
	this->Pose = Data->Pose;
	
	this->Acceleration = Data->Acceleration;
	this->Orientation = Data->Orientation;
	this->Gyro = Data->Gyro;

	this->Arm = Data->Arm;
	this->ArmDirection = Data->ArmDirection;

	this->ArmAcceleration = Data->ArmAcceleration;
	this->ArmOrientation = Data->ArmOrientation;
	this->ArmGyro = Data->ArmGyro;
	this->ArmSpaceCorrection = Data->ArmSpaceCorrection;

	this->BodySpaceNullAcceleration = Data->BodySpaceNullAcceleration;
}