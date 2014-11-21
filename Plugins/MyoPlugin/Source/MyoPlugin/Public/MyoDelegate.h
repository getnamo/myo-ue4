#pragma once
#include "MyoDelegate.generated.h"

/** Inherit override and set delegate to subscribe to callbacks*/

UENUM(BlueprintType)
enum MyoArm
{
	MYO_ARM_RIGHT = 0,
	MYO_ARM_LEFT,
	MYO_ARM_UNKNOWN
};

UENUM(BlueprintType)
enum MyoArmDirection
{
	MYO_TOWARD_WRIST = 0,
	MYO_TOWARD_ELBOW,
	MYO_DIRECTION_UNKNOWN
};

UENUM(BlueprintType)
enum MyoPose
{
	MYO_POSE_REST = 0,
	MYO_POSE_FIST,
	MYO_POSE_WAVEIN,
	MYO_POSE_WAVEOUT,
	MYO_POSE_FINGERSPREAD,
	MYO_POSE_RESERVED1,
	MYO_POSE_THUMBTOPINKY,
	MYO_POSE_UNKNOWN = 255	//this needs to be properly switched...
};

UENUM(BlueprintType)
enum MyoVibrationType
{
	MYO_VIBRATION_SHORT = 0,
	MYO_VIBRATION_MEDIUM,
	MYO_VIBRATION_LONG
};

//Latest Data structure
struct MyoDeviceData{
	//Raw data (as of beta4)
	int pose;				//0 = rest, 1 = fist, 2 = waveIn, 3 = waveOut, 4 = fingersSpread, 5 = reserved1, 6 = thumbToPinky, 65535 = unknown
	FVector acceleration;	//units of g
	FQuat quaternion;		//orientation in quaternion format
	FRotator orientation;	//orientation
	FVector gyro;			//angular speed in deg/s
	int arm;				//which arm it is bound to, 0=right, 1=left, 2=unknown
	int xDirection;			//0 = toward wrist, 1= toward elbow, 2=unknown

	//Plugin Derived - Values given arm space after arm calibration, otherwise same as raw
	FVector armAcceleration;	//units of g
	FRotator armOrientation;	//orientation
	FVector armGyro;			//angular speed in deg/s
	FRotator armSpaceCorrection;	//used to calibrate the orientation, not exposed to blueprints

	//Body space, useful for easy component space integration
	FVector bodySpaceNullAcceleration;	//units of g, in calibrated space, without gravity component
};

//Input Mapping Key Structure
struct EKeysMyo
{
	//Posess
	static const FKey MyoPoseRest;
	static const FKey MyoPoseFist;
	static const FKey MyoPoseWaveIn;
	static const FKey MyoPoseWaveOut;
	static const FKey MyoPoseFingersSpread;
	static const FKey MyoPoseThumbToPinky;
	static const FKey MyoPoseUnknown;

	//Axis, given in arm orientation by default (use calibration or use raw functions to specify)
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
	virtual void MyoOnUnpair(int32 myoId, uint64 timestamp);
	virtual void MyoOnArmMoved(int32 myoId, FVector armAcceleration, FRotator armOrientation, FVector armGyro, MyoPose pose);
	virtual void MyoOnOrientationData(int32 myoId, uint64 timestamp, FQuat quat);
	virtual void MyoOnOrientationData(int32 myoId, uint64 timestamp, FRotator rot);	//forward rotator version for blueprint events
	virtual void MyoOnAccelerometerData(int32 myoId, uint64 timestamp, FVector accel);
	virtual void MyoOnGyroscopeData(int32 myoId, uint64 timestamp, FVector gyro);
	virtual void MyoOnPose(int32 myoId, uint64 timestamp, int32 pose);
	virtual void MyoOnArmSync(int32 myoId, uint64 timestamp, int32 arm, int32 direction);
	virtual void MyoOnArmUnsync(int32 myoId, uint64 timestamp);

	/*Myo being disabled can occur if we do not have our bluetooth device in before launching app, 
	 in that case MyoDisabled() will emit.*/
	virtual void MyoDisabled();	

	/** Callable Functions */
	virtual void MyoVibrateDevice(int32 myoId, int32 type);
	virtual bool MyoIsHubEnabled();
	virtual MyoDeviceData* MyoLatestData(int32 myoId);
	virtual void MyoLatestData(	int32 myoId, int32& Pose, FVector& Acceleration, FRotator& Orientation, FVector& Gyro, 
								int32& Arm, int32& xDirection, 
								FVector& ArmAcceleration, FRotator& ArmOrientation, FVector& ArmGyro, FRotator& ArmCorrection,
								FVector& BodySpaceAcceleration);
	virtual void MyoWhichArm(int32 myoId, int32& Arm);
	virtual void MyoLeftMyoId(bool& available, int32& myoId);		//convenience function, gets the myoId of the currently myo bound to the left arm
	virtual void MyoRightMyoId(bool& available, int32& myoId);	//convenience function, gets the myoId of the currently myo bound to the right arm
	virtual void MyoPrimaryMyoId(bool& available, int32& myoId);
	virtual int32 MyoMaxId();
	virtual bool MyoIsValidId(int32 myoId);
	virtual void MyoConvertToMyoOrientationSpace(FRotator orientation, FRotator& converted);	//if you want to use the raw myo orientation not the UE4 formatted one, run your rotator through this
	virtual void MyoCalibrateArmOrientation(int32 myoId, FRotator direction);					//Uses current orientation as arm zero point (ask user to point arm to screen and call this)

	//Required Functions
	virtual void MyoStartup();
	virtual void MyoShutdown();
	virtual void MyoTick(float DeltaTime);
};