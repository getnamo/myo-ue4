#pragma once

#include "MyoPluginPrivatePCH.h"
#include "MyoEnum.h"
#include "MyoComponent.generated.h"

//TODO: multicast delegates here
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMyoControllerEventSignature, UMyoController*, Controller);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMyoWarmupEventSignature, UMyoController*, Controller, EMyoWarmupResult, Result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FMyoArmEventSignature, UMyoController*, Controller, EMyoArm, Arm, EMyoArmDirection, XDirection);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMyoPoseEventSignature, UMyoController*, Controller, EMyoPose, Pose);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMyoRSSIEventSignature, UMyoController*, Controller, int32, RSSI);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMyoBatteryEventSignature, UMyoController*, Controller, int32, Level);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMyoEmgEventSignature, UMyoController*, Controller, TArray<uint8>, Emg);

UCLASS(ClassGroup="Input Controller", meta=(BlueprintSpawnableComponent))
class MYOPLUGIN_API UMyoComponent : public UActorComponent //delegate here
{
	GENERATED_UCLASS_BODY()

public:

	//Multicast Delegates for events

	UPROPERTY(BlueprintAssignable, Category = MyoEvents)
	FMyoControllerEventSignature OnFrame;

	UPROPERTY(BlueprintAssignable, Category = MyoEvents)
	FMyoControllerEventSignature OnPaired;

	UPROPERTY(BlueprintAssignable, Category = MyoEvents)
	FMyoControllerEventSignature OnUnpaired;

	UPROPERTY(BlueprintAssignable, Category = MyoEvents)
	FMyoControllerEventSignature OnControllerConnected;

	UPROPERTY(BlueprintAssignable, Category = MyoEvents)
	FMyoControllerEventSignature OnControllerDisconnected;

	UPROPERTY(BlueprintAssignable, Category = MyoEvents)
	FMyoArmEventSignature OnArmSync;

	UPROPERTY(BlueprintAssignable, Category = MyoEvents)
	FMyoControllerEventSignature OnArmUnsync;

	UPROPERTY(BlueprintAssignable, Category = MyoEvents)
	FMyoControllerEventSignature OnUnlock;

	UPROPERTY(BlueprintAssignable, Category = MyoEvents)
	FMyoControllerEventSignature OnLock;

	UPROPERTY(BlueprintAssignable, Category = MyoEvents)
	FMyoPoseEventSignature OnPose;

	UPROPERTY(BlueprintAssignable, Category = MyoEvents)
	FMyoRSSIEventSignature OnRssi;

	UPROPERTY(BlueprintAssignable, Category = MyoEvents)
	FMyoBatteryEventSignature OnBatteryLevel;

	UPROPERTY(BlueprintAssignable, Category = MyoEvents)
	FMyoEmgEventSignature OnEmgData;

	UPROPERTY(BlueprintAssignable, Category = MyoEvents)
	FMyoWarmupEventSignature OnWarmupCompleted;

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