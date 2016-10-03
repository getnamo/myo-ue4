#pragma once

#include "MyoPluginPrivatePCH.h"
#include "MyoLambdaRunnable.h"

uint64 MyoLambdaRunnable::ThreadNumber = 0;

MyoLambdaRunnable::MyoLambdaRunnable(TFunction< void()> InFunction)
{
	FunctionPointer = InFunction;
	Finished = false;
	Number = ThreadNumber;

	FString threadStatGroup = FString::Printf(TEXT("MyoLambdaRunnable%d"), ThreadNumber);
	Thread = NULL;
	Thread = FRunnableThread::Create(this, *threadStatGroup, 0, TPri_BelowNormal); //windows default = 8mb for thread, could specify more
	ThreadNumber++;

	//Runnables.Add(this);
}

MyoLambdaRunnable::~MyoLambdaRunnable()
{
	if (Thread == NULL)
	{
		delete Thread;
		Thread = NULL;
	}

	//Runnables.Remove(this);
}

//Init
bool MyoLambdaRunnable::Init()
{
	//UE_LOG(LogClass, Log, TEXT("FLambdaRunnable %d Init"), Number);
	return true;
}

//Run
uint32 MyoLambdaRunnable::Run()
{
	if (FunctionPointer)
		FunctionPointer();

	//UE_LOG(LogClass, Log, TEXT("FLambdaRunnable %d Run complete"), Number);
	return 0;
}

//stop
void MyoLambdaRunnable::Stop()
{
	Finished = true;
}

void MyoLambdaRunnable::Exit()
{
	Finished = true;
	//UE_LOG(LogClass, Log, TEXT("FLambdaRunnable %d Exit"), Number);

	//delete ourselves when we're done
	delete this;
}

void MyoLambdaRunnable::EnsureCompletion()
{
	Stop();
	Thread->WaitForCompletion();
}

MyoLambdaRunnable* MyoLambdaRunnable::RunLambdaOnBackGroundThread(TFunction< void()> InFunction)
{
	MyoLambdaRunnable* Runnable;
	if (FPlatformProcess::SupportsMultithreading())
	{
		Runnable = new MyoLambdaRunnable(InFunction);
		//UE_LOG(LogClass, Log, TEXT("FLambdaRunnable RunLambdaBackGroundThread"));
		return Runnable;
	}
	else
	{
		return nullptr;
	}
}

FGraphEventRef MyoLambdaRunnable::RunShortLambdaOnGameThread(TFunction< void()> InFunction)
{
	return FFunctionGraphTask::CreateAndDispatchWhenReady(InFunction, TStatId(), nullptr, ENamedThreads::GameThread);
}

void MyoLambdaRunnable::ShutdownThreads()
{
	/*for (auto Runnable : Runnables)
	{
	if (Runnable != nullptr)
	{
	delete Runnable;
	}
	Runnable = nullptr;
	}*/
}