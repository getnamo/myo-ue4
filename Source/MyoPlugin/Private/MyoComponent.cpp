#include "MyoPluginPrivatePCH.h"
#include "MyoComponent.h"
#include "MyoInterface.h"
#include "Engine.h"
#include "CoreUObject.h"

UMyoControllerComponent::UMyoControllerComponent(const FObjectInitializer &init) : UActorComponent(init)
{
	bWantsInitializeComponent = true;
	bAutoActivate = true;
	PrimaryComponentTick.bCanEverTick = true;	//the component automatically ticks so we don't have to
}

void UMyoControllerComponent::OnRegister()
{
	Super::OnRegister();

	//Attach the delegate pointer automatically to the owner of the component
	//ValidSelfPointer = this;
	//SetInterfaceDelegate(GetOwner());
	//MyoStartup();
}

void UMyoControllerComponent::OnUnregister()
{
	Super::OnUnregister();
	//MyoShutdown();
}

void UMyoControllerComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
	FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//Forward the component tick
	//MyoTick(DeltaTime);
}

//Functions forwards, required implementations
bool UMyoControllerComponent::IsHubEnabled()
{
	return false; //MyoIsHubEnabled();
}

void UMyoControllerComponent::SetLockingPolicy(TEnumAsByte<EMyoLockingPolicy> policy)
{
	return; // MyoSetLockingPolicy(policy);
}

UMyoController* UMyoControllerComponent::LeftMyo()
{
	return nullptr; // MyoLeftMyo();
}

UMyoController* UMyoControllerComponent::RightMyo()
{
	return nullptr; // MyoRightMyo();
}

UMyoController* UMyoControllerComponent::PrimaryMyo()
{
	return nullptr;// MyoPrimaryMyo();
}

void UMyoControllerComponent::ConvertToMyoOrientationSpace(FRotator orientation, FRotator& converted)
{
	return; // MyoConvertToMyoOrientationSpace(orientation, converted);
}


