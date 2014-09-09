#include "MyoPluginPrivatePCH.h"

#include "IMyoPlugin.h"
#include "MyoDelegate.h"

//Empty Implementations
void MyoDelegate::MyoOnConnect(int32 myoId, uint64 timestamp){}
void MyoDelegate::MyoOnDisconnect(int32 myoId, uint64 timestamp){}
void MyoDelegate::MyoOnPair(int32 myoId, uint64 timestamp){}
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

void MyoDelegate::MyoLatestData(int32 myoId, MyoDeviceData& myoData)
{
	if (IMyoPlugin::IsAvailable())
	{
		IMyoPlugin::Get().LatestData(myoId, myoData);
	}
}

void MyoDelegate::MyoLatestData(int32 myoId, int32& Pose, FVector& Acceleration, FRotator& Rotation, FVector& Gyro, int32& Arm)
{
	MyoDeviceData data;
	MyoLatestData(myoId, data);

	Pose = data.pose;
	Acceleration = data.acceleration;
	Rotation = data.orientation;
	Gyro = data.gyro;
	Arm = data.arm;

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

void MyoDelegate::MyoConvertToRawOrientation(FRotator orientation, FRotator& converted)
{
	//Converts the orientation back to the myo base orientation
	//Pitch and Yaw have to be reversed, assumes pointing to wrist as base
	converted.Pitch = -1.f*orientation.Pitch;
	converted.Yaw = -1.f*orientation.Yaw;
	converted.Roll = orientation.Roll;
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