#include "MyoPluginPrivatePCH.h"
#include "FMyoInputDevice.h"
#include "MyoUtility.h"
#include <myo/myo.hpp>

FMyoInputDevice::FMyoInputDevice(const TSharedRef< FGenericApplicationMessageHandler >& InMessageHandler) : MessageHandler(InMessageHandler)
{

}

void FMyoInputDevice::Tick(float DeltaTime)
{
	ParseEvents();
}

void FMyoInputDevice::SendControllerEvents()
{

}

void FMyoInputDevice::ParseEvents()
{

}

void FMyoInputDevice::SetMessageHandler(const TSharedRef< FGenericApplicationMessageHandler >& InMessageHandler)
{
	MessageHandler = InMessageHandler;
}

bool FMyoInputDevice::Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar)
{
	return false;
}

void FMyoInputDevice::SetChannelValue(int32 ControllerId, FForceFeedbackChannelType ChannelType, float Value)
{

}

void FMyoInputDevice::SetChannelValues(int32 ControllerId, const FForceFeedbackValues &values)
{

}

FMyoInputDevice::~FMyoInputDevice()
{

}
