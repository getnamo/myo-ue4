#pragma once

#include "MyoPluginPrivatePCH.h"
#include "MyoEnum.h"
#include "MyoController.h"
#include "MyoControllerComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMyoControllerSignature, const FMyoControllerData&, Controller);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMyoPoseSignature, const FMyoControllerData&, Controller, TEnumAsByte<EMyoPose>, Pose);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FMyoSyncSignature, const FMyoControllerData&, Controller, TEnumAsByte<EMyoArm>, arm, TEnumAsByte<EMyoArmDirection>, ArmDirection);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FMyoMovedSignature, const FMyoControllerData&, Controller, FVector, ArmAcceleration, FRotator, ArmOrientation, FVector, ArmGyro);

UCLASS(ClassGroup="Input Controller", meta=(BlueprintSpawnableComponent))
class MYOPLUGIN_API UMyoControllerComponent : public UActorComponent //delegate here
{
	GENERATED_UCLASS_BODY()

public:

	/**
	* Event on a Myo pairing
	* @param myo (out) pointer to emitted myo controller class, branch to read other data.
	*/
	UPROPERTY(BlueprintAssignable, Category = "Myo Events")
	FMyoControllerSignature OnPair;

	/**
	* Event on a Myo unpairing
	* @param myo (out) pointer to emitted myo controller class, branch to read other data.
	*/
	UPROPERTY(BlueprintAssignable, Category = "Myo Events")
	FMyoControllerSignature OnUnpair;

	/**
	* Event on a Myo detecting a pose.
	* @param myo (out) pointer to emitted myo controller class, branch to read other data.
	* @param pose (out): EMG pose, refer to Thalmic API rest, fist, waveIn, waveOut, fingersSpread, reserved1, DoubleTap, unknown
	*/
	UPROPERTY(BlueprintAssignable, Category = "Myo Events")
	FMyoPoseSignature OnPoseChanged;

	/**
	*	Event called whenever the myo is connected with arm oriented data
	*/
	UPROPERTY(BlueprintAssignable, Category = "Myo Events")
	FMyoMovedSignature OnArmMoved;

	/**
	* Event on a Myo connecting
	* @param myo (out) pointer to emitted myo controller class, branch to read other data.
	*/
	UPROPERTY(BlueprintAssignable, Category = "Myo Events")
	FMyoControllerSignature OnConnect;

	/**
	* Event on a Myo disconnecting
	* @param myo (out) pointer to emitted myo controller class, branch to read other data.
	*/
	UPROPERTY(BlueprintAssignable, Category = "Myo Events")
	FMyoControllerSignature OnDisconnect;

	/**
	* On arm detection, typically after a wave out gesture
	* @param myo (out) pointer to emitted myo controller class, branch to read other data.
	* @param arm (out) 0 = right, 1 = left, 2 = unknown
	* @param direction (out) 0 = toward wrist, 1 = toward elbow, 2 = unknown
	*/
	UPROPERTY(BlueprintAssignable, Category = "Myo Events")
	FMyoSyncSignature OnArmSync;

	/**
	* Called when a myo has been removed from arm.
	* @param myo (out) pointer to emitted myo controller class, branch to read other data.
	*/
	UPROPERTY(BlueprintAssignable, Category = "Myo Events")
	FMyoControllerSignature OnArmUnsync;

	//Callable Blueprint functions - Need to be defined for direct access
	/**
	* Check if Myo Hub is enabled, if its not it will remain so until restart.
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = MyoFunctions)
	bool IsHubEnabled();

	/**
	* Set the Locking Policy of the Myo Hub see myo documentation for details.
	*/
	UFUNCTION(BlueprintCallable, Category = MyoFunctions)
	void SetLockingPolicy(TEnumAsByte<EMyoLockingPolicy> policy);

	//Convenience Methods - Get
	/**
	*	Obtain controller pointer to current left arm myo if attached, otherwise returns none.
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = MyoFunctions)
	UMyoController* LeftMyo();

	/**
	*	Obtain controller pointer to current right arm myo if attached, otherwise returns none.
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure,  Category = MyoFunctions)
	UMyoController* RightMyo();
	
	/**
	*	Obtain controller pointer to any last paired myo, if you only have one myo and just want the primary myo, use this.
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = MyoFunctions)
	UMyoController* PrimaryMyo();

	/**
	*	Utility function to convert between UE and Myo space, call this on raw values to get them back in Myo space.
	*/
	UFUNCTION(BlueprintCallable, Category = MyoUtilityFunctions)
	void ConvertToMyoOrientationSpace(FRotator orientation, FRotator& converted);

	virtual void OnRegister() override;
	virtual void OnUnregister() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
};