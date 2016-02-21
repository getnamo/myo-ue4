#pragma once

class DataCollector;
class MyoDelegate;

class MYOPLUGIN_API FMyoPlugin : public IMyoPlugin
{
public:
	/** IModuleInterface implementation */
	void StartupModule();
	void ShutdownModule();

	/** Delegate Method To subscribe to event calls, only supports one listener for now */
	void SetDelegate(MyoDelegate* newDelegate);
	void RemoveDelegate();

	/** Manual looping, currently called in main thread */
	void MyoTick(float DeltaTime);

	/** Optional Public API For direct module bind */
	void VibrateDevice(int deviceId, int vibrationType);
	MyoDeviceData* LatestData(int deviceId);
	void WhichArm(int deviceId, int& arm);
	void LeftMyoId(bool& available, int& deviceId);
	void RightMyoId(bool& available, int& deviceId);
	void PrimaryMyoId(bool& available, int& deviceId);
	bool IsHubEnabled();
	void CalibrateOrientation(int deviceId, FRotator direction);	//adjusts arm and body input relative to screen forward after this is called.
	
	//Utility
	void MaxMyoId(int& maxId);
	bool IsValidDeviceId(int myoId);
	void SetLockingPolicy(MyoLockingPolicy policy);
	void SetStreamEmg(int deviceId, MyoStreamEmgType type);
	void UnlockMyo(int deviceId, MyoUnlockType type);
	void LockMyo(int deviceId);
private:
	DataCollector *collector;
};