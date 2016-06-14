#include "MyoPluginPrivatePCH.h"
#include "MyoInterface.h"
#include "MyoUtility.h"
#include "MyoDataCollector.h"

#include "IMyoPlugin.h"
#include "FMyoPlugin.h"

#define PLUGIN_VERSION "0.8.0"

//using namespace MyoUtility;

//Private API - This is where the magic happens

//Init and Runtime
void FMyoPlugin::StartupModule()
{
	UE_LOG(MyoPluginLog, Log, TEXT("Using Myo Plugin version %s"), TEXT(PLUGIN_VERSION));

	// Instantiate the PrintMyoEvents class we defined above, and attach it as a listener to our Hub.
	//collector = new MyoDataCollector;

	//Register all input mapping keys and axes
	EKeys::AddKey(FKeyDetails(EKeysMyo::MyoPoseRest, LOCTEXT("MyoPoseRest", "Myo Pose Rest"), FKeyDetails::GamepadKey));
	EKeys::AddKey(FKeyDetails(EKeysMyo::MyoPoseFist, LOCTEXT("MyoPoseFist", "Myo Pose Fist"), FKeyDetails::GamepadKey));
	EKeys::AddKey(FKeyDetails(EKeysMyo::MyoPoseWaveIn, LOCTEXT("MyoPoseWaveIn", "Myo Pose Wave In"), FKeyDetails::GamepadKey));
	EKeys::AddKey(FKeyDetails(EKeysMyo::MyoPoseWaveOut, LOCTEXT("MyoPoseWaveOut", "Myo Pose Wave Out"), FKeyDetails::GamepadKey));
	EKeys::AddKey(FKeyDetails(EKeysMyo::MyoPoseFingersSpread, LOCTEXT("MyoPoseFingersSpread", "Myo Pose FingersSpread"), FKeyDetails::GamepadKey));
	EKeys::AddKey(FKeyDetails(EKeysMyo::MyoPoseDoubleTap, LOCTEXT("MyoPoseDoubleTap", "Myo Pose Double Tap"), FKeyDetails::GamepadKey));
	EKeys::AddKey(FKeyDetails(EKeysMyo::MyoPoseUnknown, LOCTEXT("MyoPoseUnknown", "Myo Pose Unknown"), FKeyDetails::GamepadKey));

	EKeys::AddKey(FKeyDetails(EKeysMyo::MyoAccelerationX, LOCTEXT("MyoAccelerationX", "Myo Acceleration X"), FKeyDetails::FloatAxis));
	EKeys::AddKey(FKeyDetails(EKeysMyo::MyoAccelerationY, LOCTEXT("MyoAccelerationY", "Myo Acceleration Y"), FKeyDetails::FloatAxis));
	EKeys::AddKey(FKeyDetails(EKeysMyo::MyoAccelerationZ, LOCTEXT("MyoAccelerationZ", "Myo Acceleration Z"), FKeyDetails::FloatAxis));

	EKeys::AddKey(FKeyDetails(EKeysMyo::MyoOrientationPitch, LOCTEXT("MyoOrientationPitch", "Myo Orientation Pitch"), FKeyDetails::FloatAxis));
	EKeys::AddKey(FKeyDetails(EKeysMyo::MyoOrientationYaw, LOCTEXT("MyoOrientationYaw", "Myo Orientation Yaw"), FKeyDetails::FloatAxis));
	EKeys::AddKey(FKeyDetails(EKeysMyo::MyoOrientationRoll, LOCTEXT("MyoOrientationRoll", "Myo Orientation Roll"), FKeyDetails::FloatAxis));

	EKeys::AddKey(FKeyDetails(EKeysMyo::MyoGyroX, LOCTEXT("MyoGyroX", "Myo Gyro X"), FKeyDetails::FloatAxis));
	EKeys::AddKey(FKeyDetails(EKeysMyo::MyoGyroY, LOCTEXT("MyoGyroY", "Myo Gyro Y"), FKeyDetails::FloatAxis));
	EKeys::AddKey(FKeyDetails(EKeysMyo::MyoGyroZ, LOCTEXT("MyoGyroZ", "Myo Gyro Z"), FKeyDetails::FloatAxis));
}

