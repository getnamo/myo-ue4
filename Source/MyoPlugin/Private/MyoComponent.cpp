#include "MyoPluginPrivatePCH.h"
#include "MyoComponent.h"
#include "MyoInterface.h"
#include "Engine.h"
#include "CoreUObject.h"

UMyoComponent::UMyoComponent(const FObjectInitializer &init) : UActorComponent(init)
{
	bWantsInitializeComponent = true;
	bAutoActivate = true;
	PrimaryComponentTick.bCanEverTick = true;	//the component automatically ticks so we don't have to
}

void UMyoComponent::OnRegister()
{
	Super::OnRegister();

	//Attach the delegate pointer automatically to the owner of the component
	//ValidSelfPointer = this;
	//SetInterfaceDelegate(GetOwner());
	//MyoStartup();
}

void UMyoComponent::OnUnregister()
{
	Super::OnUnregister();
	//MyoShutdown();
}

void UMyoComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
	FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//Forward the component tick
	//MyoTick(DeltaTime);
}

//Functions forwards, required implementations
bool UMyoComponent::IsHubEnabled()
{
	return false; //MyoIsHubEnabled();
}

void UMyoComponent::SetLockingPolicy(TEnumAsByte<EMyoLockingPolicy> policy)
{
	return; // MyoSetLockingPolicy(policy);
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


