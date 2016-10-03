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
		//MyoHub

		//did we initialize correctly?

		//Start thread loop
		while (bRunnning)
		{
			MyoHub->run(MYO_RUNTIME_MS);
		}
	});
}

FMyoInputDevice::~FMyoInputDevice()
{

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
	for (auto Component : ComponentDelegates)
	{
		InFunction(Component);
	}
}

#pragma endregion Components

#pragma region DeviceListener

//Note all these events arrive on the background thread, when we emit threads to the 
//ComponentDelegates, we need to run a TaskGraph lambda on the game-thread to ensure no funny business

void FMyoInputDevice::onPair(Myo* myo, uint64_t timestamp, FirmwareVersion firmwareVersion)
{

}
void FMyoInputDevice::onUnpair(Myo* myo, uint64_t timestamp)
{

}

void FMyoInputDevice::onConnect(Myo* myo, uint64_t timestamp, FirmwareVersion firmwareVersion)
{

}

void FMyoInputDevice::onDisconnect(Myo* myo, uint64_t timestamp)
{

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

#pragma  endregion DeviceListener

