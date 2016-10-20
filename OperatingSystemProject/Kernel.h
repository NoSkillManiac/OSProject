#ifndef INCLUDE_KERNEL
#define INCLUDE_KERNEL

#include "PCB.h"
#include "Processor.h"
#include "Memory.h"
#include "Disk.h"
#include "LTS.h"
#include "STS.h"
#include <vector>

#define NUM_PROCESSORS 1

//foward declarators since the schedulers need the kernel
class LTS;
class STS;

#pragma once
class Kernel
{
public:
	Kernel(Memory* virtualram, Disk* virtualdisk, std::vector<unsigned int>* directory);
	~Kernel();
	PID* CreateProcess(unsigned int file); //creates a process, adds the PID to the pcb and returns a pointer to that PID object
	void TerminateProcess(PID* process);
	void Event_CPUHalted(Processor* src, PID* prgrm, HaltReason reason);
	STS* GetSTS();
	Disk* GetDisk();
	Memory* GetMemory();
	unsigned int getLastUsedAddr() const
	{
		return lastUsedAddr;
	}
	void setLastUsedAddr(unsigned int lastUsed)
	{
		lastUsedAddr = lastUsed;
	}

private:
	Processor** processors;
	PCB* pcb;
	Memory* ram;
	Disk* hdd;
	std::vector<unsigned int>* files;
	LTS* lts;
	STS* sts;
	unsigned int lastUsedAddr = 0;
};

#endif