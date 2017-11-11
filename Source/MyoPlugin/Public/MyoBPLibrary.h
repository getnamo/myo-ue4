#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "MyoEnum.h"
#include "MyoBPLibrary.generated.h"

/**
* Static Myo utility functions for Blueprint 
*/
UCLASS()
class UMyoBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/** Convert struct to String output */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "To String (MyoEmgData)", BlueprintAutocast), Category = "Utilities|Myo")
	static FString Conv_MyoEmgDataToString(const FMyoEmgData& InData);
};
