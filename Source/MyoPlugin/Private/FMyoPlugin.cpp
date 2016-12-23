#include "MyoPluginPrivatePCH.h"
#include "FMyoInputDevice.h"
#include "MyoUtility.h"

#include "IMyoPlugin.h"
#include "FMyoPlugin.h"

#define LOCTEXT_NAMESPACE "MyoPlugin"

DEFINE_LOG_CATEGORY_STATIC(MyoPluginLog, Log, All);

//Private API - This is where the magic happens

//Init and Runtime
void FMyoPlugin::StartupModule()
{
	UE_LOG(MyoPluginLog, Log, TEXT("Myo Plugin startup."));

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

void FMyoPlugin::AddComponentDelegate(UMyoControllerComponent* DelegateComponent)
{

}

void FMyoPlugin::RemoveComponentDelegate(UMyoControllerComponent* DelegateComponent)
{

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

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMyoPlugin, MyoPlugin)