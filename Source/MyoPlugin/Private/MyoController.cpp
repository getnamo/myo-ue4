#include "MyoPluginPrivatePCH.h"
#include "FMyoPlugin.h"
#include "MyoEnum.h"
#include "MyoController.h"

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