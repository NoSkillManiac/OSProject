#include "PCB.h"
#include "Node.h"
#include <vector>
#include "Disk.h"

#pragma once
class LTS
{
public: 
	LTS();
	~LTS();
	void addToQueue(int pid); //adds to LinkedList
	void removeFrom(int pid); //Removes from LinkedList
	void assignProcessor(unsigned int i); //If 1 processor, always uses 0
	void checkBalance(); //checks the balance of the CPU load if multiple processors
private:
	PCB *conBlock;
	Node<PCB>* pList;
};
