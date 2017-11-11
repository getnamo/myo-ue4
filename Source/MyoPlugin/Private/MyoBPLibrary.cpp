#include "MyoPluginPrivatePCH.h"
#include "MyoBPLibrary.h"

FString UMyoBPLibrary::Conv_MyoEmgDataToString(const FMyoEmgData& InData)
{
	FString ResultString = TEXT("[");
	auto& Streams = InData.Streams;
	for (int i=0; i< Streams.Num(); i++)
	{
		if (i == Streams.Num() - 1)
		{
			ResultString.Append(FString::Printf(TEXT("%d"), Streams[i]));
		}
		else
		{
			ResultString.Append(FString::Printf(TEXT("%d, "), Streams[i]));
		}
	}
	ResultString.Append(TEXT("]"));
	return ResultString;
}

