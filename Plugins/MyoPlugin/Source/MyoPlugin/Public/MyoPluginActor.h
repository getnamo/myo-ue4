#pragma once

#include "GameFramework/Actor.h"
#include "MyoDelegateBlueprint.h"
#include "MyoPluginActor.generated.h"


/**
 * Placeable Actor that receives Myo input and pose updates. 
 * Use or Extend to receive event notifications.
 */
UCLASS()
class AMyoPluginActor : public AActor, public MyoDelegateBlueprint, public IMyoInterface
{
	GENERATED_UCLASS_BODY()


	//Callable Blueprint functions - Need to be defined for direct access
	/**
	* Check if Myo Hub is enabled, if its not it will remain so until restart.
	*/
	UFUNCTION(BlueprintCallable, Category = MyoFunctions)
	bool IsHubEnabled();

	/**
	* Set the Locking Policy of the Myo Hub see myo documentation for details.
	*/
	UFUNCTION(BlueprintCallable, Category = MyoFunctions)
	void SetLockingPolicy(enum MyoLockingPolicy policy);

	//Todo: add historical support later and latest if in demand

	//Convenience Methods - Get
	/**
	*	Obtain controller pointer to current left arm myo if attached, otherwise returns none.
	*/
	UFUNCTION(BlueprintCallable, Category = MyoFunctions)
	UMyoController* LeftMyo();

	/**
	*	Obtain controller pointer to current right arm myo if attached, otherwise returns none.
	*/
	UFUNCTION(BlueprintCallable, Category = MyoFunctions)
	UMyoController* RightMyo();

	/**
	*	Obtain controller pointer to any last paired myo, if you only have one myo and just want the primary myo, use this.
	*/
	UFUNCTION(BlueprintCallable, Category = MyoFunctions)
	UMyoController* PrimaryMyo();

	/**
	*	Utility function to convert between UE and Myo space, call this on raw values to get them back in Myo space.
	*/
	UFUNCTION(BlueprintCallable, Category = MyoUtilityFunctions)
	void ConvertToMyoOrientationSpace(FRotator orientation, FRotator& converted);


	//Required for plugin startup, end, and forwarding Tick to the Myo Delegate.
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaTime) override;
};


