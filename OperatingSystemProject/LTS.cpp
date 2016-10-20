#include "LTS.h"
#include "Node.h"
#include <iostream>


LTS::LTS(Kernel* kernel)
{
	this->pList = NULL;
	this->kernel = kernel;
}

LTS::~LTS()
{
	//node uses cascading delete on its elements automatically
	delete pList;
}

void LTS::addToQueue(PID* pid) //adds to end of queue
{
	//process is now waiting in queue
	pid->setState(WAITING);

	//if number of programs in RAM < degree of multiprocessing => Load program into memory and add to the STS (and increment numLoaded)
	if (numLoaded < DM)
	{
		//get the file data
		std::vector<unsigned int>* filedata = (this->kernel->GetDisk()->load(pid->program_file));
		int size = filedata->size();
		int* file = new int[size];
		for (int i = 0; i < size; i++)
		{
			file[i] = filedata->front();
			filedata->erase(filedata->begin());
		}
		delete filedata;

		//first load instructions into memory
		int end_instructions = 3 + pid->num_instructions; //end_instructions is the index of the first data section descriptor
		pid->base_addr = this->kernel->getLastUsedAddr();
		pid->end_addr = pid->base_addr + size;
		this->kernel->setLastUsedAddr(pid->end_addr + 1);
		int current_addr = pid->base_addr; //the address we are currently referencing in memory to write to
		for (int i = 3; i < end_instructions; i++) //skip first three values as they are just program details
		{
			this->kernel->GetMemory()->set(current_addr, file[i]);
			current_addr++;
		}

		//now load data into memory
		int end_data = size;
		for (int i = end_instructions + 3; i < end_data; i++)
		{
			this->kernel->GetMemory()->set(current_addr, file[i]);
			current_addr++;
		}

		//place program into sts
		this->kernel->GetSTS()->addToScheduler(pid);
		numLoaded++;
		//delete file;
	}
	else
	{
		//insert process into queue in order based on priority
		Node<PID*>* current = pList;
		Node<PID*>* t = new Node<PID*>(pid);
		if (current == NULL)
			pList = t;
		else if (current->getData()->priority > pid->priority)
		{
			t->setNext(pList);
			pList = t;
		}
		else
		{
			while (current != NULL)
			{
				Node<PID*>* next = current->getNext();
				if (next == NULL)
				{
					current->setNext(t);
					break;
				}
				else if (next->getData()->priority > pid->priority)
				{
					t->setNext(next);
					current->setNext(t);
					break;
				}
				current = next; //only happens if t hasn't been added yet
			}
		}

		if (numLoaded == 45)
		{
			Node<PID*>* current = this->pList;
			while (current != NULL)
			{
				PID* id = current->getData();
				std::cout << "LTS::Process " << id->id << "\tPriority: " << id->priority << std::endl;
				current = current->getNext();
			}

			this->kernel->GetSTS()->Event_CPUHalted(NULL, pList->getData(), IO);
		}
	}
}

void LTS::removeFrom(PID* pid)
{
	Node<PID*> *current = pList;
	if (current == NULL)
		return;
	if (current->getData()->id == pid->id) //first element is a match
	{
		pList = current->getNext();
		current->setNext(NULL);
		delete current;
		return;
	}

	while (current != NULL)
	{
		Node<PID*>* next = current->getNext();
		if (next == NULL)
			return;
		else if(next->getData()->id == pid->id)
		{
			current->setNext(next->getNext());
			next->setNext(NULL);
			delete next;
			return;
		}
		current = current->getNext();
	}
}

void LTS::assignProcessor(unsigned int i)
{
	//May not need implementation
}

void LTS::checkBalance()
{
	//may not need implementation
}

void LTS::Event_CPUHalted(Processor* src, PID* prgrm, HaltReason reason)
{
	//LTS only cares (for now) when a program ends, that way it can add another to the STS if any remain
	if (reason == PROGRAM_END)
	{
		//terminate finished process and reclaim memory
		this->kernel->TerminateProcess(prgrm);
		numLoaded--;
	}
}