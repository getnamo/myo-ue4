#include "MyoPluginPrivatePCH.h"
#include "FMyoInputDevice.h"
#include "MyoUtility.h"
#include "MyoLambdaRunnable.h"

#define MYO_RUNTIME_MS 10	//default to 100 fps rate

#pragma region FMyoInputDevice

bool EmitKeyUpEventForKey(FKey key, int32 user, bool repeat)
{
	FKeyEvent KeyEvent(key, FSlateApplication::Get().GetModifierKeys(), user, repeat, 0, 0);
	return FSlateApplication::Get().ProcessKeyUpEvent(KeyEvent);
}

bool EmitKeyDownEventForKey(FKey key, int32 user, bool repeat)
{
	FKeyEvent KeyEvent(key, FSlateApplication::Get().GetModifierKeys(), user, repeat, 0, 0);
	return FSlateApplication::Get().ProcessKeyDownEvent(KeyEvent);
}

FMyoInputDevice::FMyoInputDevice(const TSharedRef< FGenericApplicationMessageHandler >& InMessageHandler) : MessageHandler(InMessageHandler)
{
	//Startup the background handler
	bRunning = false;
	MyoHub = nullptr;

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
			delete MyoHub;
			MyoHub = nullptr;
			return;
		}

		MyoHub->addListener(this);
		MyoHub->setLockingPolicy(Hub::lockingPolicyNone);	//default to no locking policy

		UE_LOG(MyoPluginLog, Log, TEXT("Myo Initialized, thread loop started."));

		bRunning = true;

		//MyoHub->waitForMyo()	//optimization, wait stall thread?

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
	//We use an external thread to forward the events, this is unused. Should we poll push latest controller data instead?
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
	DefaultData.MyoId = MyoId;
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

	const FMyoControllerData& MyoData = MyoDataMap[myo];

	RunFunctionOnComponents([&, MyoData](UMyoControllerComponent* Component)
	{
		Component->OnConnect.Broadcast(MyoData);
	});
}

void FMyoInputDevice::onDisconnect(Myo* myo, uint64_t timestamp)
{
	ConnectedMyos.Remove(myo);

	UE_LOG(MyoPluginLog, Log, TEXT("onDisconnect."));

	const FMyoControllerData& MyoData = MyoDataMap[myo];

	RunFunctionOnComponents([&, MyoData](UMyoControllerComponent* Component)
	{
		Component->OnDisconnect.Broadcast(MyoData);
	});
}

void FMyoInputDevice::onArmSync(Myo* myo, uint64_t timestamp, Arm arm, XDirection xDirection, float rotation, WarmupState warmupState)
{
	UE_LOG(MyoPluginLog, Log, TEXT("onArmSync."));

	//Update the data for the myo
	FMyoControllerData Data; 
	DataForMyo(Data, myo);
	Data.Arm = (EMyoArm)arm;
	Data.ArmDirection = (EMyoArmDirection)xDirection;

	RunFunctionOnComponents([&, Data](UMyoControllerComponent* Component)
	{
		Component->OnArmSync.Broadcast(Data, Data.Arm, Data.ArmDirection);
	});
}

void FMyoInputDevice::onArmUnsync(Myo* myo, uint64_t timestamp)
{
	UE_LOG(MyoPluginLog, Log, TEXT("onArmUnsync."));

	const FMyoControllerData& MyoData = MyoDataMap[myo];

	RunFunctionOnComponents([&, MyoData](UMyoControllerComponent* Component)
	{
		Component->OnArmUnsync.Broadcast(MyoData);
	});
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

	FMyoControllerData& MyoData = MyoDataMap[myo];
	
	ReleasePose(MyoData.Pose);
	
	MyoData.Pose = ConvertedPose(pose.type());

	PressPose(MyoData.Pose);

	RunFunctionOnComponents([&, MyoData](UMyoControllerComponent* Component)
	{
		Component->OnPoseChanged.Broadcast(MyoData, MyoData.Pose);
	});
}

void FMyoInputDevice::onOrientationData(Myo* myo, uint64_t timestamp, const Quaternion<float>& rotation)
{
	FMyoControllerData& MyoData = MyoDataMap[myo];
	MyoData.Orientation = FQuat(rotation.x(), rotation.y(), rotation.z(), rotation.w()).Rotator();
}

void FMyoInputDevice::onAccelerometerData(Myo* myo, uint64_t timestamp, const Vector3<float>& accel)
{
	FMyoControllerData& MyoData = MyoDataMap[myo];
	MyoData.Acceleration = FVector(accel.x(), accel.y(), accel.z());

	//for now broadcast move on accelerometer data (later move this into ticking...
	RunFunctionOnComponents([&, MyoData](UMyoControllerComponent* Component)
	{
		Component->OnArmMoved.Broadcast(MyoData, MyoData.Acceleration, MyoData.Orientation, MyoData.Gyro);
	});
}

void FMyoInputDevice::onGyroscopeData(Myo* myo, uint64_t timestamp, const Vector3<float>& gyro)
{
	FMyoControllerData& MyoData = MyoDataMap[myo];
	MyoData.Gyro = FVector(gyro.x(), gyro.y(), gyro.z());
}

