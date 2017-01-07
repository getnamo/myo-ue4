#include "MyoPluginPrivatePCH.h"
#include "FMyoInputDevice.h"
#include "MyoUtility.h"
#include "MyoLambdaRunnable.h"

#define MYO_RUNTIME_MS 10	//default to 100 fps rate

#pragma region FMyoInputDevice

FMyoInputDevice::FMyoInputDevice(const TSharedRef< FGenericApplicationMessageHandler >& InMessageHandler) : MessageHandler(InMessageHandler)
{
	//Startup the background handler
	bRunning = false;

	UE_LOG(MyoPluginLog, Log, TEXT("Myo Input device booting up."));

	//Start our background threading
	FMyoLambdaRunnable::RunLambdaOnBackGroundThread([&] 
	{
		MyoIdCounter = 0;	//reset the id counter

		//initialize hub
		MyoHub = new Hub("com.epicgames.unrealengine");

		//did we initialize correctly?
		if (MyoHub->lastInitCausedError)
		{
			UE_LOG(MyoPluginLog, Log, TEXT("Hub initialization failed. Do you have Myo Connect installed and updated?"));
			return;
		}

		MyoHub->addListener(this);
		MyoHub->setLockingPolicy(Hub::lockingPolicyNone);	//default to no locking policy

		UE_LOG(MyoPluginLog, Log, TEXT("Myo Initialized, thread loop started."));

		bRunning = true;

		//Start thread loop
		while (bRunning)
		{
			MyoHub->run(MYO_RUNTIME_MS);
		}

		UE_LOG(MyoPluginLog, Log, TEXT("Myo thread loop stopped."));

		MyoHub->removeListener(this);
		delete MyoHub;
	});
}

FMyoInputDevice::~FMyoInputDevice()
{
	bRunning = false;
}

void FMyoInputDevice::Tick(float DeltaTime)
{

}

void FMyoInputDevice::SendControllerEvents()
{
	//We use an external thread to forward the events, this is unused
}

void FMyoInputDevice::ParseEvents()
{

}

void FMyoInputDevice::SetMessageHandler(const TSharedRef< FGenericApplicationMessageHandler >& InMessageHandler)
{
	MessageHandler = InMessageHandler;
}

bool FMyoInputDevice::Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar)
{
	return false;
}

void FMyoInputDevice::SetChannelValue(int32 ControllerId, FForceFeedbackChannelType ChannelType, float Value)
{

}

void FMyoInputDevice::SetChannelValues(int32 ControllerId, const FForceFeedbackValues &values)
{

}

#pragma endregion FMyoInputDevice

#pragma region Components

void FMyoInputDevice::AddComponentDelegate(UMyoControllerComponent* Component)
{
	ComponentDelegates.Add(Component);
}

void FMyoInputDevice::RemoveComponentDelegate(UMyoControllerComponent* Component)
{
	ComponentDelegates.Remove(Component);
}

void FMyoInputDevice::RunFunctionOnComponents(TFunction<void(UMyoControllerComponent*)> InFunction)
{
	const TArray<UMyoControllerComponent*>& SafeComponentDelegates = ComponentDelegates;
	FMyoLambdaRunnable::RunShortLambdaOnGameThread([&, SafeComponentDelegates]
	{
		for (auto Component : SafeComponentDelegates)
		{
			//Call the function on the game thread
			InFunction(Component);
		}
	});
}

#pragma endregion Components

#pragma region DeviceListener

//Note all these events arrive on the background thread, when we emit threads to the 
//ComponentDelegates, we need to run a TaskGraph lambda on the game-thread to ensure no funny business

void FMyoInputDevice::onPair(Myo* myo, uint64_t timestamp, FirmwareVersion firmwareVersion)
{
	//Track paired myos
	PairedMyos.Add(myo);

	//Add id links
	MyoIdCounter++;				//always bump the counter
	int32 MyoId = MyoIdCounter;
	MyoToIdMap.Add(myo, MyoId);
	IdToMyoMap.Add(MyoId, myo);

	//Add the map data container
	FMyoControllerData DefaultData;
	MyoDataMap.Add(myo, DefaultData);
	
	UE_LOG(MyoPluginLog, Log, TEXT("Paired."));

	RunFunctionOnComponents([&, DefaultData](UMyoControllerComponent* Component)
	{
		//todo: emit locally stored MyoComponents
		Component->OnPair.Broadcast(DefaultData);
	});
}
void FMyoInputDevice::onUnpair(Myo* myo, uint64_t timestamp)
{
	//Remove the myo and links
	FMyoControllerData MyoData = MyoDataMap[myo];
	MyoDataMap.Remove(myo);
	IdToMyoMap.Remove(IdForMyo(myo));
	MyoToIdMap.Remove(myo);
	PairedMyos.Remove(myo);

	UE_LOG(MyoPluginLog, Log, TEXT("Unpaired."));
	RunFunctionOnComponents([&, MyoData](UMyoControllerComponent* Component)
	{
		Component->OnUnpair.Broadcast(MyoData);
	});
	
}

