#include "MyoPluginPrivatePCH.h"
#include "FMyoInputDevice.h"
#include "MyoInterface.h"
#include "MyoUtility.h"

#include "IMyoPlugin.h"
#include "FMyoPlugin.h"

#define PLUGIN_VERSION "0.8.0"


//Init and Runtime
void FMyoPlugin::StartupModule()
{
	UE_LOG(MyoPluginLog, Log, TEXT("Using Myo Plugin version %s"), TEXT(PLUGIN_VERSION));

	//Register all input mapping keys and axes
	MyoUtility::AddMyoFKeys();

	bInputCreated = false;
}

void FMyoPlugin::ShutdownModule()
{
	UE_LOG(MyoPluginLog, Log, TEXT("Myo clean shutdown."));
}


void FMyoPlugin::AddComponentDelegate(UMyoComponent* Component)
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

void FMyoPlugin::RemoveComponentDelegate(UMyoComponent* Component)
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

TSharedPtr< class IInputDevice > FMyoPlugin::CreateInputDevice(const TSharedRef< FGenericApplicationMessageHandler >& InMessageHandler)
{
	MyoInputDevice = MakeShareable(new FMyoInputDevice(InMessageHandler));
	return TSharedPtr< class IInputDevice >(MyoInputDevice);

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
}

IMPLEMENT_MODULE(FMyoPlugin, MyoPlugin)