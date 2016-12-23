#pragma once

class FMyoPlugin : public IMyoPlugin
{
public:
	/** IModuleInterface implementation */
	void StartupModule();
	void ShutdownModule();

	virtual void AddComponentDelegate(UMyoComponent* Component) override;
	virtual void RemoveComponentDelegate(UMyoComponent* Component) override;

	virtual TSharedPtr< class IInputDevice > CreateInputDevice(const TSharedRef< FGenericApplicationMessageHandler >& InMessageHandler) override;
private:
	TSharedPtr< class FMyoInputDevice > MyoInputDevice;

	virtual void AddComponentDelegate(UMyoControllerComponent* DelegateComponent) override;
	virtual void RemoveComponentDelegate(UMyoControllerComponent* DelegateComponent) override;

	//Controlling myos
	virtual void UnlockMyo(UMyoController* Controller) override;;
	virtual void LockMyo(UMyoController* Controller) override;;
	virtual void SetLockingPolicy(EMyoLockingPolicy Policy) override;;
	virtual void SetStreamEmgType(EMyoStreamEmgType StreamType) override;;
	virtual void VibrateMyo(UMyoController* Controller, EMyoVibrationType VibrationType) override;;

	TArray<UMyoComponent*> DeferredDelegates;
	bool bInputCreated;
};