#pragma once
/** Inherit override and set delegate to subscribe to callbacks*/

//Latest Data structure
struct MyoDeviceData{
	int pose;				//0 = rest, 1 = fist, 2 = waveIn, 3 = waveOut, 4 = fingersSpread, 5 = reserved1, 6 = thumbToPinky, 7 = unknown (as of beta1)
	FVector acceleration;	//units of g
	FQuat quaternion;		//orientation in quaternion format
	FRotator orientation;	//orientation
	FVector gyro;			//angular speed in deg/s
	int arm;				//which arm it is bound to, 0=right, 1=left, 2=unknown
};

//Input Mapping Key Structure
struct EKeysMyo
{
	//Poses
	static const FKey MyoPoseRest;
	static const FKey MyoPoseFist;
	static const FKey MyoPoseWaveIn;
	static const FKey MyoPoseWaveOut;
	static const FKey MyoPoseFingersSpread;
	static const FKey MyoPoseThumbToPinky;
	static const FKey MyoPoseUnknown;

	//Axis
	static const FKey MyoAccelerationX;
	static const FKey MyoAccelerationY;
	static const FKey MyoAccelerationZ;

	static const FKey MyoOrientationPitch;
	static const FKey MyoOrientationYaw;
	static const FKey MyoOrientationRoll;

	static const FKey MyoGyroX;
	static const FKey MyoGyroY;
	static const FKey MyoGyroZ;
};

class MyoDelegate
{
public:
	/** Events */
	virtual void MyoOnConnect(int32 myoId, uint64 timestamp);
	virtual void MyoOnDisconnect(int32 myoId, uint64 timestamp);
	virtual void MyoOnPair(int32 myoId, uint64 timestamp);
	virtual void MyoOnOrientationData(int32 myoId, uint64 timestamp, FQuat quat);
	virtual void MyoOnOrientationData(int32 myoId, uint64 timestamp, FRotator rot);	//forward rotator version for blueprint events
	virtual void MyoOnAccelerometerData(int32 myoId, uint64 timestamp, FVector accel);
	virtual void MyoOnGyroscopeData(int32 myoId, uint64 timestamp, FVector gyro);
	virtual void MyoOnPose(int32 myoId, uint64 timestamp, int32 pose);
	virtual void MyoOnArmRecognized(int32 myoId, uint64 timestamp, int32 arm, int32 direction);
	virtual void MyoOnArmLost(int32 myoId, uint64 timestamp);

	/*Myo being disabled can occur if we do not have our bluetooth device in before launching app, 
	 in that case MyoDisabled() will emit.*/
	virtual void MyoDisabled();	

	/** Callable Functions */
	virtual void MyoVibrateDevice(int32 myoId, int32 type);
	virtual bool MyoIsHubEnabled();
	virtual void MyoLatestData(int32 myoId, MyoDeviceData& myoData);
	virtual void MyoLatestData(int32 myoId, int32& Pose, FVector& Acceleration, FRotator& Rotation, FVector& Gyro, int32& Arm);	//overloaded convenience function
	virtual void MyoWhichArm(int32 myoId, int32& Arm);
	virtual void MyoLeftMyoId(bool& available, int32& myoId);		//convenience function, gets the myoId of the currently myo bound to the left arm
	virtual void MyoRightMyoId(bool& available, int32& myoId);	//convenience function, gets the myoId of the currently myo bound to the right arm
	virtual void MyoConvertToRawOrientation(FRotator orientation, FRotator& converted);	//if you want to use the raw myo orientation not the UE4 formatted one, run your rotator through this

	//Required Functions
	virtual void MyoStartup();
	virtual void MyoTick(float DeltaTime);
};