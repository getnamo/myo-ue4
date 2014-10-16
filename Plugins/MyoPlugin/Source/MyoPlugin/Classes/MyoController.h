#pragma once

#include "MyoDelegate.h"
#include "MyoController.generated.h"

UCLASS(BlueprintType)
class UMyoController : public UObject
{
friend class MyoDelegateBlueprint;
	GENERATED_UCLASS_BODY()
public:

	//Can be rest, fist, waveIn, waveOut, fingersSpread, reserved1, thumbToPinky, unknown
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Myo Frame")
	TEnumAsByte<MyoPose> pose;

	//acceleration in units of g
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Myo Frame")
	FVector acceleration;

	//orientation in rotator format
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Myo Frame")
	FRotator orientation;

	// gyroscope vector in deg / s
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Myo Frame")
	FVector gyro;

	//right, left, unknown
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Myo Frame")
	TEnumAsByte<MyoArm> arm;

	//toward wrist, toward elbow, unknown
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Myo Frame")
	TEnumAsByte<MyoArmDirection> xDirection;

	//acceleration in arm space given in units of g
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Myo Frame")
	FVector armAcceleration;

	//orientation in arm space
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Myo Frame")
	FRotator armOrientation;

	//gyro in arm space
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Myo Frame")
	FVector armGyro;

	//the correction rotation to transform raw into arm space
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Myo Frame")
	FRotator armSpaceCorrection;

	//BodySpaceAcceleration(out) acceleration in calibrated body space, with gravity removed. (This value is used for velocity and position integration)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Myo Frame")
	FVector bodySpaceNullAcceleration;

	//Id identifying myo, used for calling functions
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Myo Frame")
	int32 myoId;

	//Convenience Call, optionally check hand possession property
	UFUNCTION(BlueprintCallable, Category = "Myo Frame")
	bool isOnLeftArm();

	//Convenience Call, optionally check hand possession property
	UFUNCTION(BlueprintCallable, Category = "Myo Frame")
	bool isOnRightArm();

	/**
	* Vibrate the specified Myo with type intensity
	* @param myoId (in) starting from 1, based on pairing priority. Call LeftMyoId/RightMyoId
	* @param type (in) Options: Short, Medium, Long
	*/
	UFUNCTION(BlueprintCallable, Category = MyoFunctions)
	void VibrateDevice(MyoVibrationType type);

	/**
	* Calibrate the Orientation for Arm Space. Ask the user to point their arm to the screen, then call this function and all orientation will be
	* in arm orientation space.
	* @param myoId (in) device you wish to calibrate, use 0 to calibrate all.
	* @param direction (in) orientation of the calibrated myo with respect to the user forward vector (R0,P0,Y0) specifies into the screen, (R0,P0,Y90) specifies pointing right, etc.
	*/
	UFUNCTION(BlueprintCallable, Category = MyoFunctions)
	void CalibrateArmOrientation(FRotator direction);

	//Conversion
	void setFromMyoDeviceData(MyoDeviceData* data);

private:
	MyoDelegate* _myoDelegate;
};