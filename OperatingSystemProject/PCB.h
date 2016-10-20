#ifndef INCLUDE_PCB
#define INCLUDE_PCB

#include "PID.h"
#include <vector>

#pragma once
class PCB
{
public:
	PCB();
	~PCB();
	void AddProcess(PID* pi);
	PID* RemProcess(int pid);
	PID* GetProcess(int pid);

private:
	std::vector<PID*>* processes;
	
};

#endif