#include "MyoPluginPrivatePCH.h"
#include "FMyoInputDevice.h"
#include "MyoUtility.h"
#include "MyoLambdaRunnable.h"

#define MYO_RUNTIME_MS 10	//default to 100 fps rate

using namespace MyoUtility;

#pragma region FMyoInputDevice

FMyoInputDevice::FMyoInputDevice(const TSharedRef< FGenericApplicationMessageHandler >& InMessageHandler) : MessageHandler(InMessageHandler)
{
	//Startup the background handler
	bRunning = false;
	bHubRunHasFinished = false;

	/*MyoHub = new Hub("com.epicgames.unrealengine");
	if (MyoHub->lastInitCausedError)
	{
		UE_LOG(MyoPluginLog, Log, TEXT("Hub initialization failed. Do you have Myo Connect installed and updated?"));
		MyoEnabled = false;
		MyoHub = nullptr;
		//return;
	}*/

	UE_LOG(MyoPluginLog, Log, TEXT("Myo Input device booting up."));

	//Start our background threading
	FMyoLambdaRunnable::RunLambdaOnBackGroundThread([&] 
	{
		MyoIdCounter = 0;	//reset the id counter

		/*FString BinariesRoot = FPaths::Combine(*FPaths::GameDir(), TEXT("Binaries"));
		FString DllFilepath = FPaths::ConvertRelativePathToFull(FPaths::Combine(*BinariesRoot, "Win64", "myo64.dll"));
		void* DLLHandle = FPlatformProcess::GetDllHandle(*DllFilepath);

		UE_LOG(MyoPluginLog, Log, TEXT("Handle: %d"), DLLHandle);*/

		while (!MyoHub.IsValid())
		{
			FPlatformProcess::Sleep(1.f);
			myo::Hub* RawHub = new Hub("com.epicgames.unrealengine");

			MyoHub = MakeShareable(RawHub);
			if (MyoHub.IsValid())
			{
				if (MyoHub->lastInitCausedError)
				{
					UE_LOG(MyoPluginLog, Log, TEXT("Hub initialization failed. Do you have Myo Connect installed and updated?"));
					MyoHub = nullptr;
				}
			}
			else
			{
				MyoHub = nullptr;
			}
			
		}

		UE_LOG(MyoPluginLog, Log, TEXT("MyoHub found."));
		MyoEnabled = true;

		MyoHub->addListener(this);
		MyoHub->setLockingPolicy(Hub::lockingPolicyNone);	//default to no locking policy

		UE_LOG(MyoPluginLog, Log, TEXT("Myo Initialized, thread loop started."));

		bRunning = true;
		//MyoEnabled = true;

		//MyoHub->waitForMyo()	//optimization, wait stall thread?

		//Start thread loop
		while (bRunning)
		{
			MyoHub->run(MYO_RUNTIME_MS);
		}

		UE_LOG(MyoPluginLog, Log, TEXT("Myo thread loop stopped."));

		MyoHub->removeListener(this);

		bHubRunHasFinished = true;
	});
}

FMyoInputDevice::~FMyoInputDevice()
{
	MyoEnabled = false;
	if (MyoHub.IsValid())
	{
		MyoHub->removeListener(this);
	}
	bRunning = false;

	//Wait for the hub run to complete
	while (!bHubRunHasFinished)
	{
		FPlatformProcess::Sleep(0.01f);
	}
}

void FMyoInputDevice::Tick(float DeltaTime)
{

	//SendControllerEvents();
}

