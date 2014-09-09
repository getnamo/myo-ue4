myo-ue4
=======

A Myo Plugin for Unreal Engine 4.

## Quick Setup ##
1. Create new or open a project. 
2. Browse to your project (typically found at *Documents/Unreal Project/{Your Project Root}*)
3. Copy *Plugins* folder into your Project root.
4. Copy *Binaries* folder into your Project root.
5. Restart the Editor and open your project again.
6. Select Window->Plugins. Click on Installed and you should see a category called Input and a plugin called Myo Plugin now available. Select Enabled. The Editor will warn you to restart, click restart.
7. When your project has reloaded, the plugin should be enabled and ready to use. 

(Optional) You can confirm it has successfully loaded by opening the Class Viewer, searching "myo" should show you one actor class added by the plugin called *MyoPluginActor*.

## Usage ##
The plugin is designed with an event driven architecture through a delegate interface. You can access device events through the UE4 Input Mapping system, the convenience Blueprint classes provided or through C++. C++ supports both subclassing of provided example class or by inheriting the MyoDelegate, you can extend your own class to support Myo events. Additionally callable functions support polling for latest data.

### Input Mapping ###

 1.	For a good example start with a template project.
 2.	Use the MyoPluginController or the MyoPluginActor (NB the convenience actor needs to be placed)
 3.	Select Edit->Project Settings.
 4.	Select Engine->Input
 5.	Under Action Mappings and Axis Mappings expand the category you wish to add controller movement to. For example if you want to add Forward motion in the standard 3rd person template, click the + sign in MoveForward.
 6.	Change None to the binding you want and adjust the scale to fit. If for example you wanted this to happen when you pitch your arm you would select Myo Orientation Pitch with a scale of say 3.0 to have snappier controls.
 7.	Play and test your scaling adjust as needed.

(Optional) Use key and axis events in any input derived class blueprint (such as controller). Note note that any events you override will cause Engine->Input mapping to stop working for that bind.

####*Input Axis Events and Buttons Available*####

```
//Poses
FKey MyoPoseRest;
FKey MyoPoseFist;
FKey MyoPoseWaveIn;
FKey MyoPoseWaveOut;
FKey MyoPoseFingersSpread;
FKey MyoPoseThumbToPinky;
FKey MyoPoseUnknown;

//Axis
FKey MyoAccelerationX;
FKey MyoAccelerationY;
FKey MyoAccelerationZ;

FKey MyoOrientationPitch;
FKey MyoOrientationYaw;
FKey MyoOrientationRoll;

FKey MyoGyroX;
FKey MyoGyroY;
FKey MyoGyroZ;
```

###Events through Blueprint###
1. Select Window->Class Viewer.
2. Search for "MyoPluginActor"
3. Right click the actor and Create a new Blueprint e.g. "MyoPluginActorBP"
4. Select Graph in the upper right hand corner and right click in the graph to bring up the function search
5. Typing in "myo" will narrow the events down to plugin related.
6. Add your desired events and linkup to your desired functions 
7. Add the created blueprint to the scene (it's an actor subclass) and hit Play.

e.g. If you want to get the acceleration data from your Myo/s add the Event "On Accelerometer Data". Right click again in an empty space in the BP graph and add a function call to "Print String", connect acceleration to string (a conversion node will automatically be made) and drag exec (the white triangle on node) from the event to the function to connect the calls. 

Compile and Play to see the accelerometer data stream as printed output after the myo/s automatically connect and pair.

####*Blueprint Events Available*####

```
void OnPose(int32 myoId, int32 timestamp, int32 pose);
void OnArmRecognized(int32 myoId, int32 timestamp, int32 arm, int32 direction);
void OnArmLost(int32 myoId, int32 timestamp);
void OnConnect(int32 myoId, int32 timestamp);
void OnDisconnect(int32 myoId, int32 timestamp);
void OnPair(int32 myoId, int32 timestamp);
void OnOrientationData(int32 myoId, int32 timestamp, FRotator orientation);
void OnAccelerometerData(int32 myoId, int32 timestamp, FVector acceleration);
void OnGyroscopeData(int32 myoId, int32 timestamp, FVector gyro);
void DeviceDisabled();
```

####*Blueprint Callable Functions Available*####

```
void VibrateDevice(int32 myoId, int32 type);
bool IsHubEnabled();
void LatestData(int32 myoId, int32& pose, FVector& Acceleration, FRotator& Rotation, FVector& Gyro, int32& Arm);
void WhichArm(int32 myoId, int32& Arm);
void LeftMyoId(bool& available, int32& myoId);
void RightMyoId(bool& available, int32& myoId);
void ConvertToRawOrientation(FRotator orientation, FRotator& converted);
```

###Events through C++###

####*Simple Version*####
1. Extend or Subclass MyoPluginActor and Override functions you wish to subscribe to e.g. 

```virtual void onPose(int32 myoId, uint64 timestamp, int32 pose) OVERRIDE;```


####*Extend your own Class to Receive Events*####
1. include "MyoDelegate.h" and "IMyoPlugin.h" in your implementation
2. Ensure your project has "MyoPlugin" added to your PublicDependencyModuleNames in your *{Project}.build.cs*
3. Make your class inherit from MyoDelegate (multiple inheritence)
4. add ```IMyoPlugin::Get().SetDelegate((MyoDelegate*)this);``` in your BeginPlay() or other form of initialization that comes before the first tick.
5. Make your class tickable (```PrimaryActorTick.bCanEverTick = true;``` in your constructor)
6. add ```IMyoPlugin::Get().MyoTick(DeltaTime);``` inside Tick(float DeltaTime)
7. override any of the delegate methods to receive the events.

To poll for the latest data call MyoGetLatestData(*Myo Device Index*, *Data Pointer*); where *Data Pointer* is a structure in the form of

```
struct MyoDeviceData{
	int pose;			//0 = rest, 1 = fist, 2 = waveIn, 3 = waveOut, 4 = fingersSpread, 5 = reserved1, 6 = thumbToPinky, 7 = unknown (as of beta1)
	FVector acceleration;	//units of g
	FQuat quaternion;	//orientation in quaternion format
	FRotator orientation;	//orientation
	FVector gyro;			//angular speed in deg/s
	int arm;			//which arm it is bound to, 0=right, 1=left, 2=unknown
};
``` 

defined in MyoDelegate.h

##Bugs and Todo##
* Hub runs on the main thread, adds 1ms to render loop. Should be separated into its own thread or reduced to near 0ms.
* Only the first paired is Myo supported for Input Mapping, will need some thought into how to cleanly emit a second Myo.
* Myo does not stop running when stopping PIE, may encounter large number of events when resuming.
* Platforms apart from Windows are untested
* While Plugin does support hot plugging Myos (turning them on/off, running out of range), it does not support hot plugging of the bluetooth hub. This is a limitation of the Myo SDK. If you run your game with the hub disabled (e.g. bluetooth usb unplugged), you will receive the event *MyoDisabled()* and no further notifications. Plugging in the bluetooth hub and restarting the game will re-enable it.


##Credits and Misc##
* Bound to Thalmic Beta-1 SDK
* Plugin by Getnamo, Myo SDK provided by Thalmic Labs