#ifndef INCLUDE_LTS
#define INCLUDE_LTS

#define DM 10 //degree of multiprogramming

#include "Kernel.h"
#include <vector>
#include "PID.h"
#include "Node.h"

class Kernel;
class Processor;

#pragma once
class LTS
{
public: 
	LTS(Kernel* kernel);
	~LTS();
	void addToQueue(PID* pid); //adds to LinkedList
	void removeFrom(PID* pid); //Removes from LinkedList
	void assignProcessor(unsigned int i); //If 1 processor, always uses 0
	void checkBalance(); //checks the balance of the CPU load if multiple processors
	void Event_CPUHalted(Processor* src, PID* prgrm, HaltReason reason); //called by the kernel when a cpu halts
	void MapProcess(PID* pInfo); //maps a process into memory
	void UnMapProcess(PID* pInfo); //unmaps/removes a process from memory and reclaims the memory

private:
	PID *conBlock;
	Node<PID*>* pList;
	Kernel* kernel;
	int numLoaded = 0;
};

#endif