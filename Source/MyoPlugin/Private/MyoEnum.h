#pragma once

#include "CoreUObject.h"

UENUM(BlueprintType)
enum EMyoArm
{
	MYO_ARM_RIGHT = 0,
	MYO_ARM_LEFT,
	MYO_ARM_UNKNOWN
};

UENUM(BlueprintType)
enum EMyoArmDirection
{
	MYO_TOWARD_WRIST = 0,
	MYO_TOWARD_ELBOW,
	MYO_DIRECTION_UNKNOWN
};

UENUM(BlueprintType)
enum EMyoWarmupState
{
	MYO_WARMUP_STATE_UNKNOWN = 0,
	MYO_WARMUP_STATE_COLD,
	MYO_WARMUP_STATE_WARM
};

UENUM(BlueprintType)
enum EMyoWarmupResult
{
	MYO_WARMUP_RESULT_UNKNOWN = 0,
	MYO_WARMUP_RESULT_SUCCESS,
	MYO_WARMUP_RESULT_FAILED_TIMEOUT
};

UENUM(BlueprintType)
enum EMyoPose
{
	MYO_POSE_REST = 0,
	MYO_POSE_FIST,
	MYO_POSE_WAVEIN,
	MYO_POSE_WAVEOUT,
	MYO_POSE_FINGERSPREAD,
	MYO_POSE_DOUBLETAP,
	MYO_POSE_MAX = 255
};

UENUM(BlueprintType)
enum EMyoVibrationType
{
	MYO_VIBRATION_SHORT = 0,
	MYO_VIBRATION_MEDIUM,
	MYO_VIBRATION_LONG
};

UENUM(BlueprintType)
enum EMyoLockingPolicy
{
	MYO_LOCKING_POLICY_NONE,
	MYO_LOCKING_POLICY_STANDARD
};

UENUM(BlueprintType)
enum EMyoUnlockType
{
	MYO_UNLOCK_TIMED,
	MYO_UNLOCK_HOLD
};

UENUM(BlueprintType)
enum EMyoStreamEmgType
{
	MYO_STREAM_EMG_DISABLED = 0,
	MYO_STREAM_EMG_ENABLED
};

USTRUCT()
struct FMyoFirmwareVersion
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadOnly, Category = MyoFirmwareVersion)
	int32 VersionMajor;

	UPROPERTY(BlueprintReadOnly, Category = MyoFirmwareVersion)
	int32 VersionMinor;

	UPROPERTY(BlueprintReadOnly, Category = MyoFirmwareVersion)
	int32 VersionPatch;

	UPROPERTY(BlueprintReadOnly, Category = MyoFirmwareVersion)
	int32 VersionHardwareRev;
};


//USTRUCT()
struct FMyoEmgData
{
	//GENERATED_BODY()

	//UPROPERTY()
	//BlueprintReadWrite, Category = "Emg Data Struct")
	//TArray<int32> streams;
};

/**
* Converted Controller Data.
*/
struct FMyoDeviceData {

	//Sample TimeStamp
	uint64 TimeStamp;

	//9-Axis Data
	FVector Acceleration;	//units of g
	FQuat Quaternion;		//orientation in quaternion format
	FRotator Orientation;	//orientation
	FVector Gyro;			//angular speed in deg/s

	//Arm specifics
	EMyoArm Arm;
	EMyoArmDirection ArmDirection;

	//Plugin Derived - Values given arm space after arm calibration, otherwise same as raw
	FVector ArmAcceleration;	//units of g
	FRotator ArmOrientation;	//orientation
	FVector ArmGyro;			//angular speed in deg/s
	FRotator ArmSpaceCorrection;	//used to calibrate the orientation, not exposed to blueprints

									    //Body space, useful for easy component space integration
	FVector BodySpaceNullAcceleration;	//units of g, in calibrated space, without gravity component

	//Streaming Data
	EMyoStreamEmgType SteamType;

	//Pose Data
	EMyoPose Pose;

	bool Valid;
	bool IsLocked;
};

//Input Mapping Key Structure
struct EKeysMyo
{
	//Possess
	static const FKey MyoPoseRest;
	static const FKey MyoPoseFist;
	static const FKey MyoPoseWaveIn;
	static const FKey MyoPoseWaveOut;
	static const FKey MyoPoseFingersSpread;
	static const FKey MyoPoseDoubleTap;
	static const FKey MyoPoseUnknown;

	//Axis, given in arm orientation by default (use calibration or use raw functions to specify)
	static const FKey MyoAccelerationX;
	static const FKey MyoAccelerationY;
	static const FKey MyoAccelerationZ;

	static const FKey MyoOrientationPitch;
	static const FKey MyoOrientationYaw;
	static const FKey MyoOrientationRoll;

	static const FKey MyoGyroX;
	static const FKey MyoGyroY;
	static const FKey MyoGyroZ;
};