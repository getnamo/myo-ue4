#include "MyoPluginPrivatePCH.h"
#include "MyoUtility.h"

using namespace MyoUtility;

MyoDataCollector::MyoDataCollector() {
	myoInterface = NULL;
	hub = NULL;
	Enabled = false;
	Listening = false;
	lastPose = myo::Pose::unknown;
}
MyoDataCollector::~MyoDataCollector() {
	ShutDown();
}

	//Input Mapping
	void MyoDataCollector::PressPose(myo::Pose pose) {

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
	void MyoDataCollector::ReleasePose(myo::Pose pose) {
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

	void MyoDataCollector::onConnect(myo::Myo *myo, uint64_t timestamp,
		myo::FirmwareVersion firmwareVersion) {
		UE_LOG(MyoPluginLog, Log, TEXT("Myo %d  has connected."), identifyMyo(myo));
		if (myoInterface)
		{
			//myoInterface->OnConnect(()
			myoInterface->MyoOnConnect(identifyMyo(myo), timestamp);
		}
	}

	void MyoDataCollector::onDisconnect(myo::Myo *myo, uint64_t timestamp) {
		UE_LOG(MyoPluginLog, Log, TEXT("Myo %d  has disconnected."), identifyMyo(myo));
		if (myoInterface)
		{
			myoInterface->MyoOnDisconnect(identifyMyo(myo), timestamp);
		}

		//Ensure we have a valid input mapping pointer
		if (myo == lastPairedMyo)
		{
			lastPairedMyo = lastValidMyo();
		}
	}

	void MyoDataCollector::onArmSync(myo::Myo* myo, uint64_t timestamp, myo::Arm arm, myo::XDirection xDirection, float rotation,
		myo::WarmupState warmupState) {
		int myoIndex = myoIndexForMyo(myo);
		m_data[myoIndex].arm = arm;
		m_data[myoIndex].xDirection = xDirection;

		//internal tracking
		if (arm == myo::armLeft)
			leftMyo = myoIndex + 1;
		if (arm == myo::armRight)
			rightMyo = myoIndex + 1;

		if (myoInterface)
		{
			myoInterface->MyoOnArmSync(identifyMyo(myo), timestamp, arm, xDirection);
		}
	}
	void MyoDataCollector::onArmUnsync(myo::Myo *myo, uint64_t timestamp) {
		int myoIndex = myoIndexForMyo(myo);
		m_data[myoIndex].arm = myo::armUnknown;
		m_data[myoIndex].xDirection = myo::xDirectionUnknown;


		//internal tracking, -1 means it's invalid
		if (leftMyo == myoIndex + 1)
			leftMyo = -1;
		if (rightMyo == myoIndex + 1)
			rightMyo = -1;

		if (myoInterface)
		{
			myoInterface->MyoOnArmUnsync(identifyMyo(myo), timestamp);
		}
	}

	void MyoDataCollector::onPair(myo::Myo* myo, uint64_t timestamp,
		myo::FirmwareVersion firmwareVersion) {
		// The pointer address we get for a Myo is unique - in other words, it's safe to compare two Myo pointers to
		// see if they're referring to the same Myo.

		// Add the Myo pointer to our list of known Myo devices. This list is used to implement identifyMyo() below so
		// that we can give each Myo a nice short identifier.
		int myoIndex = myoIndexForMyo(myo);
		if (myoIndex == -1) {
			knownMyos.push_back(myo);

			FMyoDeviceData data;
			//ensure we initialize values to proper start points
			data.xDirection = myo::xDirectionUnknown;
			data.arm = myo::armUnknown;

			if (correctionAvailable)
				data.armSpaceCorrection = armSpaceCorrection;

			m_data.push_back(data);
		}

		// Now that we've added it to our list, get our short ID for it and forward this to the delegate
		if (myoInterface != NULL)
		{
			myoInterface->MyoOnPair(identifyMyo(myo), timestamp);
		}

		lastPairedMyo = myo;
	}

	void MyoDataCollector::onUnpair(myo::Myo *myo, uint64_t timestamp) {
		int myoIndex = myoIndexForMyo(myo);

		if (myoInterface != NULL)
		{
			myoInterface->MyoOnUnpair(identifyMyo(myo), timestamp);
		}
	}

	// Called when a paired Myo has provided new orientation data, which is represented
	// as a unit quaternion.
	void MyoDataCollector::onOrientationData(myo::Myo* myo, uint64_t timestamp, const myo::Quaternion<float>& quat) {
		int myoIndex = myoIndexForMyo(myo);

		m_data[myoIndex].quaternion.X = quat.x();
		m_data[myoIndex].quaternion.Y = quat.y();
		m_data[myoIndex].quaternion.Z = quat.z();
		m_data[myoIndex].quaternion.W = quat.w();
		m_data[myoIndex].orientation = convertOrientationToUE(FRotator(m_data[myoIndex].quaternion));

		m_data[myoIndex].armOrientation = convertOrientationToArmSpace(m_data[myoIndex].orientation, m_data[myoIndex].armSpaceCorrection, (myo::XDirection)m_data[myoIndex].xDirection);

		if (myoInterface)
		{
			myoInterface->MyoOnOrientationData(myoIndex + 1, timestamp, m_data[myoIndex].quaternion);	//quaternion - raw
			myoInterface->MyoOnOrientationData(myoIndex + 1, timestamp, m_data[myoIndex].armOrientation);	//overloaded rotator - in UE format, arm converted

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
	void MyoDataCollector::onAccelerometerData(myo::Myo *myo, uint64_t timestamp, const myo::Vector3< float > &accel) {
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

		if (myoInterface)
		{
			myoInterface->MyoOnAccelerometerData(myoIndex + 1, timestamp, m_data[myoIndex].acceleration);
			myoInterface->MyoOnArmMoved(myoIndex + 1, m_data[myoIndex].armAcceleration, m_data[myoIndex].armOrientation, m_data[myoIndex].armGyro, (EMyoPose)m_data[myoIndex].pose);	//non-thalmic api

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
	void MyoDataCollector::onGyroscopeData(myo::Myo *myo, uint64_t timestamp, const myo::Vector3< float > &gyro) {
		int myoIndex = myoIndexForMyo(myo);
		m_data[myoIndex].gyro.X = gyro.x();
		m_data[myoIndex].gyro.Y = gyro.y();
		m_data[myoIndex].gyro.Z = gyro.z();

		m_data[myoIndex].armGyro = convertVectorToUE(m_data[myoIndex].gyro);

		if (myoInterface)
		{
			myoInterface->MyoOnGyroscopeData(myoIndex + 1, timestamp, m_data[myoIndex].gyro);

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
	void MyoDataCollector::onUnlock(myo::Myo* myo, uint64_t timestamp)
	{
		int myoIndex = myoIndexForMyo(myo);
		m_data[myoIndex].isLocked = false;
	}
	// onLock() is called whenever Myo has become locked. No pose events will be sent until the Myo is unlocked again.
	void MyoDataCollector::onLock(myo::Myo* myo, uint64_t timestamp)
	{
		int myoIndex = myoIndexForMyo(myo);
		m_data[myoIndex].isLocked = true;
	}

	// Called whenever the Myo detects that the person wearing it has changed their pose, for example,
	// making a fist, or not making a fist anymore.
	void MyoDataCollector::onPose(myo::Myo* myo, uint64_t timestamp, myo::Pose pose) {
		int myoIndex = myoIndexForMyo(myo);
		m_data[myoIndex].pose = (int)pose.type();


		//debug
		UE_LOG(MyoPluginLog, Log, TEXT("Myo %d switched to pose %s."), identifyMyo(myo), tcharFromStdString(pose.toString()));
		//FString debugmsg = FString::Printf(TEXT("Myo %d switched to pose %s."), identifyMyo(myo), tcharFromStdString(pose.toString()));
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, debugmsg);

		if (myoInterface)
		{
			myoInterface->MyoOnPose(myoIndex + 1, timestamp, (int32)pose.type());

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

	void MyoDataCollector::onEmgData(myo::Myo* myo, uint64_t timestamp, const int8_t* emg)
	{
		int myoIndex = myoIndexForMyo(myo);

		//Gather and organize
		//There are 8 streams one for each plate
		FMyoEmgData data;
		for (int i = 0; i < 8; i++) {
			data.streams.Add(emg[i]);
		}

		//TArray<int32> data;
		//for (int i = 0; i < 8; i++) {
		//	data.Add(emg[i]);
		//}

		//Emit
		myoInterface->MyoOnEmgData(myoIndex + 1,
			data);
	}

	// This is a utility function implemented for this sample that maps a myo::Myo* to a unique ID starting at 1.
	// It does so by looking for the Myo pointer in knownMyos, which onPair() adds each Myo into as it is paired.
	size_t MyoDataCollector::identifyMyo(myo::Myo* myo) {
		// Walk through the list of Myo devices that we've seen pairing events for.
		for (size_t i = 0; i < knownMyos.size(); ++i) {
			// If two Myo pointers compare equal, they refer to the same Myo device.
			if (knownMyos[i] == myo) {
				return i + 1;
			}
		}

		return 0;
	}

	myo::Myo* MyoDataCollector::lastValidMyo() {
		for (size_t i = 0; i < knownMyos.size(); ++i) {
			// If two Myo pointers compare equal, they refer to the same Myo device.
			myo::Myo* myo = knownMyos[i];
			if (myo) {
				return knownMyos[i];
			}
		}
		return NULL;
	}

	bool MyoDataCollector::myoIsValidForInputMapping(myo::Myo* myo) {
		return (myo == lastPairedMyo);
	}

	size_t MyoDataCollector::myoIndexForMyo(myo::Myo* myo) {
		return identifyMyo(myo) - 1;
	}

	void MyoDataCollector::StartListening() {
		if (!Listening) {
			hub->addListener(this);	//for single threaded listener needs to be added locally
			Listening = true;
		}
	}
	void MyoDataCollector::StopListening() {
		if (Listening) {
			hub->removeListener(this);	//for single threaded listener needs to be removed locally
			Listening = false;
		}
	}

	void MyoDataCollector::UnlockHoldEachMyo()
	{
		for (size_t i = 0; i < knownMyos.size(); ++i) {
			// If two Myo pointers compare equal, they refer to the same Myo device.
			myo::Myo* myo = knownMyos[i];
			if (myo) {
				myo->unlock(myo::Myo::unlockHold);
			}
		}
	}
	void MyoDataCollector::LockEachMyo()
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
	bool MyoDataCollector::Startup()
	{
		if (hub == NULL)
		{
			hub = new myo::Hub("com.plugin.unrealengine4");

			if (hub->lastInitCausedError) {
				delete hub;
				hub = NULL;
				UE_LOG(MyoPluginLog, Log, TEXT("Myo Hub failed to initialize, did you install and run MyoConnect?"));
			}
			else if (hub != NULL) {
				UE_LOG(MyoPluginLog, Log, TEXT("Myo Hub Initialized."));
			}

			//if (myo::ThrowOnError::lastCallWasError())
			//	hu
			//	;
		}
		if (hub) {
			//Start a hub thread
			//FMyoHubWorker::HubInit(hub);	//disabled until easy way to callOnGameThread is added for every event

			StartListening();
			Enabled = true;
			return true;
		}
		else {
			return false;
		}
	}

	void MyoDataCollector::ShutDown()
	{
		StopListening();

		//Stop our hub thread
		//FMyoHubWorker::Shutdown();	//disabled until easy way to callOnGameThread is added for every event

		if (hub != NULL) {
			delete hub;
			hub = NULL;

			//clear all lists
			knownMyos.clear();
			m_data.clear();
			//controllers

			//Shutdown message
			UE_LOG(MyoPluginLog, Log, TEXT("Myo Hub Shutdown."));
		}
	}

	void MyoDataCollector::ResetHub()
	{
		ShutDown();
		Startup();
	}

	void MyoDataCollector::SetLockingPolicy(myo::Hub::LockingPolicy policy)
	{
		lockingPolicy = policy;
		hub->setLockingPolicy(policy);
	}
};