#pragma once
#include "MyoPluginPrivatePCH.h"
#include "MyoControllerData.generated.h"

USTRUCT()
struct FMyoControllerData
{
	GENERATED_USTRUCT_BODY()

	//Can be rest, fist, waveIn, waveOut, fingersSpread, reserved1, DoubleTap, unknown
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Myo Frame")
	TEnumAsByte<EMyoPose> Pose;

	//acceleration in units of g
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Myo Frame")
	FVector Acceleration;

	//orientation in rotator format
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Myo Frame")
	FRotator Orientation;

	// gyroscope vector in deg / s
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Myo Frame")
	FVector Gyro;

	//right, left, unknown
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Myo Frame")
	TEnumAsByte<EMyoArm> Arm;

	//toward wrist, toward elbow, unknown
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Myo Frame")
	TEnumAsByte<EMyoArmDirection> ArmDirection;

	//acceleration in arm space given in units of g
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Myo Frame")
	FVector ArmAcceleration;

	//orientation in arm space
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Myo Frame")
	FRotator ArmOrientation;

	//gyro in arm space
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Myo Frame")
	FVector ArmGyro;

	//the correction rotation to transform raw into arm space
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Myo Frame")
	FRotator ArmSpaceCorrection;

	//BodySpaceAcceleration(out) acceleration in calibrated body space, with gravity removed. (This value is used for velocity and position integration)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Myo Frame")
	FVector BodySpaceNullAcceleration;

	//Id identifying myo, used for calling functions
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Myo Frame")
	int32 MyoId;

	/** Seconds since start of device, this is independent of tick deltas*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MyoControllerData)
	float TimeStamp;
};