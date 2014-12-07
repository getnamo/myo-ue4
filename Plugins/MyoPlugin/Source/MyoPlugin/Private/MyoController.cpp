#include "MyoPluginPrivatePCH.h"
#include "MyoDelegate.h"
#include "MyoController.h"

UMyoController::UMyoController(const FObjectInitializer &init) : UObject(init)
{
}

bool UMyoController::isOnLeftArm()
{
	return arm == MyoArm::MYO_ARM_LEFT;
}

bool UMyoController::isOnRightArm()
{
	return arm == MyoArm::MYO_ARM_RIGHT;
}

void UMyoController::CalibrateArmOrientation(FRotator direction)
{
	_myoDelegate->MyoCalibrateArmOrientation(this->myoId, direction);
}

void UMyoController::VibrateDevice(MyoVibrationType type)
{
	_myoDelegate->MyoVibrateDevice(this->myoId, type);
}

void UMyoController::Unlock(MyoUnlockType type)
{
	_myoDelegate->MyoUnlockMyo(this->myoId, type);
}
void UMyoController::Lock()
{
	_myoDelegate->MyoLockMyo(this->myoId);
}

void UMyoController::setFromMyoDeviceData(MyoDeviceData* data)
{
	this->pose = (MyoPose)data->pose;
	
	this->acceleration = data->acceleration;	
	this->orientation = data->orientation;
	this->gyro = data->gyro;

	this->arm = (MyoArm)data->arm;
	this->xDirection = (MyoArmDirection)data->xDirection;

	this->armAcceleration = data->armAcceleration;
	this->armOrientation = data->armOrientation;
	this->armGyro = data->armGyro;
	this->armSpaceCorrection = data->armSpaceCorrection;

	this->bodySpaceNullAcceleration = data->bodySpaceNullAcceleration;
}