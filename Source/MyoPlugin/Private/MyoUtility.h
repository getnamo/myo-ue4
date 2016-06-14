#pragma once

#include <iostream>
#include <stdexcept>
#include <vector>
#include <myo/myo.hpp>
#include "SlateBasics.h"

namespace MyoUtility
{
	#define LOCTEXT_NAMESPACE "MyoPlugin"
	#define ORIENTATION_SCALE_PITCH 0.01111111111	//1/90
	#define ORIENTATION_SCALE_YAWROLL 0.00555555555 //1/180
	#define GYRO_SCALE 0.02222222222				//1/45


	//UE IM event wrappers
	bool EmitKeyUpEventForKey(FKey key, int32 user, bool repeat);
	bool EmitKeyDownEventForKey(FKey key, int32 user, bool repeat);
	bool EmitAnalogInputEventForKey(FKey key, float value, int32 user);

	//String Conversion to UE4
	TCHAR* tcharFromStdString(std::string str);
	FRotator convertOrientationToUE(FRotator rawOrientation);

	//Same as blueprint function, internal copy
	FRotator combineRotators(FRotator A, FRotator B);

	FVector convertVectorToUE(FVector rawAcceleration);

	FVector convertAccelerationToBodySpace(FVector armAcceleration, FRotator orientation, FRotator armCorrection, myo::XDirection direction);

	FRotator convertOrientationToArmSpace(FRotator convertedOrientation, FRotator armCorrection, myo::XDirection direction);
}