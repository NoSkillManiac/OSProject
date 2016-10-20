#ifndef INCLUDE_STS
#define INCLUDE_STS

#include "Kernel.h"
#include "Node.h"
#include "PID.h"
#include "Processor.h"

class Kernel;
class Processor;

#pragma once
class STS
{
public:
	STS(Kernel* kernel, Processor** cpus);
	~STS();
	void addToQueue(PID* pid); // adds to linkedlist
	void addToScheduler(PID* pi); //adds process to scheduler
	void removeFrom(PID* pid); // removes from linked list
	void action(PID* pid); //determines action to be taken after checkstate
	void incPriority(PID* pid); //increases priority after a certian wait time; DOES NOT NEED IMPLEMENTATION YET
	void Event_CPUHalted(Processor* src, PID* prgrm, HaltReason reason); //called by kernel when a cpu halts

private:
	ProcessState checkState(PID* pid); //Checks process state to determine action
	unsigned long checkCTme(PID* pid); //checks time on CPU for each process
	unsigned long checkWTime(PID* pid); //checks time in queue
	Node<PID*>* pList;
	PID id;
	Kernel* kernel;
	Processor** processors;
};

#endif