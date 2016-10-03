#include "MyoPluginPrivatePCH.h"
#include "MyoComponent.h"
#include "IMyoPlugin.h"

UMyoComponent::UMyoComponent(const FObjectInitializer &init) : UActorComponent(init)
{
	bWantsInitializeComponent = true;
	bAutoActivate = true;
	PrimaryComponentTick.bCanEverTick = true;	//the component automatically ticks so we don't have to
}

void UMyoComponent::OnRegister()
{
	Super::OnRegister();
	IMyoPlugin::Get().AddComponentDelegate(this);
}

void UMyoComponent::OnUnregister()
{
	IMyoPlugin::Get().RemoveComponentDelegate(this);
	Super::OnUnregister();
}


//Functions forwards, required implementations
bool UMyoComponent::IsHubEnabled()
{
	return IMyoPlugin::Get().IsHubEnabled();
}

void UMyoComponent::SetLockingPolicy(TEnumAsByte<EMyoLockingPolicy> Policy)
{
	return IMyoPlugin::Get().SetLockingPolicy(Policy); 
}

UMyoController* UMyoComponent::LeftMyo()
{
	return nullptr; // MyoLeftMyo();
}

UMyoController* UMyoComponent::RightMyo()
{
	return nullptr; // MyoRightMyo();
}

UMyoController* UMyoComponent::PrimaryMyo()
{
	return nullptr;// MyoPrimaryMyo();
}

void UMyoComponent::ConvertToMyoOrientationSpace(FRotator orientation, FRotator& converted)
{
	return; // MyoConvertToMyoOrientationSpace(orientation, converted);
}


