#include "MyoPluginPrivatePCH.h"

#include "IMyoPlugin.h"
#include "FMyoPlugin.h"
#include "MyoDelegate.h"
#include "MyoDelegateBlueprint.h"
#include "MyoHubWorker.h"
#include "SlateBasics.h"

#include <iostream>
#include <stdexcept>
#include <vector>
#include <myo/myo.hpp>

IMPLEMENT_MODULE(FMyoPlugin, MyoPlugin)

#define LOCTEXT_NAMESPACE "MyoPlugin"
#define ORIENTATION_SCALE_PITCH 0.01111111111	//1/90
#define ORIENTATION_SCALE_YAWROLL 0.00555555555 //1/180
#define GYRO_SCALE 0.02222222222				//1/45

#define PLUGIN_VERSION "0.7.13"

//Private API - This is where the magic happens

//UE v4.6 IM event wrappers
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

bool EmitAnalogInputEventForKey(FKey key, float value, int32 user)
{
	FAnalogInputEvent AnalogInputEvent(key, FSlateApplication::Get().GetModifierKeys(), user, false, 0, 0, value);
	return FSlateApplication::Get().ProcessAnalogInputEvent(AnalogInputEvent);
}

//String Conversion to UE4
TCHAR* tcharFromStdString(std::string str){
	TCHAR *param = new TCHAR[str.size() + 1];
	param[str.size()] = 0;
	std::copy(str.begin(), str.end(), param);
	return param;
}
FRotator convertOrientationToUE(FRotator rawOrientation)
{
	FRotator convertedOrientation;
	convertedOrientation.Pitch = rawOrientation.Pitch*-1.f;
	convertedOrientation.Yaw = rawOrientation.Yaw*-1.f;
	convertedOrientation.Roll = rawOrientation.Roll;
	return convertedOrientation;

	//debug return raw orientation
	//return rawOrientation;
}

//Same as blueprint function, internal copy
FRotator combineRotators(FRotator A, FRotator B)
{
	FQuat AQuat = FQuat(A);
	FQuat BQuat = FQuat(B);

	return FRotator(BQuat*AQuat);
}

FVector convertVectorToUE(FVector rawAcceleration)
{
	return FVector(rawAcceleration.X, -rawAcceleration.Y, -rawAcceleration.Z);
}

FVector convertAccelerationToBodySpace(FVector armAcceleration, FRotator orientation, FRotator armCorrection, myo::XDirection direction)
{
	float directionModifier = 1.f;
	//something wrong here, also make sure this is applied to the arm space correction as well
	if (direction == myo::xDirectionTowardElbow){
		directionModifier = -1.f;
	}

	//Compensate for arm roll
	FRotator armYawCorrection = FRotator(0, armCorrection.Yaw, 0);

	FRotator fullCompensationOrientation = combineRotators(orientation, armYawCorrection);

	FVector reactionAcceleration = fullCompensationOrientation.RotateVector(armAcceleration);

	//Subtract gravity
	return ((reactionAcceleration * FVector(directionModifier, directionModifier, 1.f)) + FVector(0, 0, 1)) * -1.f;
}

FRotator convertOrientationToArmSpace(FRotator convertedOrientation, FRotator armCorrection, myo::XDirection direction)
{
	float directionModifier = 1.f;
	//Check for arm direction, compensate if needed by reversing pitch and roll
	if (direction == myo::xDirectionTowardElbow){
		directionModifier = -1.f;
		convertedOrientation = FRotator(convertedOrientation.Pitch*directionModifier, convertedOrientation.Yaw, convertedOrientation.Roll*directionModifier);
	}

	//Compensate Roll (pre)
	FRotator tempRot = combineRotators(FRotator(0, 0, armCorrection.Roll*directionModifier), convertedOrientation);

	//Compensate for Yaw (post) and return the result
	return combineRotators(tempRot, FRotator(0, armCorrection.Yaw, 0));
}

