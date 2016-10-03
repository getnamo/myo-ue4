#include "MyoPluginPrivatePCH.h"
#include "FMyoPlugin.h"
#include "MyoEnum.h"
#include "MyoController.h"

UMyoController::UMyoController(const FObjectInitializer &init) : UObject(init)
{
}

bool UMyoController::isOnLeftArm()
{
	return FrameData.Arm == EMyoArm::MYO_ARM_LEFT;
}

bool UMyoController::isOnRightArm()
{
	return FrameData.Arm == EMyoArm::MYO_ARM_RIGHT;
}

void UMyoController::CalibrateArmOrientation(FRotator Direction)
{
	if (IMyoPlugin::IsAvailable())
	{
		//IMyoPlugin::Get().CalibrateOrientation(this->MyoId, Direction);
	}
}

void UMyoController::VibrateDevice(EMyoVibrationType Type)
{
	if (IMyoPlugin::IsAvailable())
	{
		//IMyoPlugin::Get().VibrateDevice(this->MyoId, Type);
	}
}

void UMyoController::Unlock(EMyoUnlockType Type)
{
	if (IMyoPlugin::IsAvailable())
	{
		//IMyoPlugin::Get().UnlockMyo(this->MyoId, Type);
	}
}
void UMyoController::Lock()
{
	if (IMyoPlugin::IsAvailable())
	{
		//IMyoPlugin::Get().LockMyo(this->MyoId);
	}
}

void UMyoController::SetStreamEmg(EMyoStreamEmgType StreamType)
{
	if (IMyoPlugin::IsAvailable())
	{
		//IMyoPlugin::Get().SetStreamEmg(this->MyoId, StreamType);
	}
}