#include "PCB.h"

PCB::PCB()
{
	processes = new std::vector<PID*>();
}


PCB::~PCB()
{
	//delete all the PInfo objects
	for (int i = 0; i < processes->size(); i++)
	{
		PID* pi = (*processes)[i];
		delete pi;
	}
	delete processes;
}

void PCB::AddProcess(PID* pi)
{
	processes->push_back(pi);
}

PID* PCB::RemProcess(int index)
{
	for (int i = 0; i < processes->size(); i++)
	{
		PID* pi = (*processes)[i];
		if (pi->id == index)
		{
			processes->erase(processes->begin() + i);
			return pi;
		}
	}

	return NULL;
}

PID* PCB::GetProcess(int index)
{
	for (int i = 0; i < processes->size(); i++)
	{
		PID* pi = (*processes)[i];
		if (pi->id == index)
		{
			return pi;
		}
	}

	return NULL;
}