void FMyoPlugin::ShutdownModule()
{
	//collector->myoInterface = NULL;
	
	//collector->ShutDown();
	delete collector;
}


//Public API Implementation

bool FMyoPlugin::IsValidDeviceId(int deviceId)
{
	return !(deviceId < 1 || deviceId > collector->m_data.size());
}

/** Public API **/
void FMyoPlugin::VibrateDevice(int deviceId, int vibrationType)
{
	if (!this->IsValidDeviceId(deviceId)) return;

	myo::Myo* myo = collector->knownMyos[deviceId - 1];
	myo->vibrate(static_cast<myo::Myo::VibrationType>(vibrationType));
}

void FMyoPlugin::LockMyo(int deviceId)
{
	if (!this->IsValidDeviceId(deviceId)) return;

	myo::Myo* myo = collector->knownMyos[deviceId - 1];
	myo->lock();
}

UMyoController* FMyoPlugin::ControllerForMyoId(int myoID)
{
	//collector->myo
}

void FMyoPlugin::UnlockMyo(int deviceId, EMyoUnlockType type)
{
	if (!this->IsValidDeviceId(deviceId)) return;

	myo::Myo* myo = collector->knownMyos[deviceId - 1];

	myo::Myo::UnlockType myoUnlockType;
	switch (type)
	{
	case MYO_UNLOCK_TIMED:
		myoUnlockType = myo::Myo::unlockTimed;
		break;
	case MYO_UNLOCK_HOLD:
		myoUnlockType = myo::Myo::unlockHold;
		break;
	default:
		myoUnlockType = myo::Myo::unlockTimed;
		break;
	}
	myo->unlock(myoUnlockType);
}

void FMyoPlugin::SetLockingPolicy(EMyoLockingPolicy policy)
{
	if (collector->hub == NULL){
		UE_LOG(MyoPluginLog, Log, TEXT("(FMyoPlugin)Hub hasn't been initialized, locking policy setting failed."))
		return;
	}

	//Reset our hub for this to work
	collector->ResetHub();

	//Set the locking policy
	myo::Hub::LockingPolicy hubLockingPolicy;
	switch (policy)
	{
	case MYO_LOCKING_POLICY_NONE:
		hubLockingPolicy = myo::Hub::lockingPolicyNone;
		break;
	case MYO_LOCKING_POLICY_STANDARD:
		hubLockingPolicy = myo::Hub::lockingPolicyStandard;
		break;
	default:
		hubLockingPolicy = myo::Hub::lockingPolicyNone;
		break;
	}

	UE_LOG(MyoPluginLog, Log, TEXT("Set Policy to %d"), (int)hubLockingPolicy);

	//Reflect the locking policy in our data
	collector->SetLockingPolicy(hubLockingPolicy);
}

void FMyoPlugin::SetStreamEmg(int deviceId, EMyoStreamEmgType type)
{
	if (!this->IsValidDeviceId(deviceId)) return;

	myo::Myo* myo = collector->knownMyos[deviceId - 1];

	myo::Myo::StreamEmgType myoNativeStreamType;
	switch (type)
	{
	case MYO_STREAM_EMG_DISABLED:
		myoNativeStreamType = myo::Myo::streamEmgDisabled;
		break;
	case MYO_STREAM_EMG_ENABLED:
		myoNativeStreamType = myo::Myo::streamEmgEnabled;
		break;
	default:
		myoNativeStreamType = myo::Myo::streamEmgDisabled;
		break;
	}

	myo->setStreamEmg(myoNativeStreamType);
}
 
//Freshest Data
FMyoDeviceData* FMyoPlugin::LatestData(int deviceId)
{
	if (!this->IsValidDeviceId(deviceId)) return NULL;

	return &(collector->m_data[deviceId - 1]);
}

