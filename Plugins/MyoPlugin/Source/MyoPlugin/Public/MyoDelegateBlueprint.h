#pragma once
#include "MyoDelegate.h"

//Declares Blueprint event calls, only 

class MyoDelegateBlueprint : public MyoDelegate
{
	//Delegate function override to passthrough and convert parameters to blueprint
	virtual void MyoOnConnect(int32 myoId, uint64 timestamp) override;
	virtual void MyoOnDisconnect(int32 myoId, uint64 timestamp) override;
	virtual void MyoOnPair(int32 myoId, uint64 timestamp) override;
	virtual void MyoOnOrientationData(int32 myoId, uint64 timestamp, FRotator rot) override;
	virtual void MyoOnAccelerometerData(int32 myoId, uint64 timestamp, FVector accel) override;
	virtual void MyoOnGyroscopeData(int32 myoId, uint64 timestamp, FVector gyro) override;
	virtual void MyoOnPose(int32 myoId, uint64 timestamp, int32 pose) override;
	virtual void MyoOnArmRecognized(int32 myoId, uint64 timestamp, int32 arm, int32 direction) override;
	virtual void MyoOnArmLost(int32 myoId, uint64 timestamp) override;
	virtual void MyoDisabled() override;

public:

	//Blueprint Events, name them the same way with UFunction declaration
	virtual void OnPose(int32 myoId, int32 timestamp, int32 pose) = 0;
	virtual void OnArmRecognized(int32 myoId, int32 timestamp, int32 arm, int32 direction) = 0;
	virtual void OnArmLost(int32 myoId, int32 timestamp) = 0;
	virtual void OnConnect(int32 myoId, int32 timestamp) = 0;
	virtual void OnDisconnect(int32 myoId, int32 timestamp) = 0;
	virtual void OnPair(int32 myoId, int32 timestamp) = 0;
	virtual void OnOrientationData(int32 myoId, int32 timestamp, FRotator rotation) = 0;
	virtual void OnAccelerometerData(int32 myoId, int32 timestamp, FVector acceleration) = 0;
	virtual void OnGyroscopeData(int32 myoId, int32 timestamp, FVector gyro) = 0;
	virtual void DeviceDisabled() = 0;

	//Callable Blueprint Functions
	virtual void VibrateDevice(int32 myoId, int32 type);
	virtual bool IsHubEnabled();
	virtual void LatestData(int32 myoId, int32& pose, FVector& Acceleration, FRotator& Rotation, FVector& Gyro, int32& Arm);
	virtual void WhichArm(int32 myoId, int32& Arm);
	virtual void LeftMyoId(bool& available, int32& myoId);
	virtual void RightMyoId(bool& available, int32& myoId);
	virtual void ConvertToRawOrientation(FRotator orientation, FRotator& converted);

	//Required functions to make plugin work
	virtual void MyoStartup() override;
	virtual void MyoTick(float DeltaTime) override;
};