void FMyoInputDevice::SendControllerEvents()
{
	//We use an external thread to forward the events, this is unused. Should we poll push latest controller data instead?

	//Do we have a valid number of myos connected?
	if (ConnectedMyos.Num() == 0 || !MyoEnabled)
	{
		return;
	}

	//For Each myo, broadcast it's movements
	/*for (auto myo : ConnectedMyos) {
		FMyoControllerData& MyoData = *MyoDataMap[myo];

		for (auto Component : ComponentDelegates)
		{
			//Call the function on the game thread
			Component->OnArmMoved.Broadcast(MyoData, MyoData.Acceleration, MyoData.Orientation, MyoData.Gyro);
		}
	}*/

	//for the primary myo also emit IM events
	Myo* PrimaryMyo = ConnectedMyos[0];
	FMyoControllerData& MyoData = *MyoDataMap[PrimaryMyo];
	
	//Orientation - Scale input mapping to -1.0-> 1.0 range
	EmitAnalogInputEventForKey(EKeysMyo::MyoOrientationPitch, MyoData.ArmOrientation.Pitch * ORIENTATION_SCALE_PITCH, 0);
	EmitAnalogInputEventForKey(EKeysMyo::MyoOrientationYaw, MyoData.ArmOrientation.Yaw * ORIENTATION_SCALE_YAWROLL, 0);
	EmitAnalogInputEventForKey(EKeysMyo::MyoOrientationRoll, MyoData.ArmOrientation.Roll * ORIENTATION_SCALE_YAWROLL, 0);

	//Acceleration - No scaling needed, 1.0 = 1g.
	EmitAnalogInputEventForKey(EKeysMyo::MyoAccelerationX, MyoData.ArmAcceleration.X, 0);
	EmitAnalogInputEventForKey(EKeysMyo::MyoAccelerationY, MyoData.ArmAcceleration.Y, 0);
	EmitAnalogInputEventForKey(EKeysMyo::MyoAccelerationZ, MyoData.ArmAcceleration.Z, 0);

	//Gyro - scaled down by 1/45. Fast flicks should be close to 1.0, slower gyro motions may need scaling up if used in input mapping
	EmitAnalogInputEventForKey(EKeysMyo::MyoGyroX, MyoData.ArmGyro.X * GYRO_SCALE, 0);
	EmitAnalogInputEventForKey(EKeysMyo::MyoGyroY, MyoData.ArmGyro.Y * GYRO_SCALE, 0);
	EmitAnalogInputEventForKey(EKeysMyo::MyoGyroZ, MyoData.ArmGyro.Z * GYRO_SCALE, 0);
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
	//Only game component should receive callbacks
	UWorld* ComponentWorld = Component->GetOwner()->GetWorld();
	if (ComponentWorld == nullptr)
	{
		return;
	}
	if (ComponentWorld->WorldType == EWorldType::Game ||
		ComponentWorld->WorldType == EWorldType::GamePreview ||
		ComponentWorld->WorldType == EWorldType::PIE)
	{
		ComponentDelegates.Add(Component);
	}
}

void FMyoInputDevice::RemoveComponentDelegate(UMyoControllerComponent* Component)
{
	ComponentDelegates.Remove(Component);
}

