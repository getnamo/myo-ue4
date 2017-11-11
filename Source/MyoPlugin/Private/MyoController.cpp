#include "MyoPluginPrivatePCH.h"
#include "FMyoPlugin.h"
#include "MyoEnum.h"
#include "MyoController.h"

void FMyoControllerData::setFromMyoDeviceData(FMyoDeviceData* Data)
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


UMyoController::UMyoController(const FObjectInitializer &init) : UObject(init)
{
}

bool UMyoController::isOnLeftArm()
{
	return MyoData.Arm == EMyoArm::MYO_ARM_LEFT;
}

bool UMyoController::isOnRightArm()
{
	return MyoData.Arm == EMyoArm::MYO_ARM_RIGHT;
}

void UMyoController::CalibrateArmOrientation(FRotator Direction)
{
	if (IMyoPlugin::IsAvailable())
	{
		IMyoPlugin::Get().CalibrateOrientation(MyoData.MyoId, Direction);
	}
}

void UMyoController::VibrateDevice(EMyoVibrationType Type)
{
	if (IMyoPlugin::IsAvailable())
	{
		IMyoPlugin::Get().VibrateMyo(this, Type);
	}
}

void UMyoController::Unlock(EMyoUnlockType Type)
{
	if (IMyoPlugin::IsAvailable())
	{
		IMyoPlugin::Get().UnlockMyo(this);
	}
}
void UMyoController::Lock()
{
	if (IMyoPlugin::IsAvailable())
	{
		IMyoPlugin::Get().LockMyo(this);
	}
}

void UMyoController::SetStreamEmg(EMyoStreamEmgType StreamType)
{
	if (IMyoPlugin::IsAvailable())
	{
		IMyoPlugin::Get().SetStreamEmgType(this, StreamType);
	}
}