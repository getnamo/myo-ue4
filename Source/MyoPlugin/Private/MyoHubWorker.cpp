#include "MyoPluginPrivatePCH.h"
#include "MyoHubWorker.h"
#include <myo/myo.hpp>

//***********************************************************
//Thread Worker Starts as NULL, prior to being instanced
//		This line is essential! Compiler error without it
FMyoHubWorker* FMyoHubWorker::Runnable = NULL;
//***********************************************************

FMyoHubWorker::FMyoHubWorker(myo::Hub* newHub)
{
	stopCalled = false;
	hub = newHub;

	Thread = FRunnableThread::Create(this, TEXT("FMyoHubWorker"), 0, TPri_Normal); //windows default = 8mb for thread, could specify more
}

FMyoHubWorker::~FMyoHubWorker()
{
	stopCalled = true;
	delete Thread;
	Thread = NULL;
}

//MyoPosing, these aren't getting called even if we're added as a listener
void onPose(myo::Myo* myo, uint64_t timestamp, myo::Pose pose){
	UE_LOG(LogClass, Log, TEXT("Posing!"));
}

void onAccelerometerData(myo::Myo *myo, uint64_t timestamp, const myo::Vector3< float > &accel){
	UE_LOG(LogClass, Log, TEXT("Accelerating!"));
}

//Init
bool FMyoHubWorker::Init()
{
	UE_LOG(LogClass, Log, TEXT("(FMyoHubWorker)Init"));
	hub->addListener(this);
	return true;
}

//Run
uint32 FMyoHubWorker::Run()
{
	UE_LOG(LogClass, Log, TEXT("(FMyoHubWorker)Run"));
	//Initial wait before starting
	//FPlatformProcess::Sleep(0.5);
	
	//do work here
	while (!stopCalled && hub != NULL){
		hub->run(10);	//run @ 100hz
		//To be continued, this will cause the engine to fail since 
		//we're calling not thread-safe functions through the myo listeners. Will need
		//a way to specify lambda calls on the game thread for this to work.
		//Something akin to RunOnUIThread() found in android
		//Leaving this in for another day

		//auto func = []() { UE_LOG(LogClass, Log, TEXT("Lambda call:")); };

		//UE_LOG(LogClass, Log, TEXT("(FMyoHubWorker)Tick"));
	}
	
	return 0;
}

//stop
void FMyoHubWorker::Stop()
{
	UE_LOG(LogClass, Log, TEXT("(FMyoHubWorker)Stop"));
	hub->removeListener(this);
	stopCalled = true;
}

FMyoHubWorker* FMyoHubWorker::HubInit(myo::Hub* newHub)
{
	//Create new instance of thread if it does not exist
	//		and the platform supports multi threading!
	if (Runnable){
		delete Runnable;
		Runnable = NULL;
	}
	if (!Runnable && FPlatformProcess::SupportsMultithreading())
	{
		Runnable = new FMyoHubWorker(newHub);
	}
	UE_LOG(LogClass, Log, TEXT("Supports MT: %d"), FPlatformProcess::SupportsMultithreading());

	return Runnable;
}

bool FMyoHubWorker::ThreadRunning(){
	if (Runnable){
		return (!Runnable->stopCalled);
	}
	else{
		return false;
	}
}

void FMyoHubWorker::Shutdown()
{
	if (Runnable)
	{
		delete Runnable;
		Runnable = NULL;
	}
}