#pragma once
#include "MyoUtility.h"

class MyoDataCollector : public myo::DeviceListener {
public:
	MyoDataCollector();
	~MyoDataCollector();

	// We store each Myo pointer that we pair with in this list, so that we can keep track of the order we've seen
	// each Myo and give it a unique short identifier (see onPair() and identifyMyo() above).
	std::vector<myo::Myo*> knownMyos;
	int leftMyo;
	int rightMyo;
	myo::Myo* lastPairedMyo;
	TArray<UMyoController*> controllers;

	std::vector<FMyoDeviceData>  m_data;	//up to n devices supported
	IMyoInterface* myoInterface;
	bool Enabled;
	bool Listening;
	myo::Hub::LockingPolicy lockingPolicy;
	myo::Pose lastPose;
	FRotator armSpaceCorrection;	//used to hold on to the correction throughout hub destroying
	bool correctionAvailable = false;

	myo::Hub *hub;
}