void FMyoInputDevice::RunFunctionOnComponents(TFunction<void(UMyoControllerComponent*)> InFunction)
{
	const TArray<UMyoControllerComponent*>& SafeComponentDelegates = ComponentDelegates;
	FMyoLambdaRunnable::RunShortLambdaOnGameThread([&, SafeComponentDelegates, InFunction]
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

	//Create the UObject wrapper (on game thread)
	//UMyoController* Controller = NewObject<UMyoController>(this);

	//Add the map data container
	FMyoControllerData DefaultData = FMyoControllerData();
	DefaultData.MyoId = MyoId;
	DefaultData.Pose = EMyoPose::MYO_POSE_UNKNOWN;

	//spawn our controller wrappers
	UMyoController* Controller = NewObject<UMyoController>();
	Controller->AddToRoot();
	Controller->MyoData = DefaultData;
	MyoDataMap.Add(myo, &Controller->MyoData);
	Controllers.Add(Controller);
	ControllerMap.Add(myo, Controller);
	
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
	FMyoControllerData MyoData = *MyoDataMap[myo];
	MyoDataMap.Remove(myo);
	IdToMyoMap.Remove(IdForMyo(myo));
	MyoToIdMap.Remove(myo);
	PairedMyos.Remove(myo);

	if (ControllerMap.Contains(myo))
	{
		UMyoController* Controller = ControllerMap[myo];
		Controller->RemoveFromRoot();
		Controllers.Remove(Controller);
		ControllerMap.Remove(myo);
	}

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

	const FMyoControllerData& MyoData = *MyoDataMap[myo];

	RunFunctionOnComponents([&, MyoData](UMyoControllerComponent* Component)
	{
		Component->OnConnect.Broadcast(MyoData);
	});
}

void FMyoInputDevice::onDisconnect(Myo* myo, uint64_t timestamp)
{
	ConnectedMyos.Remove(myo);

	UE_LOG(MyoPluginLog, Log, TEXT("onDisconnect."));

	const FMyoControllerData& MyoData = *MyoDataMap[myo];

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
	Data.bIsArmSynced = true;

	RunFunctionOnComponents([&, Data](UMyoControllerComponent* Component)
	{
		Component->OnArmSync.Broadcast(Data, Data.Arm, Data.ArmDirection);
	});
}

void FMyoInputDevice::onArmUnsync(Myo* myo, uint64_t timestamp)
{
	UE_LOG(MyoPluginLog, Log, TEXT("onArmUnsync."));

	FMyoControllerData& MyoData = *MyoDataMap[myo];
	MyoData.bIsArmSynced = false;

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

	FMyoControllerData& MyoData = *MyoDataMap[myo];
	
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
	FMyoControllerData& MyoData = *MyoDataMap[myo];
	MyoData.Orientation = convertOrientationToUE(FQuat(rotation.x(), rotation.y(), rotation.z(), rotation.w()).Rotator());
	MyoData.ArmOrientation = convertOrientationToArmSpace(MyoData.Orientation, MyoData.ArmSpaceCorrection, MyoData.ArmDirection);
}

void FMyoInputDevice::onAccelerometerData(Myo* myo, uint64_t timestamp, const Vector3<float>& accel)
{
	FMyoControllerData& MyoData = *MyoDataMap[myo];
	MyoData.Acceleration = convertVectorToUE(FVector(accel.x(), accel.y(), accel.z()));
	MyoData.ArmAcceleration = MyoData.Acceleration;
	MyoData.BodySpaceNullAcceleration = convertAccelerationToBodySpace( MyoData.ArmAcceleration,
																		MyoData.ArmOrientation, 
																		MyoData.ArmSpaceCorrection, 
																		MyoData.ArmDirection);

	RunFunctionOnComponents([&, MyoData](UMyoControllerComponent* Component)
	{
		Component->OnArmMoved.Broadcast(MyoData, MyoData.ArmAcceleration, MyoData.ArmOrientation, MyoData.ArmGyro);
	});
}

void FMyoInputDevice::onGyroscopeData(Myo* myo, uint64_t timestamp, const Vector3<float>& gyro)
{
	FMyoControllerData& MyoData = *MyoDataMap[myo];
	MyoData.Gyro = FVector(gyro.x(), gyro.y(), gyro.z());
	MyoData.ArmGyro = convertVectorToUE(MyoData.Gyro);
}

void FMyoInputDevice::onRssi(Myo* myo, uint64_t timestamp, int8_t rssi)
{
	FMyoControllerData& MyoData = *MyoDataMap[myo];
	MyoData.RSSI = rssi;

	RunFunctionOnComponents([&, MyoData](UMyoControllerComponent* Component)
	{
		Component->OnRSSIChanged.Broadcast(MyoData, MyoData.RSSI);
	});
}

void FMyoInputDevice::onBatteryLevelReceived(myo::Myo* myo, uint64_t timestamp, uint8_t level)
{
	UE_LOG(MyoPluginLog, Log, TEXT("onBatteryLevelReceived: %d"), level);
	FMyoControllerData& MyoData = *MyoDataMap[myo];
	MyoData.BatteryLevel = level;

	RunFunctionOnComponents([&, MyoData](UMyoControllerComponent* Component)
	{
		Component->OnBatteryLevelChanged.Broadcast(MyoData, MyoData.BatteryLevel);
	});
}

void FMyoInputDevice::onEmgData(myo::Myo* myo, uint64_t timestamp, const int8_t* emg)
{
	FMyoControllerData& MyoData = *MyoDataMap[myo];
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

	FMyoControllerData& MyoData = *MyoDataMap[myo];

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
	//todo: implement old style calibration function
	FMyoControllerData& MyoData = *MyoDataMap[MyoForId(MyoId)];

	MyoData.ArmSpaceCorrection = combineRotators(MyoData.Orientation*-1.f, Direction);
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
	//return true;
	return MyoHub.IsValid();
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
		Data = *MyoDataMap[myo];
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
	case MYO_POSE_UNKNOWN:
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
		return EMyoPose::MYO_POSE_UNKNOWN;
	}
}

void FMyoInputDevice::PressPose(EMyoPose pose)
{
	FMyoLambdaRunnable::RunShortLambdaOnGameThread([&, pose]
	{
		if (bRunning)
		{
			EmitKeyDownEventForKey(KeyFromPose(pose), 0, 0);
		}
	});
}

void FMyoInputDevice::ReleasePose(EMyoPose pose)
{
	FMyoLambdaRunnable::RunShortLambdaOnGameThread([&, pose]
	{
		if (bRunning)
		{
			EmitKeyUpEventForKey(KeyFromPose(pose), 0, 0);
		}
	});
}

UMyoController* FMyoInputDevice::ControllerForId(int32 MyoId)
{
	return ControllerMap[MyoForId(MyoId)];
}

#pragma  endregion DeviceListener
