#pragma once
#include "MyoDelegate.h"

//Declares Blueprint event calls, only 

class MyoDelegateBlueprint : public MyoDelegate
{
	//Delegate function override to passthrough and convert parameters to blueprint
	virtual void MyoOnConnect(int32 myoId, uint64 timestamp) override;
	virtual void MyoOnDisconnect(int32 myoId, uint64 timestamp) override;
	virtual void MyoOnPair(int32 myoId, uint64 timestamp) override;
	virtual void MyoOnUnpair(int32 myoId, uint64 timestamp) override;
	virtual void MyoOnOrientationData(int32 myoId, uint64 timestamp, FRotator rot) override;
	virtual void MyoOnAccelerometerData(int32 myoId, uint64 timestamp, FVector accel) override;
	virtual void MyoOnGyroscopeData(int32 myoId, uint64 timestamp, FVector gyro) override;
	virtual void MyoOnPose(int32 myoId, uint64 timestamp, int32 pose) override;
	virtual void MyoOnArmRecognized(int32 myoId, uint64 timestamp, int32 arm, int32 direction) override;
	virtual void MyoOnArmLost(int32 myoId, uint64 timestamp) override;
	virtual void MyoDisabled() override;

public:

	//Blueprint Events, name them the same way with UFunction declaration. These contain no timestamps (unsupported in blueprints due to 64bit value)
	virtual void OnPose(int32 myoId, int32 pose) = 0;
	virtual void OnArmRecognized(int32 myoId, int32 arm, int32 direction) = 0;
	virtual void OnArmLost(int32 myoId) = 0;
	virtual void OnConnect(int32 myoId) = 0;
	virtual void OnDisconnect(int32 myoId) = 0;
	virtual void OnPair(int32 myoId) = 0;
	//virtual void OnUnpair(int32 myoId) = 0;
	virtual void OnOrientationData(int32 myoId, FRotator rotation) = 0;
	virtual void OnAccelerometerData(int32 myoId, FVector acceleration) = 0;
	virtual void OnGyroscopeData(int32 myoId, FVector gyro) = 0;
	virtual void DeviceDisabled() = 0;

	//Callable Blueprint Functions
	virtual void VibrateDevice(int32 myoId, int32 type);
	virtual bool IsHubEnabled();
	virtual void LatestData(	int32 myoId, int32& Pose, FVector& Acceleration, FRotator& Orientation, FVector& Gyro,
								int32& Arm, int32& xDirection,
								FVector& ArmAcceleration, FRotator& ArmOrientation, FVector& ArmGyro, FRotator& ArmCorrection,
								FVector& BodySpaceAcceleration);
	virtual void WhichArm(int32 myoId, int32& Arm);
	virtual void LeftMyoId(bool& available, int32& myoId);
	virtual void RightMyoId(bool& available, int32& myoId);
	virtual void ConvertToMyoOrientationSpace(FRotator orientation, FRotator& converted);
	virtual void CalibrateArmOrientation(int32 myoId);					//Uses current orientation as arm zero point (ask user to point arm to screen and call this)

	//Required functions to make plugin work
	virtual void MyoStartup() override;
	virtual void MyoTick(float DeltaTime) override;
};