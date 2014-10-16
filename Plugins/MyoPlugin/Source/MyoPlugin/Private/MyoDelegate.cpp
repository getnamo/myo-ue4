#include "MyoPluginPrivatePCH.h"

#include "IMyoPlugin.h"
#include "MyoDelegate.h"

//Input Mapping EKey definitions
const FKey EKeysMyo::MyoPoseRest("MyoPoseRest");
const FKey EKeysMyo::MyoPoseFist("MyoPoseFist");
const FKey EKeysMyo::MyoPoseWaveIn("MyoPoseWaveIn");
const FKey EKeysMyo::MyoPoseWaveOut("MyoPoseWaveOut");
const FKey EKeysMyo::MyoPoseFingersSpread("MyoPoseFingersSpread");
const FKey EKeysMyo::MyoPoseThumbToPinky("MyoPoseThumbToPinky");
const FKey EKeysMyo::MyoPoseUnknown("MyoPoseUnknown");
const FKey EKeysMyo::MyoAccelerationX("MyoAccelerationX");
const FKey EKeysMyo::MyoAccelerationY("MyoAccelerationY");
const FKey EKeysMyo::MyoAccelerationZ("MyoAccelerationZ");
const FKey EKeysMyo::MyoOrientationPitch("MyoOrientationPitch");
const FKey EKeysMyo::MyoOrientationYaw("MyoOrientationYaw");
const FKey EKeysMyo::MyoOrientationRoll("MyoOrientationRoll");
const FKey EKeysMyo::MyoGyroX("MyoGyroX");
const FKey EKeysMyo::MyoGyroY("MyoGyroY");
const FKey EKeysMyo::MyoGyroZ("MyoGyroZ");

//Empty Implementations
void MyoDelegate::MyoOnConnect(int32 myoId, uint64 timestamp){}
void MyoDelegate::MyoOnDisconnect(int32 myoId, uint64 timestamp){}
void MyoDelegate::MyoOnPair(int32 myoId, uint64 timestamp){}
void MyoDelegate::MyoOnUnpair(int32 myoId, uint64 timestamp){}
void MyoDelegate::MyoOnArmMoved(int32 myoId, FVector armAcceleration, FRotator armOrientation, FVector armGyro, MyoPose pose){}
void MyoDelegate::MyoOnOrientationData(int32 myoId, uint64 timestamp, FQuat quat){}
void MyoDelegate::MyoOnOrientationData(int32 myoId, uint64 timestamp, FRotator rot){}
void MyoDelegate::MyoOnAccelerometerData(int32 myoId, uint64 timestamp, FVector accel){}
void MyoDelegate::MyoOnGyroscopeData(int32 myoId, uint64 timestamp, FVector gyro){}
void MyoDelegate::MyoOnPose(int32 myoId, uint64 timestamp, int32 pose){}
void MyoDelegate::MyoOnArmRecognized(int32 myoId, uint64 timestamp, int32 arm, int32 direction){}
void MyoDelegate::MyoOnArmLost(int32 myoId, uint64 timestamp){}

void MyoDelegate::MyoDisabled(){}

//Vibrate
void MyoDelegate::MyoVibrateDevice(int32 myoId, int32 type)
{
	if (IMyoPlugin::IsAvailable())
	{
		IMyoPlugin::Get().VibrateDevice(myoId, type);
	}
}

bool MyoDelegate::MyoIsHubEnabled()
{
	if (IMyoPlugin::IsAvailable())
	{
		return IMyoPlugin::Get().IsHubEnabled();
	}
	else{
		return false;
	}
}

MyoDeviceData* MyoDelegate::MyoLatestData(int32 myoId)
{
	if (IMyoPlugin::IsAvailable())
	{
		return IMyoPlugin::Get().LatestData(myoId);
	}
	return NULL;
}

void MyoDelegate::MyoLatestData(int32 myoId, int32& Pose, FVector& Acceleration, FRotator& Orientation, FVector& Gyro,
					int32& Arm, int32& xDirection,
					FVector& ArmAcceleration, FRotator& ArmOrientation, FVector& ArmGyro, FRotator& ArmCorrection,
					FVector& BodySpaceAcceleration){

	MyoDeviceData data = *MyoLatestData(myoId);

	Pose = data.pose;
	Acceleration = data.acceleration;
	Orientation = data.orientation;
	Gyro = data.gyro;
	
	Arm = data.arm;
	xDirection = data.xDirection;
	
	ArmAcceleration = data.armAcceleration;
	ArmOrientation = data.armOrientation;
	ArmGyro = data.armGyro;
	ArmCorrection = data.armSpaceCorrection;

	BodySpaceAcceleration = data.bodySpaceNullAcceleration;
}

void MyoDelegate::MyoWhichArm(int32 myoId, int32& Arm)
{
	if (IMyoPlugin::IsAvailable())
	{
		IMyoPlugin::Get().WhichArm(myoId, Arm);
	}
}
void MyoDelegate::MyoLeftMyoId(bool& available, int32& myoId)
{
	if (IMyoPlugin::IsAvailable())
	{
		IMyoPlugin::Get().LeftMyoId(available, myoId);
	}
}
void MyoDelegate::MyoRightMyoId(bool& available, int32& myoId)
{
	if (IMyoPlugin::IsAvailable())
	{
		IMyoPlugin::Get().RightMyoId(available, myoId);
	}
}
void MyoDelegate::MyoPrimaryMyoId(bool& available, int32& myoId)
{
	if (IMyoPlugin::IsAvailable())
	{
		IMyoPlugin::Get().PrimaryMyoId(available, myoId);
	}
}

int32 MyoDelegate::MyoMaxId()
{
	int size = 0;
	if (IMyoPlugin::IsAvailable())
	{
		IMyoPlugin::Get().MaxMyoId(size);
	}
	return size;
}

bool MyoDelegate::MyoIsValidId(int32 myoId)
{
	if (IMyoPlugin::IsAvailable())
	{
		return IMyoPlugin::Get().IsValidDeviceId(myoId);
	}
	return false;
}

void MyoDelegate::MyoConvertToMyoOrientationSpace(FRotator orientation, FRotator& converted)
{
	//Converts the orientation back to the myo base orientation
	//Pitch and Yaw have to be reversed, assumes pointing to wrist as base
	converted.Pitch = -1.f*orientation.Pitch;
	converted.Yaw = -1.f*orientation.Yaw;
	converted.Roll = orientation.Roll;
}

void MyoDelegate::MyoCalibrateArmOrientation(int32 myoId, FRotator direction)
{
	if (IMyoPlugin::IsAvailable())
	{
		IMyoPlugin::Get().CalibrateOrientation(myoId, direction);
	}
}


void MyoDelegate::MyoTick(float DeltaTime)
{
	if (IMyoPlugin::IsAvailable())
	{
		IMyoPlugin::Get().MyoTick(DeltaTime);
	}
}
void MyoDelegate::MyoStartup()
{
	if (IMyoPlugin::IsAvailable())
	{
		IMyoPlugin::Get().SetDelegate((MyoDelegate*)this);
	}
}

void MyoDelegate::MyoShutdown()
{
	if (IMyoPlugin::IsAvailable())
	{
		IMyoPlugin::Get().RemoveDelegate();
	}
}