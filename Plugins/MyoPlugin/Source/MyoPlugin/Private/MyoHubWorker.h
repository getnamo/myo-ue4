#pragma once
#include <myo/myo.hpp>

/*
Not currently used due to complexity of requiring events to emit on the game thread.
No convenience functions have been found so leaving this for another day.
*/

//~~~~~ Multi Threading ~~~
class FMyoHubWorker : public FRunnable, public myo::DeviceListener
{
protected:
	/** Singleton instance, can access the thread any time via static accessor, if it is active! */
	static  FMyoHubWorker* Runnable;

	/** Thread to run the worker FRunnable on */
	FRunnableThread* Thread;

	myo::Hub* hub;

public:
	bool stopCalled;

	//~~~ Thread Core Functions ~~~

	//Constructor / Destructor
	FMyoHubWorker(myo::Hub* newHub);
	virtual ~FMyoHubWorker();

	// Begin FRunnable interface.
	virtual bool Init();
	virtual uint32 Run();
	virtual void Stop();
	// End FRunnable interface

	//~~~ Starting and Stopping Thread ~~~

	/*
	Start the thread and the worker from static
	*/
	static FMyoHubWorker* HubInit(myo::Hub* newHub);
	static bool ThreadRunning();

	/** Shuts down the thread. Static so it can easily be called from outside the thread context */
	static void Shutdown();
};