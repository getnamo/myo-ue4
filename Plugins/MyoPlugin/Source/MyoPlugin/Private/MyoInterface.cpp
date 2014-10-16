#pragma once

#include "MyoPluginPrivatePCH.h"
#include "MyoInterface.h"

UMyoInterface::UMyoInterface(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{

}

//This is required for compiling, would also let you know if somehow you called
//the base event/function rather than the over-rided version
FString IMyoInterface::ToString()
{
	return "IMyoInterface::ToString()";
}
