// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ModuleManager.h"
#include "MyoControllerComponent.h"
#include "MyoEnum.h"
#include "IInputDeviceModule.h"

/**
 * The public interface to this module
 */
class MYOPLUGIN_API IMyoPlugin : public IInputDeviceModule
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
	 * Public API
	 */

	//Component delegate tracking
	virtual void AddComponentDelegate(UMyoControllerComponent* DelegateComponent) {};
	virtual void RemoveComponentDelegate(UMyoControllerComponent* DelegateComponent) {};

	//Controlling myos
	virtual void UnlockMyo(UMyoController* Controller) {};
	virtual void LockMyo(UMyoController* Controller) {};
	virtual void SetLockingPolicy(EMyoLockingPolicy Policy) {};
	virtual void SetStreamEmgType(EMyoStreamEmgType StreamType) {};
	virtual void VibrateMyo(UMyoController* Controller, EMyoVibrationType VibrationType) {};

	virtual bool IsHubEnabled() { return false; }
	virtual void CalibrateOrientation(int32 MyoId, FRotator Direction) {};
};