void FMyoInputDevice::onRssi(Myo* myo, uint64_t timestamp, int8_t rssi)
{
	FMyoControllerData& MyoData = MyoDataMap[myo];
	MyoData.RSSI = rssi;

	RunFunctionOnComponents([&, MyoData](UMyoControllerComponent* Component)
	{
		Component->OnRSSIChanged.Broadcast(MyoData, MyoData.RSSI);
	});
}

void FMyoInputDevice::onBatteryLevelReceived(myo::Myo* myo, uint64_t timestamp, uint8_t level)
{
	UE_LOG(MyoPluginLog, Log, TEXT("onBatteryLevelReceived: %d"), level);
	FMyoControllerData& MyoData = MyoDataMap[myo];
	MyoData.BatteryLevel = level;

	RunFunctionOnComponents([&, MyoData](UMyoControllerComponent* Component)
	{
		Component->OnBatteryLevelChanged.Broadcast(MyoData, MyoData.BatteryLevel);
	});
}

void FMyoInputDevice::onEmgData(myo::Myo* myo, uint64_t timestamp, const int8_t* emg)
{
	FMyoControllerData& MyoData = MyoDataMap[myo];
	FMyoEmgData EmgData;
	EmgData.setFromArray(emg);

	RunFunctionOnComponents([&, MyoData, EmgData](UMyoControllerComponent* Component)
	{
		Component->OnEmgData.Broadcast(MyoData, EmgData);
	});
}

void FMyoInputDevice::onWarmupCompleted(myo::Myo* myo, uint64_t timestamp, WarmupResult warmupResult)
{
	UE_LOG(MyoPluginLog, Log, TEXT("onWarmupCompleted."));

	FMyoControllerData& MyoData = MyoDataMap[myo];

	RunFunctionOnComponents([&, MyoData](UMyoControllerComponent* Component)
	{
		Component->OnWarmupCompleted.Broadcast(MyoData);
	});
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
	Myo* myo = MyoForId(MyoId);
	myo->vibrate((Myo::VibrationType)VibrationType);
}

void FMyoInputDevice::UnlockDevice(int32 MyoId, EMyoUnlockType UnlockType)
{
	Myo* myo = MyoForId(MyoId);
	myo->unlock((Myo::UnlockType)UnlockType);
}

void FMyoInputDevice::LockDevice(int32 MyoId)
{
	Myo* myo = MyoForId(MyoId);
	myo->lock();
}

void FMyoInputDevice::SetEMGStreamType(int32 MyoId, EMyoStreamEmgType StreamType)
{
	Myo* myo = MyoForId(MyoId);
	myo->setStreamEmg((Myo::StreamEmgType)StreamType);
}

bool FMyoInputDevice::IsHubEnabled()
{
	return MyoHub != nullptr;
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

void FMyoInputDevice::DataForMyo(FMyoControllerData& Data, Myo* myo)
{
	if (MyoDataMap.Contains(myo))
	{
		Data = MyoDataMap[myo];
	}
}

//Conversion
FKey FMyoInputDevice::KeyFromPose(EMyoPose Pose)
{
	switch (Pose)
	{
	case MYO_POSE_REST:
		return EKeysMyo::MyoPoseRest;
	case MYO_POSE_FIST:
		return EKeysMyo::MyoPoseFist;
	case MYO_POSE_WAVEIN:
		return EKeysMyo::MyoPoseWaveIn;
	case MYO_POSE_WAVEOUT:
		return EKeysMyo::MyoPoseWaveOut;
	case MYO_POSE_FINGERSPREAD:
		return EKeysMyo::MyoPoseFingersSpread;
	case MYO_POSE_DOUBLETAP:
		return EKeysMyo::MyoPoseDoubleTap;
	case MYO_POSE_MAX:
		return EKeysMyo::MyoPoseUnknown;
	default:
		return EKeysMyo::MyoPoseUnknown;
	}
}

EMyoPose FMyoInputDevice::ConvertedPose(myo::Pose::Type Pose)
{
	if (Pose == myo::Pose::rest)
	{
		return EMyoPose::MYO_POSE_REST;
	}
	else if (Pose == myo::Pose::fist)
	{
		return EMyoPose::MYO_POSE_FIST;
	}
	else if (Pose == myo::Pose::waveIn)
	{
		return EMyoPose::MYO_POSE_WAVEIN;
	}
	else if (Pose == myo::Pose::waveOut)
	{
		return EMyoPose::MYO_POSE_WAVEOUT;
	}
	else if (Pose == myo::Pose::fingersSpread)
	{
		return EMyoPose::MYO_POSE_FINGERSPREAD;
	}
	else if (Pose == myo::Pose::doubleTap)
	{
		return EMyoPose::MYO_POSE_DOUBLETAP;
	}
	else
	{//unknown
		return EMyoPose::MYO_POSE_MAX;
	}
}

void FMyoInputDevice::PressPose(EMyoPose pose)
{
	EmitKeyDownEventForKey(KeyFromPose(pose), 0, 0);
}

void FMyoInputDevice::ReleasePose(EMyoPose pose)
{
	EmitKeyUpEventForKey(KeyFromPose(pose), 0, 0);
}



#pragma  endregion DeviceListener

