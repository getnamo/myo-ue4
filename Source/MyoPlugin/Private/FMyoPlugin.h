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
	TArray<UMyoComponent*> DeferredDelegates;
	bool bInputCreated;
};