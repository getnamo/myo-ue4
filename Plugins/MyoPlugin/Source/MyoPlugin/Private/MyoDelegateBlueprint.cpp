#include "MyoPluginPrivatePCH.h"
#include "MyoDelegateBlueprint.h"


//Events
void MyoDelegateBlueprint::MyoOnConnect(int32 myoId, uint64 timestamp)
{
	OnConnect(myoId);
}
void MyoDelegateBlueprint::MyoOnDisconnect(int32 myoId, uint64 timestamp)
{
	OnDisconnect(myoId);
}
void MyoDelegateBlueprint::MyoOnPair(int32 myoId, uint64 timestamp)
{
	OnPair(myoId);
}
void MyoDelegateBlueprint::MyoOnUnpair(int32 myoId, uint64 timestamp)
{
	//OnUnpair(myoId);
}
void MyoDelegateBlueprint::MyoOnOrientationData(int32 myoId, uint64 timestamp, FRotator rotation)
{
	OnOrientationData(myoId, rotation);
}
void MyoDelegateBlueprint::MyoOnAccelerometerData(int32 myoId, uint64 timestamp, FVector accel)
{
	OnAccelerometerData(myoId, accel);
}
void MyoDelegateBlueprint::MyoOnGyroscopeData(int32 myoId, uint64 timestamp, FVector gyro)
{
	OnGyroscopeData(myoId, gyro);
}
void MyoDelegateBlueprint::MyoOnPose(int32 myoId, uint64 timestamp, int32 pose)
{
	OnPose(myoId, pose);
}
void MyoDelegateBlueprint::MyoOnArmRecognized(int32 myoId, uint64 timestamp, int32 arm, int32 direction)
{
	OnArmRecognized(myoId, arm, direction);
}
void MyoDelegateBlueprint::MyoOnArmLost(int32 myoId, uint64 timestamp)
{
	OnArmLost(myoId);
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

void MyoDelegateBlueprint::LatestData(	int32 myoId, int32& Pose, FVector& Acceleration, FRotator& Orientation, FVector& Gyro,
										int32& Arm, int32& xDirection,
										FVector& ArmAcceleration, FRotator& ArmOrientation, FVector& ArmGyro, FRotator& ArmCorrection,
										FVector& BodySpaceAcceleration) {

	MyoLatestData(myoId, Pose, Acceleration, Orientation, Gyro, Arm, xDirection, ArmAcceleration, ArmOrientation, ArmGyro, ArmCorrection, BodySpaceAcceleration);
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
void MyoDelegateBlueprint::ConvertToMyoOrientationSpace(FRotator orientation, FRotator& converted)
{
	MyoConvertToMyoOrientationSpace(orientation, converted);
}

void MyoDelegateBlueprint::CalibrateArmOrientation(int32 myoId)
{
	MyoCalibrateArmOrientation(myoId);
}

void MyoDelegateBlueprint::MyoTick(float DeltaTime)
{
	MyoDelegate::MyoTick(DeltaTime);
}
void MyoDelegateBlueprint::MyoStartup()
{
	MyoDelegate::MyoStartup();
}