#include "PID.h"


PID::PID()
{
	registers = new int[14];
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
	SYSTEMTIME time;
	GetSystemTime(&time);
	unsigned long current = time.wSecond * 1000 + time.wMilliseconds;
	unsigned long ellapsed = current - lastMeasurement;

	if (state == RUNNING && s != RUNNING)
		cputime += ellapsed;
	else if (state == WAITING && s != WAITING)
		waittime += ellapsed;
	lastMeasurement = current;

	state = ProcessState(s);
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