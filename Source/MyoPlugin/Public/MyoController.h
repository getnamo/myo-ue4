#pragma once

#include "MyoEnum.h"
#include "MyoController.generated.h"

USTRUCT(BlueprintType)
struct MYOPLUGIN_API FMyoControllerData
{
	GENERATED_USTRUCT_BODY()

	//Can be rest, fist, waveIn, waveOut, fingersSpread, reserved1, DoubleTap, unknown
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Myo Controller Data")
	TEnumAsByte<EMyoPose> Pose;

	//acceleration in units of g
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Myo Controller Data")
	FVector Acceleration;

	//orientation in rotator format
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Myo Controller Data")
	FRotator Orientation;

	// gyroscope vector in deg / s
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Myo Controller Data")
	FVector Gyro;

	//right, left, unknown
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Myo Controller Data")
	TEnumAsByte<EMyoArm> Arm;

	//toward wrist, toward elbow, unknown
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Myo Controller Data")
	TEnumAsByte<EMyoArmDirection> ArmDirection;

	//acceleration in arm space given in units of g
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Myo Controller Data")
	FVector ArmAcceleration;

	//orientation in arm space
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Myo Controller Data")
	FRotator ArmOrientation;

	//gyro in arm space
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Myo Controller Data")
	FVector ArmGyro;

	//the correction rotation to transform raw into arm space
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Myo Controller Data")
	FRotator ArmSpaceCorrection;

	//BodySpaceAcceleration(out) acceleration in calibrated body space, with gravity removed. (This value is used for velocity and position integration)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Myo Controller Data")
	FVector BodySpaceNullAcceleration;

	//Id identifying myo, used for calling functions
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Myo Controller Data")
	int32 MyoId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Myo Controller Data")
	int32 BatteryLevel;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Myo Controller Data")
	int32 RSSI;

	//Conversion
	void setFromMyoDeviceData(FMyoDeviceData* Data);
};



UCLASS(BlueprintType)
class MYOPLUGIN_API UMyoController : public UObject
{
friend class MyoDelegateBlueprint;
	GENERATED_UCLASS_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Myo Frame")
	FMyoControllerData MyoData;

	//Convenience Call, optionally check hand possession property
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Myo Frame")
	bool isOnLeftArm();

	//Convenience Call, optionally check hand possession property
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Myo Frame")
	bool isOnRightArm();

	/**
	* Vibrate the specified Myo with type intensity
	* @param myoId (in) starting from 1, based on pairing priority. Call LeftMyoId/RightMyoId
	* @param type (in) Options: Short, Medium, Long
	*/
	UFUNCTION(BlueprintCallable, Category = MyoFunctions)
	void VibrateDevice(EMyoVibrationType Type);

	/**
	* Calibrate the Orientation for Arm Space. Ask the user to point their arm to the screen, then call this function and all orientation will be
	* in arm orientation space.
	* @param myoId (in) device you wish to calibrate, use 0 to calibrate all.
	* @param direction (in) orientation of the calibrated myo with respect to the user forward vector (R0,P0,Y0) specifies into the screen, (R0,P0,Y90) specifies pointing right, etc.
	*/
	UFUNCTION(BlueprintCallable, Category = MyoFunctions)
	void CalibrateArmOrientation(FRotator Direction);

	/**
	* Unlocks the Myo if locked to detect gestures
	* @param type (in) type of unlock (short period or indefinite hold)
	*/
	UFUNCTION(BlueprintCallable, Category = MyoFunctions)
	void Unlock(EMyoUnlockType Type);

	/**
	* Tell the Myo to stay unlocked until told otherwise.
	*/
	UFUNCTION(BlueprintCallable, Category = MyoFunctions)
	void Lock();

	/**
	* Sets the EMG streaming mode for this Myo
	*/
	UFUNCTION(BlueprintCallable, Category = MyoFunctions)
	void SetStreamEmg(EMyoStreamEmgType StreamType);
};