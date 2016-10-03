#pragma once

#include "MyoControllerData.h"
#include "MyoController.generated.h"

//todo: decide, should this be an fstruct wrapped?

UCLASS(BlueprintType)
class MYOPLUGIN_API UMyoController : public UObject
{
	GENERATED_UCLASS_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Myo Frame")
	FMyoControllerData FrameData;

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