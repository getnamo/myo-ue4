#include "MyoPluginPrivatePCH.h"
#include "MyoDelegateBlueprint.h"


//Events
void MyoDelegateBlueprint::MyoOnConnect(int32 myoId, uint64 timestamp)
{
	OnConnect(myoId, timestamp);
}
void MyoDelegateBlueprint::MyoOnDisconnect(int32 myoId, uint64 timestamp)
{
	OnDisconnect(myoId, timestamp);
}
void MyoDelegateBlueprint::MyoOnPair(int32 myoId, uint64 timestamp)
{
	OnPair(myoId, timestamp);
}
void MyoDelegateBlueprint::MyoOnOrientationData(int32 myoId, uint64 timestamp, FRotator rotation)
{
	OnOrientationData(myoId, timestamp, rotation);
}
void MyoDelegateBlueprint::MyoOnAccelerometerData(int32 myoId, uint64 timestamp, FVector accel)
{
	OnAccelerometerData(myoId, timestamp, accel);
}
void MyoDelegateBlueprint::MyoOnGyroscopeData(int32 myoId, uint64 timestamp, FVector gyro)
{
	OnGyroscopeData(myoId, timestamp, gyro);
}
void MyoDelegateBlueprint::MyoOnPose(int32 myoId, uint64 timestamp, int32 pose)
{
	OnPose(myoId, timestamp, pose);
}
void MyoDelegateBlueprint::MyoOnArmRecognized(int32 myoId, uint64 timestamp, int32 arm, int32 direction)
{
	OnArmRecognized(myoId, timestamp, arm, direction);
}
void MyoDelegateBlueprint::MyoOnArmLost(int32 myoId, uint64 timestamp)
{
	OnArmLost(myoId, timestamp);
}
void MyoDelegateBlueprint::MyoDisabled()
{
	DeviceDisabled();
}

//Functions
void MyoDelegateBlueprint::VibrateDevice(int32 myoId, int32 type)
{
	MyoVibrateDevice(myoId, type);
}
bool MyoDelegateBlueprint::IsHubEnabled()
{
	return MyoIsHubEnabled();
}
void MyoDelegateBlueprint::LatestData(int32 myoId, int32& Pose, FVector& Acceleration, FRotator& Rotation, FVector& Gyro, int32& Arm)
{
	MyoLatestData(myoId, Pose, Acceleration, Rotation, Gyro, Arm);
}
void MyoDelegateBlueprint::WhichArm(int32 myoId, int32& Arm)
{
	MyoWhichArm(myoId, Arm);
}
void MyoDelegateBlueprint::LeftMyoId(bool& available, int32& myoId)
{
	MyoLeftMyoId(available, myoId);
}
void MyoDelegateBlueprint::RightMyoId(bool& available, int32& myoId)
{
	MyoRightMyoId(available, myoId);
}
void MyoDelegateBlueprint::ConvertToRawOrientation(FRotator orientation, FRotator& converted)
{
	MyoConvertToRawOrientation(orientation, converted);
}

void MyoDelegateBlueprint::MyoTick(float DeltaTime)
{
	MyoDelegate::MyoTick(DeltaTime);
}
void MyoDelegateBlueprint::MyoStartup()
{
	MyoDelegate::MyoStartup();
}