// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ModuleManager.h"

class MyoDelegate;
struct MyoDeviceData;

/**
 * The public interface to this module
 */
class IMyoPlugin : public IModuleInterface
{

public:

	/**
	 * Singleton-like access to this module's interface.  This is just for convenience!
	 * Beware of calling this during the shutdown phase, though.  Your module might have been unloaded already.
	 *
	 * @return Returns singleton instance, loading the module on demand if needed
	 */
	static inline IMyoPlugin& Get()
	{
		return FModuleManager::LoadModuleChecked< IMyoPlugin >( "MyoPlugin" );
	}

	/**
	 * Checks to see if this module is loaded and ready.  It is only valid to call Get() if IsAvailable() returns true.
	 *
	 * @return True if the module is loaded and ready to use
	 */
	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded( "MyoPlugin" );
	}

	/**
	 * Public API, implemented in FMyoPlugin.cpp
	 * Optional way of getting data is to subscribe to the MyoDelegate class through inheritance
	 */
	virtual void SetDelegate(MyoDelegate* newDelegate) {};
	virtual void RemoveDelegate(){};
	virtual void MyoTick(float DeltaTime) {};

	virtual void VibrateDevice(int deviceId, int vibrationType) {};
	virtual void LatestData(int deviceId, MyoDeviceData& data) {};
	virtual void WhichArm(int deviceId, int& arm){};
	virtual void LeftMyoId(bool& available, int& deviceId){};
	virtual void RightMyoId(bool& available, int& deviceId){};
	virtual bool IsHubEnabled() = 0;
	virtual void CalibrateOrientation(int deviceId, FRotator direction){};
};