void FMyoPlugin::WhichArm(int deviceId, int& arm)
{
	if (!this->IsValidDeviceId(deviceId)) return;

	arm = collector->m_data[deviceId - 1].arm;
}

void FMyoPlugin::LeftMyoId(bool& available, int& deviceId)
{
	if (collector->leftMyo == -1)
		available = false;
	else{
		available = true;
		deviceId = collector->leftMyo;
	}
}
void FMyoPlugin::RightMyoId(bool& available, int& deviceId)
{
	if (collector->rightMyo == -1)
		available = false;
	else{
		available = true;
		deviceId = collector->rightMyo;
	}
}

//Returns last valid myo, this is a catch-all identifier 
//so a user can just call this and get their one paired myo pointer easily
void FMyoPlugin::PrimaryMyoId(bool& available, int& deviceId)
{
	deviceId = collector->identifyMyo(collector->lastValidMyo());
	available = (deviceId != -1);
}

void FMyoPlugin::MaxMyoId(int& maxId)
{
	maxId = collector->m_data.size();
}

bool FMyoPlugin::IsHubEnabled()
{
	return collector->Enabled;
}

//sets the calibration orientation
void FMyoPlugin::CalibrateOrientation(int deviceId, FRotator direction)
{

	//special case, means calibrate all
	if (deviceId == 0)
	{
		for (size_t i = 0; i < collector->m_data.size(); ++i) 
		{
			//Grab current orientation set it to the space correction orientation
			collector->m_data[i].armSpaceCorrection = combineRotators(collector->m_data[i].orientation*-1.f, direction);
			collector->armSpaceCorrection = collector->m_data[i].armSpaceCorrection;
			collector->correctionAvailable = true;
		}
	}
	//Otherwise check device id validity and calibrate specific myo
	else
	{
		if (!this->IsValidDeviceId(deviceId)) return;

		//Grab current orientation set it to the space correction orientation
		collector->m_data[deviceId - 1].armSpaceCorrection = combineRotators(collector->m_data[deviceId - 1].orientation*-1.f, direction);
		collector->armSpaceCorrection = collector->m_data[deviceId - 1].armSpaceCorrection;
		collector->correctionAvailable = true;
	}
}

void FMyoPlugin::SetDelegate(MyoDelegate* newDelegate){ 
	collector->myoInterface = newDelegate;
	collector->Startup();

	//Emit disabled event if we didn't manage to create the hub
	if (!collector->Enabled){
		collector->myoInterface->MyoDisabled();
		UE_LOG(MyoPluginLog, Warning, TEXT("Myo is Disabled."));
	}
	else{
		//Default to None
		collector->SetLockingPolicy(myo::Hub::lockingPolicyNone);
	}

	UE_LOG(MyoPluginLog, Log, TEXT("Myo Delegate Set (should only be called once per begin play or you have duplicates)."));
}

void FMyoPlugin::RemoveDelegate()
{
	collector->myoInterface = NULL;
	collector->ShutDown();	//we only allow one delegate to be active so remove listening to stop overload of streaming
}

void FMyoPlugin::MyoTick(float DeltaTime)
{
	//Integration would go here if it was included
	//m_data[myoIndex].deltaVelocity = m_data[myoIndex].acceleration * DeltaTime;

	/**
	* We're trying to emulate a single tick;
	* So run the hub for only 1ms, this will add roughly 1-2ms to render loop time
	* While this is not ideal and a separate loop would be an improvement, 1ms is an 
	* acceptable frame lag for simplicity.
	*/
	if (collector->Listening){
		if (collector->Enabled){
			collector->hub->run(1);	//Removed when multi-threaded version is added
			//UE_LOG(MyoPluginLog, Log, TEXT("Myo Tick. %1.4f"), DeltaTime);
		}
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMyoPlugin, MyoPlugin)