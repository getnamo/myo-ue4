#pragma once

class DataCollector;
class MyoDelegate;

class FMyoPlugin : public IMyoPlugin
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
	void LatestData(int deviceId, MyoDeviceData& data);
	void WhichArm(int deviceId, int& arm);
	void LeftMyoId(bool& available, int& deviceId);
	void RightMyoId(bool& available, int& deviceId);
	bool IsHubEnabled();
	void CalibrateOrientation(int deviceId);	//adjusts all input to arm orientation after this is called.
	
	//Internal Utility
	bool isValidDeviceId(int deviceId);

private:
	DataCollector *collector;
};