#pragma once

class FMyoPlugin : public IMyoPlugin
{
public:
	/** IModuleInterface implementation */
	void StartupModule();
	void ShutdownModule();

	virtual TSharedPtr< class IInputDevice > CreateInputDevice(const TSharedRef< FGenericApplicationMessageHandler >& InMessageHandler) override;
private:
	TSharedPtr< class FMyoInputDevice > MyoInputDevice;
};