class DataCollector : public myo::DeviceListener {
public:
	DataCollector(){
		myoDelegate = NULL;
		hub = NULL;
		Enabled = false;
		Listening = false;
		lastPose = myo::Pose::unknown;
	}
	~DataCollector(){
		ShutDown();
	}

	//Input Mapping
	void PressPose(myo::Pose pose){

		if (pose == myo::Pose::rest)
		{
			EmitKeyDownEventForKey(EKeysMyo::MyoPoseRest, 0, 0);
		}
		else if (pose == myo::Pose::fist)
		{
			EmitKeyDownEventForKey(EKeysMyo::MyoPoseFist, 0, 0);
		}
		else if (pose == myo::Pose::waveIn)
		{
			EmitKeyDownEventForKey(EKeysMyo::MyoPoseWaveIn, 0, 0);
		}
		else if (pose == myo::Pose::waveOut)
		{
			EmitKeyDownEventForKey(EKeysMyo::MyoPoseWaveOut, 0, 0);
		}
		else if (pose == myo::Pose::fingersSpread)
		{
			EmitKeyDownEventForKey(EKeysMyo::MyoPoseFingersSpread, 0, 0);
		}
		else if (pose == myo::Pose::doubleTap)
		{
			EmitKeyDownEventForKey(EKeysMyo::MyoPoseDoubleTap, 0, 0);
		}
		else
		{//unknown
			EmitKeyDownEventForKey(EKeysMyo::MyoPoseUnknown, 0, 0);
		}
	}
	void ReleasePose(myo::Pose pose){
		if (pose == myo::Pose::rest)
		{
			EmitKeyUpEventForKey(EKeysMyo::MyoPoseRest, 0, 0);
		}
		else if (pose == myo::Pose::fist)
		{
			EmitKeyUpEventForKey(EKeysMyo::MyoPoseFist, 0, 0);
		}
		else if (pose == myo::Pose::waveIn)
		{
			EmitKeyUpEventForKey(EKeysMyo::MyoPoseWaveIn, 0, 0);
		}
		else if (pose == myo::Pose::waveOut)
		{
			EmitKeyUpEventForKey(EKeysMyo::MyoPoseWaveOut, 0, 0);
		}
		else if (pose == myo::Pose::fingersSpread)
		{
			EmitKeyUpEventForKey(EKeysMyo::MyoPoseFingersSpread, 0, 0);
		}
		else if (pose == myo::Pose::doubleTap)
		{
			EmitKeyUpEventForKey(EKeysMyo::MyoPoseDoubleTap, 0, 0);
		}
		else
		{//unknown
			EmitKeyUpEventForKey(EKeysMyo::MyoPoseUnknown, 0, 0);
		}
	}

	void onConnect(myo::Myo *myo, uint64_t timestamp,
		myo::FirmwareVersion firmwareVersion){
		UE_LOG(MyoPluginLog, Log, TEXT("Myo %d  has connected."), identifyMyo(myo));
		if (myoDelegate)
		{
			myoDelegate->MyoOnConnect(identifyMyo(myo), timestamp);
		}
	}

	void onDisconnect(myo::Myo *myo, uint64_t timestamp){
		UE_LOG(MyoPluginLog, Log, TEXT("Myo %d  has disconnected."), identifyMyo(myo));
		if (myoDelegate)
		{
			myoDelegate->MyoOnDisconnect(identifyMyo(myo), timestamp);
		}

		//Ensure we have a valid input mapping pointer
		if (myo == lastPairedMyo)
		{
			lastPairedMyo = lastValidMyo();
		}
	}

