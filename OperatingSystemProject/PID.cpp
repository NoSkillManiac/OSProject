#include "PID.h"


PID::PID()
{
	registers = new int[14];
	QueryPerformanceFrequency(&lastMeasurementFrequency);
}


PID::~PID()
{
	delete registers;
}


ProcessState PID::getState()
{
	return state;
}

void PID::setState(ProcessState s)
{
	LARGE_INTEGER currentTime;
	QueryPerformanceCounter(&currentTime);
	unsigned long ellapsedTime = (unsigned long)((currentTime.QuadPart - lastMeasurement.QuadPart) / lastMeasurementFrequency.QuadPart);

	if (state == RUNNING && s != RUNNING)
		cputime += ellapsedTime;
	else if (state == WAITING && s != WAITING)
		waittime += ellapsedTime;

	state = ProcessState(s);

	//get frequency for next measurement
	QueryPerformanceFrequency(&lastMeasurementFrequency);
	lastMeasurement = currentTime;
}

int PID::getID(PID pid)
{
	return id;
}

void PID::recordMemory(unsigned int* memory)
{
	this->simMemory = memory;
}

unsigned int* PID::getMemory()
{
	return this->simMemory;
}