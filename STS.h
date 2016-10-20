#include "LTS.h"
#include "Node.h"
#include "Processor.h"
#include "ProcessInfo.h"

#pragma once
class STS
{
public:
	STS();
	~STS();
	void addToQueue(int pid); // adds to linkedlist
	void removeFrom(int pid); // removes from linked list
	void interrupt(int pid); //sends interrupt signal to processor in case of higher priority.
	void action(int pid); //determines action to be taken after checkstate
	void incPriority(int pid); //increases priority after a certian wait time; DOES NOT NEED IMPLEMENTATION YET

private:
	ProcessState checkState(int pid); //Checks process state to determine action
	unsigned long checkCTme(int pid); //checks time on CPU for each process
	unsigned long checkWTime(int pid); //checks time in queue
	Node<PCB>* pList;
};
