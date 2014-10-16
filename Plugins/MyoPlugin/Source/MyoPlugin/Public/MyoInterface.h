#pragma once

#include "MyoController.h"
#include "MyoInterface.generated.h"

UINTERFACE(MinimalAPI)
class UMyoInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class IMyoInterface
{
	GENERATED_IINTERFACE_BODY()

public:

	/**
	* Event on a Myo connecting
	* @param myo (out) pointer to emitted myo controller class, branch to read other data.
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = MyoEvents)
	void OnConnect(UMyoController* myo);

	/**
	* Event on a Myo disconnecting
	* @param myo (out) pointer to emitted myo controller class, branch to read other data.
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = MyoEvents)
	void OnDisconnect(UMyoController* myo);

	/**
	* Event on a Myo pairing
	* @param myo (out) pointer to emitted myo controller class, branch to read other data.
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = MyoEvents)
	void OnPair(UMyoController* myo);

	/**
	* Event on a Myo unpairing
	* @param myo (out) pointer to emitted myo controller class, branch to read other data.
	*/
	//UFUNCTION(BlueprintImplementableEvent, Category = MyoEvents)
	//void OnUnpair(MyoController* myo);


	/**
	*	Event (non-thalmic api) giving calibrated values
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = MyoEvents)
	void OnArmMoved(UMyoController* myo, FVector armAcceleration, FRotator armOrientation, FVector armGyro, MyoPose pose);

	/**
	* Event on a Myo receiving orientation data, typically each frame.
	* @param myo (out) pointer to emitted myo controller class, branch to read other data.
	* @param orientation (out) orientation in rotator format.
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = MyoEvents)
	void OnOrientationData(UMyoController* myo, FRotator orientation);

	/**
	* Event on a Myo receiving acceleration data, typically each frame.
	* @param myo (out) pointer to emitted myo controller class, branch to read other data.
	* @param acceleration (out) acceleration in units of g.
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = MyoEvents)
	void OnAccelerometerData(UMyoController* myo, FVector acceleration);

	/**
	* Event on a Myo receiving gyroscope data, typically each frame.
	* @param myo (out) pointer to emitted myo controller class, branch to read other data.
	* @param gyro (out) gyroscope vector in deg/s.
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = MyoEvents)
	void OnGyroscopeData(UMyoController* myo, FVector gyro);

	/**
	* Event on a Myo detecting a pose.
	* @param myo (out) pointer to emitted myo controller class, branch to read other data.
	* @param pose (out): EMG pose, refer to Thalmic API rest, fist, waveIn, waveOut, fingersSpread, reserved1, thumbToPinky, unknown
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = MyoEvents)
	void OnPose(UMyoController* myo, MyoPose pose);

	/**
	* On arm detection, typically after a wave out gesture
	* @param myo (out) pointer to emitted myo controller class, branch to read other data.
	* @param arm (out) 0 = right, 1 = left, 2 = unknown
	* @param direction (out) 0 = toward wrist, 1 = toward elbow, 2 = unknown
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = MyoEvents)
	void OnArmRecognized(UMyoController* myo, MyoArm arm, MyoArmDirection direction);

	/**
	* Called when a myo has been removed from arm.
	* @param myo (out) pointer to emitted myo controller class, branch to read other data.
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = MyoEvents)
	void OnArmLost(UMyoController* myo);

	/**
	* Called when a problem occurs such as bluetooth usb device not being detected (unplugged).
	* If you receive this event further Myo events will not work until you plug in the hub and restart the application.
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = MyoEvents)
	void DeviceDisabled();


	virtual FString ToString();
};