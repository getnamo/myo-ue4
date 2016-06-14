#include "MyoPluginPrivatePCH.h"
#include "MyoUtility.h"
#include "Myo.h"

namespace MyoUtility 
{

	bool EmitKeyUpEventForKey(FKey key, int32 user, bool repeat)
	{
		FKeyEvent KeyEvent(key, FSlateApplication::Get().GetModifierKeys(), user, repeat, 0, 0);
		return FSlateApplication::Get().ProcessKeyUpEvent(KeyEvent);
	}

	bool EmitKeyDownEventForKey(FKey key, int32 user, bool repeat)
	{
		FKeyEvent KeyEvent(key, FSlateApplication::Get().GetModifierKeys(), user, repeat, 0, 0);
		return FSlateApplication::Get().ProcessKeyDownEvent(KeyEvent);
	}

	bool EmitAnalogInputEventForKey(FKey key, float value, int32 user)
	{
		FAnalogInputEvent AnalogInputEvent(key, FSlateApplication::Get().GetModifierKeys(), user, false, 0, 0, value);
		return FSlateApplication::Get().ProcessAnalogInputEvent(AnalogInputEvent);
	}

	//String Conversion to UE4
	TCHAR* tcharFromStdString(std::string str) {
		TCHAR *param = new TCHAR[str.size() + 1];
		param[str.size()] = 0;
		std::copy(str.begin(), str.end(), param);
		return param;
	}
	FRotator convertOrientationToUE(FRotator rawOrientation)
	{
		FRotator convertedOrientation;
		convertedOrientation.Pitch = rawOrientation.Pitch*-1.f;
		convertedOrientation.Yaw = rawOrientation.Yaw*-1.f;
		convertedOrientation.Roll = rawOrientation.Roll;
		return convertedOrientation;

		//debug return raw orientation
		//return rawOrientation;
	}

	//Same as blueprint function, internal copy
	FRotator combineRotators(FRotator A, FRotator B)
	{
		FQuat AQuat = FQuat(A);
		FQuat BQuat = FQuat(B);

		return FRotator(BQuat*AQuat);
	}

	FVector convertVectorToUE(FVector rawAcceleration)
	{
		return FVector(rawAcceleration.X, -rawAcceleration.Y, -rawAcceleration.Z);
	}

	FVector convertAccelerationToBodySpace(FVector armAcceleration, FRotator orientation, FRotator armCorrection, myo::XDirection direction)
	{
		float directionModifier = 1.f;
		//something wrong here, also make sure this is applied to the arm space correction as well
		if (direction == myo::xDirectionTowardElbow) {
			directionModifier = -1.f;
		}

		//Compensate for arm roll
		FRotator armYawCorrection = FRotator(0, armCorrection.Yaw, 0);

		FRotator fullCompensationOrientation = combineRotators(orientation, armYawCorrection);

		FVector reactionAcceleration = fullCompensationOrientation.RotateVector(armAcceleration);

		//Subtract gravity
		return ((reactionAcceleration * FVector(directionModifier, directionModifier, 1.f)) + FVector(0, 0, 1)) * -1.f;
	}

	FRotator convertOrientationToArmSpace(FRotator convertedOrientation, FRotator armCorrection, myo::XDirection direction)
	{
		float directionModifier = 1.f;
		//Check for arm direction, compensate if needed by reversing pitch and roll
		if (direction == myo::xDirectionTowardElbow) {
			directionModifier = -1.f;
			convertedOrientation = FRotator(convertedOrientation.Pitch*directionModifier, convertedOrientation.Yaw, convertedOrientation.Roll*directionModifier);
		}

		//Compensate Roll (pre)
		FRotator tempRot = combineRotators(FRotator(0, 0, armCorrection.Roll*directionModifier), convertedOrientation);

		//Compensate for Yaw (post) and return the result
		return combineRotators(tempRot, FRotator(0, armCorrection.Yaw, 0));
	}

}