	void onArmSync(myo::Myo* myo, uint64_t timestamp, myo::Arm arm, myo::XDirection xDirection, float rotation,
		myo::WarmupState warmupState){
		int myoIndex = myoIndexForMyo(myo);
		m_data[myoIndex].arm = arm;
		m_data[myoIndex].xDirection = xDirection;

		//internal tracking
		if (arm == myo::armLeft)
			leftMyo = myoIndex + 1;
		if (arm == myo::armRight)
			rightMyo = myoIndex + 1;

		if (myoDelegate)
		{
			myoDelegate->MyoOnArmSync(identifyMyo(myo), timestamp, arm, xDirection);
		}
	}
	void onArmUnsync(myo::Myo *myo, uint64_t timestamp){
		int myoIndex = myoIndexForMyo(myo);
		//bool armJustLost = false;
		//int old = m_data[myoIndex].arm;
		m_data[myoIndex].arm = myo::armUnknown;
		m_data[myoIndex].xDirection = myo::xDirectionUnknown;

		//beta 3 fix - downgrade to beta 2 for now 
		//if (old != m_data[myoIndex].arm)
		//	armJustLost = true;

		//internal tracking, -1 means it's invalid
		if (leftMyo == myoIndex + 1)
			leftMyo = -1;
		if (rightMyo == myoIndex + 1)
			rightMyo = -1;

		if (myoDelegate)// && armJustLost)
		{
			myoDelegate->MyoOnArmUnsync(identifyMyo(myo), timestamp);
		}
	}

	void onPair(myo::Myo* myo, uint64_t timestamp,
		myo::FirmwareVersion firmwareVersion){
		// The pointer address we get for a Myo is unique - in other words, it's safe to compare two Myo pointers to
		// see if they're referring to the same Myo.

		// Add the Myo pointer to our list of known Myo devices. This list is used to implement identifyMyo() below so
		// that we can give each Myo a nice short identifier.
		int myoIndex = myoIndexForMyo(myo);
		if (myoIndex == -1){
			knownMyos.push_back(myo);

			MyoDeviceData data;
			//ensure we initialize values to proper start points
			data.xDirection = myo::xDirectionUnknown;
			data.arm = myo::armUnknown;

			if (correctionAvailable)
				data.armSpaceCorrection = armSpaceCorrection;

			m_data.push_back(data);
		}

		// Now that we've added it to our list, get our short ID for it and forward this to the delegate
		if (myoDelegate!=NULL)
		{
			myoDelegate->MyoOnPair(identifyMyo(myo), timestamp);
		}

		lastPairedMyo = myo;
	}

	void onUnpair(myo::Myo *myo, uint64_t timestamp){
		int myoIndex = myoIndexForMyo(myo);

		if (myoDelegate != NULL)
		{
			myoDelegate->MyoOnUnpair(identifyMyo(myo), timestamp);
		}
	}

	// Called when a paired Myo has provided new orientation data, which is represented
	// as a unit quaternion.
	void onOrientationData(myo::Myo* myo, uint64_t timestamp, const myo::Quaternion<float>& quat){
		int myoIndex = myoIndexForMyo(myo);

		m_data[myoIndex].quaternion.X = quat.x();
		m_data[myoIndex].quaternion.Y = quat.y();
		m_data[myoIndex].quaternion.Z = quat.z();
		m_data[myoIndex].quaternion.W = quat.w();
		m_data[myoIndex].orientation = convertOrientationToUE(FRotator(m_data[myoIndex].quaternion));
		
		m_data[myoIndex].armOrientation = convertOrientationToArmSpace(m_data[myoIndex].orientation, m_data[myoIndex].armSpaceCorrection, (myo::XDirection)m_data[myoIndex].xDirection);

		if (myoDelegate)
		{
			myoDelegate->MyoOnOrientationData(myoIndex + 1, timestamp, m_data[myoIndex].quaternion);	//quaternion - raw
			myoDelegate->MyoOnOrientationData(myoIndex + 1, timestamp, m_data[myoIndex].armOrientation);	//overloaded rotator - in UE format, arm converted

			//InputMapping - only supports controller 1 for now
			if (myoIsValidForInputMapping(myo))
			{
				//Scale input mapping to -1.0-> 1.0 range
				EmitAnalogInputEventForKey(EKeysMyo::MyoOrientationPitch, m_data[myoIndex].armOrientation.Pitch * ORIENTATION_SCALE_PITCH, 0);
				EmitAnalogInputEventForKey(EKeysMyo::MyoOrientationYaw, m_data[myoIndex].armOrientation.Yaw * ORIENTATION_SCALE_YAWROLL, 0);
				EmitAnalogInputEventForKey(EKeysMyo::MyoOrientationRoll, m_data[myoIndex].armOrientation.Roll * ORIENTATION_SCALE_YAWROLL, 0);
			}
		}
	}

