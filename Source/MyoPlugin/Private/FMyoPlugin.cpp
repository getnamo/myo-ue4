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
	MyoInputDevice->ShutDownLoop();

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

}

void FMyoPlugin::LockMyo(UMyoController* Controller)
{

}

void FMyoPlugin::SetLockingPolicy(EMyoLockingPolicy Policy)
{

}

void FMyoPlugin::SetStreamEmgType(EMyoStreamEmgType StreamType)
{

}

void FMyoPlugin::VibrateMyo(UMyoController* Controller, EMyoVibrationType VibrationType)
{

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