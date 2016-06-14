#include "MyoPluginPrivatePCH.h"
#include "FMyoInputDevice.h"
#include "MyoInterface.h"
#include "MyoUtility.h"
//#include "MyoDataCollector.h"

#include "IMyoPlugin.h"
#include "FMyoPlugin.h"

#define LOCTEXT_NAMESPACE "MyoPlugin"
#define PLUGIN_VERSION "0.8.0"

DEFINE_LOG_CATEGORY_STATIC(MyoPluginLog, Log, All);


//using namespace MyoUtility;

//Private API - This is where the magic happens

//Init and Runtime
void FMyoPlugin::StartupModule()
{
	UE_LOG(MyoPluginLog, Log, TEXT("Using Myo Plugin version %s"), TEXT(PLUGIN_VERSION));

	// Instantiate the PrintMyoEvents class we defined above, and attach it as a listener to our Hub.
	//collector = new MyoDataCollector;

	//Register all input mapping keys and axes
	EKeys::AddKey(FKeyDetails(EKeysMyo::MyoPoseRest, LOCTEXT("MyoPoseRest", "Myo Pose Rest"), FKeyDetails::GamepadKey));
	EKeys::AddKey(FKeyDetails(EKeysMyo::MyoPoseFist, LOCTEXT("MyoPoseFist", "Myo Pose Fist"), FKeyDetails::GamepadKey));
	EKeys::AddKey(FKeyDetails(EKeysMyo::MyoPoseWaveIn, LOCTEXT("MyoPoseWaveIn", "Myo Pose Wave In"), FKeyDetails::GamepadKey));
	EKeys::AddKey(FKeyDetails(EKeysMyo::MyoPoseWaveOut, LOCTEXT("MyoPoseWaveOut", "Myo Pose Wave Out"), FKeyDetails::GamepadKey));
	EKeys::AddKey(FKeyDetails(EKeysMyo::MyoPoseFingersSpread, LOCTEXT("MyoPoseFingersSpread", "Myo Pose FingersSpread"), FKeyDetails::GamepadKey));
	EKeys::AddKey(FKeyDetails(EKeysMyo::MyoPoseDoubleTap, LOCTEXT("MyoPoseDoubleTap", "Myo Pose Double Tap"), FKeyDetails::GamepadKey));
	EKeys::AddKey(FKeyDetails(EKeysMyo::MyoPoseUnknown, LOCTEXT("MyoPoseUnknown", "Myo Pose Unknown"), FKeyDetails::GamepadKey));

	EKeys::AddKey(FKeyDetails(EKeysMyo::MyoAccelerationX, LOCTEXT("MyoAccelerationX", "Myo Acceleration X"), FKeyDetails::FloatAxis));
	EKeys::AddKey(FKeyDetails(EKeysMyo::MyoAccelerationY, LOCTEXT("MyoAccelerationY", "Myo Acceleration Y"), FKeyDetails::FloatAxis));
	EKeys::AddKey(FKeyDetails(EKeysMyo::MyoAccelerationZ, LOCTEXT("MyoAccelerationZ", "Myo Acceleration Z"), FKeyDetails::FloatAxis));

	EKeys::AddKey(FKeyDetails(EKeysMyo::MyoOrientationPitch, LOCTEXT("MyoOrientationPitch", "Myo Orientation Pitch"), FKeyDetails::FloatAxis));
	EKeys::AddKey(FKeyDetails(EKeysMyo::MyoOrientationYaw, LOCTEXT("MyoOrientationYaw", "Myo Orientation Yaw"), FKeyDetails::FloatAxis));
	EKeys::AddKey(FKeyDetails(EKeysMyo::MyoOrientationRoll, LOCTEXT("MyoOrientationRoll", "Myo Orientation Roll"), FKeyDetails::FloatAxis));

	EKeys::AddKey(FKeyDetails(EKeysMyo::MyoGyroX, LOCTEXT("MyoGyroX", "Myo Gyro X"), FKeyDetails::FloatAxis));
	EKeys::AddKey(FKeyDetails(EKeysMyo::MyoGyroY, LOCTEXT("MyoGyroY", "Myo Gyro Y"), FKeyDetails::FloatAxis));
	EKeys::AddKey(FKeyDetails(EKeysMyo::MyoGyroZ, LOCTEXT("MyoGyroZ", "Myo Gyro Z"), FKeyDetails::FloatAxis));
}

void FMyoPlugin::ShutdownModule()
{
	//collector->myoInterface = NULL;
	
	//collector->ShutDown();
	//delete collector;
}


TSharedPtr< class IInputDevice > FMyoPlugin::CreateInputDevice(const TSharedRef< FGenericApplicationMessageHandler >& InMessageHandler)
{
	FMyoPlugin::MyoInputDevice = MakeShareable(new FMyoInputDevice(InMessageHandler));
	return TSharedPtr< class IInputDevice >(MyoInputDevice);
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMyoPlugin, MyoPlugin)