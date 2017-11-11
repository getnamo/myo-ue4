#include "MyoPluginPrivatePCH.h"
#include "FMyoInputDevice.h"
#include "MyoUtility.h"

#include "IMyoPlugin.h"
#include "FMyoPlugin.h"

#define LOCTEXT_NAMESPACE "MyoPlugin"

//Private API - This is where the magic happens

//Init and Runtime
void FMyoPlugin::StartupModule()
{
	UE_LOG(MyoPluginLog, Log, TEXT("Myo Plugin startup."));

	//Register all input mapping keys and axes
	MyoUtility::AddMyoFKeys();

	bInputCreated = false;

	IModularFeatures::Get().RegisterModularFeature(IInputDeviceModule::GetModularFeatureName(), this);
}

#undef LOCTEXT_NAMESPACE

void FMyoPlugin::ShutdownModule()
{
	if (MyoInputDevice.IsValid())
	{
		MyoInputDevice->ShutDownLoop();
	}

	UE_LOG(MyoPluginLog, Log, TEXT("Myo Plugin clean shutdown."));
	IModularFeatures::Get().UnregisterModularFeature(IInputDeviceModule::GetModularFeatureName(), this);
}



void FMyoPlugin::AddComponentDelegate(UMyoControllerComponent* Component)
{
	if (bInputCreated)
	{
		MyoInputDevice->AddComponentDelegate(Component);
	}
	else
	{
		DeferredDelegates.Add(Component);
	}
}

void FMyoPlugin::RemoveComponentDelegate(UMyoControllerComponent* Component)
{
	if (bInputCreated)
	{
		MyoInputDevice->RemoveComponentDelegate(Component);
	}
	else
	{
		DeferredDelegates.Remove(Component);
	}
}

void FMyoPlugin::UnlockMyo(UMyoController* Controller)
{
	MyoInputDevice->UnlockDevice(Controller->MyoData.MyoId, MYO_UNLOCK_HOLD);
}

void FMyoPlugin::LockMyo(UMyoController* Controller)
{
	MyoInputDevice->LockDevice(Controller->MyoData.MyoId);
}

void FMyoPlugin::SetLockingPolicy(EMyoLockingPolicy Policy)
{
	MyoInputDevice->SetLockingPolicy(Policy);
}

void FMyoPlugin::SetStreamEmgType(UMyoController* Controller, EMyoStreamEmgType StreamType)
{
	MyoInputDevice->SetEMGStreamType(Controller->MyoData.MyoId, StreamType);
}

void FMyoPlugin::VibrateMyo(UMyoController* Controller, EMyoVibrationType VibrationType)
{
	MyoInputDevice->VibrateDevice(Controller->MyoData.MyoId, VibrationType);
}

void FMyoPlugin::CalibrateOrientation(int32 MyoId, FRotator Direction)
{
	MyoInputDevice->CalibrateOrientation(MyoId, Direction);
}

UMyoController* FMyoPlugin::LeftMyo()
{
	for (auto Controller : MyoInputDevice->Controllers)
	{
		if (Controller->MyoData.Arm == EMyoArm::MYO_ARM_LEFT)
		{
			return Controller;
		}
	}
	//No left paired controllers available
	return nullptr;
}

UMyoController* FMyoPlugin::RightMyo()
{
	for (auto Controller : MyoInputDevice->Controllers)
	{
		if (Controller->MyoData.Arm == EMyoArm::MYO_ARM_RIGHT)
		{
			return Controller;
		}
	}
	//No right paired controllers available
	return nullptr;
}

UMyoController* FMyoPlugin::PrimaryMyo()
{
	//Grab Latest controller
	int32 Max = MyoInputDevice->Controllers.Num();
	if (Max > 0)
	{
		return MyoInputDevice->Controllers[Max - 1];
	}
	return nullptr;
}

UMyoController* FMyoPlugin::MyoForId(int32 MyoId)
{
	return MyoInputDevice->ControllerForId(MyoId);
}

TSharedPtr< class IInputDevice > FMyoPlugin::CreateInputDevice(const TSharedRef< FGenericApplicationMessageHandler >& InMessageHandler)
{
	UE_LOG(MyoPluginLog, Log, TEXT("Creating Input device"));

	MyoInputDevice = MakeShareable(new FMyoInputDevice(InMessageHandler));

	//Delegates may have been added before the input device is created, add them to our input device to handle
	if (DeferredDelegates.Num() > 0)
	{
		for (auto Component : DeferredDelegates)
		{
			MyoInputDevice->AddComponentDelegate(Component);
		}
		DeferredDelegates.Empty();
	}
	bInputCreated = true;

	return TSharedPtr< class IInputDevice >(MyoInputDevice);
}

IMPLEMENT_MODULE(FMyoPlugin, MyoPlugin)