	// Called when a paired Myo has provided new accelerometer data in units of g. 
	void onAccelerometerData(myo::Myo *myo, uint64_t timestamp, const myo::Vector3< float > &accel){
		int myoIndex = myoIndexForMyo(myo);
		m_data[myoIndex].acceleration.X = accel.x();
		m_data[myoIndex].acceleration.Y = accel.y();
		m_data[myoIndex].acceleration.Z = accel.z();

		//convert to UE space
		m_data[myoIndex].acceleration = convertVectorToUE(m_data[myoIndex].acceleration);

		//add compensation for arm direction
		m_data[myoIndex].armAcceleration = m_data[myoIndex].acceleration;

		//convert to body space with nullified acceleration
		m_data[myoIndex].bodySpaceNullAcceleration = convertAccelerationToBodySpace(m_data[myoIndex].armAcceleration, m_data[myoIndex].orientation, m_data[myoIndex].armSpaceCorrection, (myo::XDirection)m_data[myoIndex].xDirection);

		if (myoDelegate)
		{
			myoDelegate->MyoOnAccelerometerData(myoIndex + 1, timestamp, m_data[myoIndex].acceleration);
			myoDelegate->MyoOnArmMoved(myoIndex + 1, m_data[myoIndex].armAcceleration, m_data[myoIndex].armOrientation, m_data[myoIndex].armGyro, (MyoPose)m_data[myoIndex].pose);	//non-thalmic api

			//InputMapping - only supports controller 1 for now
			if (myoIsValidForInputMapping(myo))
			{
				//No scaling needed, 1.0 = 1g.
				EmitAnalogInputEventForKey(EKeysMyo::MyoAccelerationX, m_data[myoIndex].armAcceleration.X, 0);
				EmitAnalogInputEventForKey(EKeysMyo::MyoAccelerationY, m_data[myoIndex].armAcceleration.Y, 0);
				EmitAnalogInputEventForKey(EKeysMyo::MyoAccelerationZ, m_data[myoIndex].armAcceleration.Z, 0);
			}
		}
	}

	//ang/s
	void onGyroscopeData(myo::Myo *myo, uint64_t timestamp, const myo::Vector3< float > &gyro){
		int myoIndex = myoIndexForMyo(myo);
		m_data[myoIndex].gyro.X = gyro.x();
		m_data[myoIndex].gyro.Y = gyro.y();
		m_data[myoIndex].gyro.Z = gyro.z();

		m_data[myoIndex].armGyro = convertVectorToUE(m_data[myoIndex].gyro);

		if (myoDelegate)
		{
			myoDelegate->MyoOnGyroscopeData(myoIndex + 1, timestamp, m_data[myoIndex].gyro);

			//InputMapping - only supports controller 1 for now
			if (myoIsValidForInputMapping(myo))
			{
				//scaled down by 1/45. Fast flicks should be close to 1.0, slower gyro motions may need scaling up if used in input mapping
				EmitAnalogInputEventForKey(EKeysMyo::MyoGyroX, m_data[myoIndex].armGyro.X * GYRO_SCALE, 0);
				EmitAnalogInputEventForKey(EKeysMyo::MyoGyroY, m_data[myoIndex].armGyro.Y * GYRO_SCALE, 0);
				EmitAnalogInputEventForKey(EKeysMyo::MyoGyroZ, m_data[myoIndex].armGyro.Z * GYRO_SCALE, 0);
			}
		}
	}

