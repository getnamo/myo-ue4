#include "MyoPluginPrivatePCH.h"
#include "FMyoInputDevice.h"
#include "MyoUtility.h"
#include "MyoLambdaRunnable.h"

#define MYO_RUNTIME_MS 10

#pragma region FMyoInputDevice

FMyoInputDevice::FMyoInputDevice(const TSharedRef< FGenericApplicationMessageHandler >& InMessageHandler) : MessageHandler(InMessageHandler)
{
	bRunnning = false;

	//Start our background threading
	MyoLambdaRunnable::RunLambdaOnBackGroundThread([&] 
	{
		//initialize hub
		MyoHub = new Hub("com.epicgames.unrealengine");

		//did we initialize correctly?
		if (MyoHub->lastInitCausedError)
		{
			UE_LOG(MyoPluginLog, Log, TEXT("Hub initialization failed. Do you have Myo Connect installed and updated?"));
			return;
		}

		MyoHub->addListener(this);

		//Start thread loop
		while (bRunnning)
		{
			MyoHub->run(MYO_RUNTIME_MS);
		}

		MyoHub->removeListener(this);
		delete MyoHub;
	});
}

FMyoInputDevice::~FMyoInputDevice()
{
	bRunnning = false;
}

void FMyoInputDevice::Tick(float DeltaTime)
{

}

void FMyoInputDevice::SendControllerEvents()
{
	ParseEvents();
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

void FMyoInputDevice::AddComponentDelegate(UMyoComponent* Component)
{
	ComponentDelegates.Add(Component);
}

void FMyoInputDevice::RemoveComponentDelegate(UMyoComponent* Component)
{
	ComponentDelegates.Remove(Component);
}

void FMyoInputDevice::RunFunctionOnComponents(TFunction<void(UMyoComponent*)> InFunction)
{
	const TArray<UMyoComponent*>& SafeComponentDelegates = ComponentDelegates;
	MyoLambdaRunnable::RunShortLambdaOnGameThread([&, SafeComponentDelegates]
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
	const int32 MyoId = IdForMyo(myo);
	RunFunctionOnComponents([&](UMyoComponent* Component)
	{
		//todo: emit locally stored MyoComponents
		//Component->B
	});
}
void FMyoInputDevice::onUnpair(Myo* myo, uint64_t timestamp)
{

}

void FMyoInputDevice::onConnect(Myo* myo, uint64_t timestamp, FirmwareVersion firmwareVersion)
{
	//Myo connected, track it
	ConnectedMyos.Add(myo);

	//todo: make/spawn uactor as required

	//emit
}

void FMyoInputDevice::onDisconnect(Myo* myo, uint64_t timestamp)
{
	ConnectedMyos.Remove(myo);
}

void FMyoInputDevice::onArmSync(Myo* myo, uint64_t timestamp, Arm arm, XDirection xDirection, float rotation, WarmupState warmupState)
{

}

void FMyoInputDevice::onArmUnsync(Myo* myo, uint64_t timestamp)
{

}

void FMyoInputDevice::onUnlock(Myo* myo, uint64_t timestamp)
{

}

void FMyoInputDevice::onLock(Myo* myo, uint64_t timestamp)
{

}

void FMyoInputDevice::onPose(Myo* myo, uint64_t timestamp, Pose pose)
{

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

}

void FMyoInputDevice::onEmgData(myo::Myo* myo, uint64_t timestamp, const int8_t* emg)
{

}

void FMyoInputDevice::onWarmupCompleted(myo::Myo* myo, uint64_t timestamp, WarmupResult warmupResult)
{

}

int32 FMyoInputDevice::IdForMyo(Myo* myo)
{
	return ConnectedMyos.Find(myo);
}

myo::Myo* FMyoInputDevice::MyoForId(int32 MyoId)
{
	if(MyoId<ConnectedMyos.Num())
	{
		return ConnectedMyos[MyoId];
	}
	else
	{
		return nullptr;
	}
}

void FMyoInputDevice::SetLockingPolicy(EMyoLockingPolicy Policy)
{

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

#pragma  endregion DeviceListener