void FMyoInputDevice::onConnect(Myo* myo, uint64_t timestamp, FirmwareVersion firmwareVersion)
{
	ConnectedMyos.Add(myo);

	UE_LOG(MyoPluginLog, Log, TEXT("onConnect."));
}

void FMyoInputDevice::onDisconnect(Myo* myo, uint64_t timestamp)
{
	ConnectedMyos.Remove(myo);

	UE_LOG(MyoPluginLog, Log, TEXT("onDisconnect."));
}

void FMyoInputDevice::onArmSync(Myo* myo, uint64_t timestamp, Arm arm, XDirection xDirection, float rotation, WarmupState warmupState)
{
	UE_LOG(MyoPluginLog, Log, TEXT("onArmSync."));

	//Register

	RunFunctionOnComponents([&](UMyoControllerComponent* Component)
	{
		//Component->OnArmSync.Broadcast()
	});
}

void FMyoInputDevice::onArmUnsync(Myo* myo, uint64_t timestamp)
{
	UE_LOG(MyoPluginLog, Log, TEXT("onArmUnsync."));
}

void FMyoInputDevice::onUnlock(Myo* myo, uint64_t timestamp)
{
	UE_LOG(MyoPluginLog, Log, TEXT("onUnlock."));
}

void FMyoInputDevice::onLock(Myo* myo, uint64_t timestamp)
{
	UE_LOG(MyoPluginLog, Log, TEXT("onLock."));
}

void FMyoInputDevice::onPose(Myo* myo, uint64_t timestamp, Pose pose)
{
	UE_LOG(MyoPluginLog, Log, TEXT("onPose."));
}

void FMyoInputDevice::onOrientationData(Myo* myo, uint64_t timestamp, const Quaternion<float>& rotation)
{

}

void FMyoInputDevice::onAccelerometerData(Myo* myo, uint64_t timestamp, const Vector3<float>& accel)
{

}

void FMyoInputDevice::onGyroscopeData(Myo* myo, uint64_t timestamp, const Vector3<float>& gyro)
{

}

void FMyoInputDevice::onRssi(Myo* myo, uint64_t timestamp, int8_t rssi)
{

}

void FMyoInputDevice::onBatteryLevelReceived(myo::Myo* myo, uint64_t timestamp, uint8_t level)
{
	UE_LOG(MyoPluginLog, Log, TEXT("onBatteryLevelReceived."));
}

void FMyoInputDevice::onEmgData(myo::Myo* myo, uint64_t timestamp, const int8_t* emg)
{

}

void FMyoInputDevice::onWarmupCompleted(myo::Myo* myo, uint64_t timestamp, WarmupResult warmupResult)
{
	UE_LOG(MyoPluginLog, Log, TEXT("onWarmupCompleted."));
}


//Controlling the Myo

void FMyoInputDevice::SetLockingPolicy(EMyoLockingPolicy Policy)
{
	MyoHub->setLockingPolicy((Hub::LockingPolicy)Policy);
}

void FMyoInputDevice::CalibrateOrientation(int32 MyoId, FRotator Direction)
{

}

void FMyoInputDevice::VibrateDevice(int32 MyoId, EMyoVibrationType VibrationType)
{

}

void FMyoInputDevice::UnlockDevice(int32 MyoId, EMyoUnlockType UnlockType)
{

}

void FMyoInputDevice::LockDevice(int32 MyoId)
{

}

void FMyoInputDevice::SetEMGStreamType(int32 MyoId, EMyoStreamEmgType StreamType)
{

}

bool FMyoInputDevice::IsHubEnabled()
{
	return false;
}

void FMyoInputDevice::ShutDownLoop()
{
	bRunning = false;
}

int32 FMyoInputDevice::IdForMyo(Myo* myo)
{
	if (MyoToIdMap.Contains(myo))
	{
		return MyoToIdMap[myo];
	}
	else
	{
		return -1;
	}
}

Myo* FMyoInputDevice::MyoForId(int32 MyoId)
{
	if (IdToMyoMap.Contains(MyoId))
	{
		return IdToMyoMap[MyoId];
	}
	else
	{
		return nullptr;
	}
}

#pragma  endregion DeviceListener