	// onUnlock() is called whenever Myo has become unlocked, and will start delivering pose events.
	void onUnlock(myo::Myo* myo, uint64_t timestamp)
	{
		int myoIndex = myoIndexForMyo(myo);
		m_data[myoIndex].isLocked = false;
	}
	// onLock() is called whenever Myo has become locked. No pose events will be sent until the Myo is unlocked again.
	void onLock(myo::Myo* myo, uint64_t timestamp)
	{
		int myoIndex = myoIndexForMyo(myo);
		m_data[myoIndex].isLocked = true;
	}

	// Called whenever the Myo detects that the person wearing it has changed their pose, for example,
	// making a fist, or not making a fist anymore.
	void onPose(myo::Myo* myo, uint64_t timestamp, myo::Pose pose){
		int myoIndex = myoIndexForMyo(myo);
		m_data[myoIndex].pose = (int)pose.type();
		

		//debug
		UE_LOG(MyoPluginLog, Log, TEXT("Myo %d switched to pose %s."), identifyMyo(myo), tcharFromStdString(pose.toString()));
		//FString debugmsg = FString::Printf(TEXT("Myo %d switched to pose %s."), identifyMyo(myo), tcharFromStdString(pose.toString()));
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, debugmsg);

		if (myoDelegate)
		{
			myoDelegate->MyoOnPose(myoIndex + 1, timestamp, (int32)pose.type());

			//InputMapping - only supports controller 1 for now, last bound myo = input mapper
			if (myoIsValidForInputMapping(myo))
			{
				//release the old pose, press new pose
				ReleasePose(lastPose);
				PressPose(pose);

				//update lastPose
				lastPose = pose;
			}

		}
	}

	void onEmgData(myo::Myo* myo, uint64_t timestamp, const int8_t* emg)
	{
		int myoIndex = myoIndexForMyo(myo);

		//Gather and organize
		//There are 8 streams one for each plate
		FMyoEmgData data;
		for (int i = 0; i < 8; i++) {
			data.streams.Add(emg[i]);
		}

		/*TArray<int32> data;
		for (int i = 0; i < 8; i++) {
			data.Add(emg[i]);
		}*/
		
		//Emit
		myoDelegate->MyoOnEmgData(	myoIndex + 1,
									data);
	}

	// This is a utility function implemented for this sample that maps a myo::Myo* to a unique ID starting at 1.
	// It does so by looking for the Myo pointer in knownMyos, which onPair() adds each Myo into as it is paired.
	size_t identifyMyo(myo::Myo* myo) {
		// Walk through the list of Myo devices that we've seen pairing events for.
		for (size_t i = 0; i < knownMyos.size(); ++i) {
			// If two Myo pointers compare equal, they refer to the same Myo device.
			if (knownMyos[i] == myo) {
				return i + 1;
			}
		}

		return 0;
	}

	myo::Myo* lastValidMyo(){
		for (size_t i = 0; i < knownMyos.size(); ++i) {
			// If two Myo pointers compare equal, they refer to the same Myo device.
			myo::Myo* myo = knownMyos[i];
			if (myo) {
				return knownMyos[i];
			}
		}
		return NULL;
	}
	
	bool myoIsValidForInputMapping(myo::Myo* myo){
		return (myo == lastPairedMyo);
	}

	size_t myoIndexForMyo(myo::Myo* myo){
		return identifyMyo(myo) - 1;
	}

	void StartListening(){
		if (!Listening){
			hub->addListener(this);	//for single threaded listener needs to be added locally
			Listening = true;
		}
	}
	void StopListening(){
		if (Listening){
			hub->removeListener(this);	//for single threaded listener needs to be removed locally
			Listening = false;
		}
	}

	void UnlockHoldEachMyo()
	{
		for (size_t i = 0; i < knownMyos.size(); ++i) {
			// If two Myo pointers compare equal, they refer to the same Myo device.
			myo::Myo* myo = knownMyos[i];
			if (myo) {
				myo->unlock(myo::Myo::unlockHold);
			}
		}
	}
	void LockEachMyo()
	{
		for (size_t i = 0; i < knownMyos.size(); ++i) {
			// If two Myo pointers compare equal, they refer to the same Myo device.
			myo::Myo* myo = knownMyos[i];
			if (myo) {
				myo->lock();
			}
		}
	}
	//The plugin may live longer than the playing session, run() causes a backlog of events to stream, to fix this we will close it in between sessions
	bool Startup()
	{
		if (hub == NULL)
		{
			hub = new myo::Hub("com.plugin.unrealengine4");

			if (hub->lastInitCausedError){
				delete hub;
				hub = NULL;
				UE_LOG(MyoPluginLog, Log, TEXT("Myo Hub failed to initialize, did you install and run MyoConnect?"));
			}
			else if(hub != NULL){
				UE_LOG(MyoPluginLog, Log, TEXT("Myo Hub Initialized."));
			}
			
			/*if (myo::ThrowOnError::lastCallWasError())
				hu
				;*/
		}
		if (hub){
			//Start a hub thread
			//FMyoHubWorker::HubInit(hub);	//disabled until easy way to callOnGameThread is added for every event

			StartListening();
			Enabled = true;
			return true;
		}
		else{
			return false;
		}
	}

	void ShutDown()
	{
		StopListening();

		//Stop our hub thread
		//FMyoHubWorker::Shutdown();	//disabled until easy way to callOnGameThread is added for every event

		if (hub != NULL){
			delete hub;
			hub = NULL;

			//clear all lists
			knownMyos.clear();
			m_data.clear();

			//Shutdown message
			UE_LOG(MyoPluginLog, Log, TEXT("Myo Hub Shutdown."));
		}
	}

	void ResetHub()
	{
		ShutDown();
		Startup();
	}

	void SetLockingPolicy(myo::Hub::LockingPolicy policy)
	{
		lockingPolicy = policy;
		hub->setLockingPolicy(policy);
	}

	// We store each Myo pointer that we pair with in this list, so that we can keep track of the order we've seen
	// each Myo and give it a unique short identifier (see onPair() and identifyMyo() above).
	std::vector<myo::Myo*> knownMyos;
	int leftMyo;
	int rightMyo;
	myo::Myo* lastPairedMyo;
	std::vector<MyoDeviceData>  m_data;	//up to n devices supported
	MyoDelegate* myoDelegate;
	bool Enabled;
	bool Listening;
	myo::Hub::LockingPolicy lockingPolicy;
	myo::Pose lastPose;
	FRotator armSpaceCorrection;	//used to hold on to the correction throughout hub destroying
	bool correctionAvailable = false;

	myo::Hub *hub;
};

//Init and Runtime
void FMyoPlugin::StartupModule()
{
	UE_LOG(MyoPluginLog, Log, TEXT("Using Myo Plugin version %s"), TEXT(PLUGIN_VERSION));

	// Instantiate the PrintMyoEvents class we defined above, and attach it as a listener to our Hub.
	collector = new DataCollector;

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
	collector->myoDelegate = NULL;
	
	collector->ShutDown();
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
void FMyoPlugin::UnlockMyo(int deviceId, MyoUnlockType type)
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

void FMyoPlugin::SetLockingPolicy(MyoLockingPolicy policy)
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

void FMyoPlugin::SetStreamEmg(int deviceId, MyoStreamEmgType type)
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
MyoDeviceData* FMyoPlugin::LatestData(int deviceId)
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
	collector->myoDelegate = newDelegate;
	collector->Startup();

	//Emit disabled event if we didn't manage to create the hub
	if (!collector->Enabled){
		collector->myoDelegate->MyoDisabled();
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
	collector->myoDelegate = NULL;
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