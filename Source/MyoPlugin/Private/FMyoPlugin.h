#pragma once

class FMyoPlugin : public IMyoPlugin
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	virtual TSharedPtr<class IInputDevice> CreateInputDevice(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler);
private:
	TSharedPtr< class FMyoInputDevice > MyoInputDevice;

	virtual void AddComponentDelegate(UMyoControllerComponent* DelegateComponent) override;
	virtual void RemoveComponentDelegate(UMyoControllerComponent* DelegateComponent) override;

	//Controlling Myos
	virtual void UnlockMyo(UMyoController* Controller) override;
	virtual void LockMyo(UMyoController* Controller) override;
	virtual void SetLockingPolicy(EMyoLockingPolicy Policy) override;
	virtual void SetStreamEmgType(EMyoStreamEmgType StreamType) override;
	virtual void VibrateMyo(UMyoController* Controller, EMyoVibrationType VibrationType) override;

	TArray<UMyoControllerComponent*> DeferredDelegates;
	bool bInputCreated;
};