#pragma once
#include "MyoDelegate.h"
#include "MyoInterface.h"

//Declares Blueprint event calls, only 
#include "MyoInterface.h"

class MyoDelegateBlueprint : public MyoDelegate
{
public:

	//Component Functions - override these in your classes to expose to blueprint, need different naming however (e.g. drop Myo prefix)
	virtual UMyoController* MyoPrimaryMyo();
	virtual UMyoController* MyoLeftMyo();
	virtual UMyoController* MyoRightMyo();
	virtual bool MyoIsHubEnabled() override;
	virtual void MyoConvertToMyoOrientationSpace(FRotator orientation, FRotator& converted) override;

	//Required functions to make plugin work
	virtual void MyoStartup() override;
	virtual void MyoShutdown() override;
	virtual void MyoTick(float DeltaTime) override;

	//If you want an alternate delegate, set it here
	void SetInterfaceDelegate(UObject* newDelegate);
protected:
	UObject* ValidSelfPointer;	//REQUIRED: has to be set before MyoStartup by a UObject subclass.

private:
	//Delegate function override to passthrough and convert parameters to blueprint
	virtual void MyoOnConnect(int32 myoId, uint64 timestamp) override;
	virtual void MyoOnDisconnect(int32 myoId, uint64 timestamp) override;
	virtual void MyoOnPair(int32 myoId, uint64 timestamp) override;
	virtual void MyoOnUnpair(int32 myoId, uint64 timestamp) override;
	virtual void MyoOnArmMoved(int32 myoId, FVector armAcceleration, FRotator armOrientation, FVector armGyro, MyoPose pose);
	virtual void MyoOnOrientationData(int32 myoId, uint64 timestamp, FRotator rot) override;
	virtual void MyoOnAccelerometerData(int32 myoId, uint64 timestamp, FVector accel) override;
	virtual void MyoOnGyroscopeData(int32 myoId, uint64 timestamp, FVector gyro) override;
	virtual void MyoOnPose(int32 myoId, uint64 timestamp, int32 pose) override;
	virtual void MyoOnArmSync(int32 myoId, uint64 timestamp, int32 arm, int32 direction) override;
	virtual void MyoOnArmUnsync(int32 myoId, uint64 timestamp) override;
	virtual void MyoDisabled() override;

	UMyoController* InternalAddController(int newId);
	UMyoController* InternalControllerForId(int32 myoId);
	UObject* _interfaceDelegate;
	TArray<class UMyoController*> _latestFrame;
};