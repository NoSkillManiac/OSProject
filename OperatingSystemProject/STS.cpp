#include "STS.h"
#include <cstdlib>
#include <iostream>

STS::STS(Kernel* kernel, Processor** cpus)
{
	this->pList = NULL;
	this->kernel = kernel;
	this->processors = cpus;
}

STS::~STS()
{
	//node uses cascading deletion
	delete this->pList;
}

void STS::addToScheduler(PID* pi) //adds to the scheduler
{
	//until scheduling/running happens, set pi to waiting (if not already)
	pi->setState(WAITING);

	//scheduling is preemptive. Check for available CPU cores
	//first we check for CPU not doing anything
	for (int i = 0; i < NUM_PROCESSORS; i++)
	{
		if (this->processors[i]->getContext() == NULL)
		{
			//processor is available
			Processor* processor = this->processors[i]; //get cpu
			processor->setContext(pi); //set context
			processor->resume(); //begin executing process
			pi->setState(RUNNING); //set process state to running
			return;
		}
	}

	//if we make it here then no CPU was available. Find CPU with lowest priority (highest priority value), and see if pi should be switched in to the CPU
	Processor* lowestProcessor = this->processors[0];
	for (int i = 1; i < NUM_PROCESSORS; i++)
	{
		if (this->processors[i]->getContext()->priority > lowestProcessor->getContext()->priority)
			lowestProcessor = this->processors[i];
	}

	//add process to the queue
	Node<PID*> *t = new Node<PID*>(pi);
	Node<PID*> *current = pList;
	if (pList == NULL)
		pList = t;
	else if (current->getData()->priority > pi->priority)
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
			else if (next->getData()->priority > pi->priority)
			{
				t->setNext(next);
				current->setNext(t);
				break;
			}
			current = next;
		}
	}

	//now that the queue has the process added, ordered by priority, see if this is a high enough priority process
	if (pi->priority < lowestProcessor->getContext()->priority)
	{
		//process being scheduled takes priority over the lowest priority process being executed currently
		//NOT READY TO PREEMPT
		//lowestProcessor->interrupt(); //interrupt that processor. Event_CPUHalted will handle the context switch
	}
}

void STS::addToQueue(PID* pi) //adds queue based on priority
{
	Node<PID*> *t = new Node<PID*>(pi);
	Node<PID*> *current = pList;
	if (current == NULL)
		pList = t;
	while (current->getNext() != NULL)
	{
		if (current->getData()->priority < t->getData()->priority)
		{
			t->setNext(current);
			pList = t;
		}
		else if (t->getData()->priority > current->getNext()->getData()->priority)
		{
			t->setNext(current->getNext());
			current->setNext(t);
		}
		else if (current->getNext()->getNext() == NULL && t->getData()->priority <= current->getNext()->getData()->priority)
			current->setNext(t);
		current = current->getNext();
	}

	//checkInterrupt(); -- Still needs implementing, will do 11/11.
}

void STS::removeFrom(PID* pid) //removes process from queue for any reason
{
	//check if list is empty
	Node<PID*>* current = pList;
	if (current == NULL)
		return;
	else if (current->getData()->id == pid->id)
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
		else if (next->getData()->id == pid->id)
		{
			current->setNext(next->getNext());
			next->setNext(NULL);
			delete next;
			return;
		}
		current = current->getNext();
	}
}

void STS::action(PID* pid)
{
	ProcessState ps = checkState(pid);
	switch (ps)
	{
	case 0: //starting
		pid->setState(STARTING);
		addToQueue(pid);
		break; 

	case 1: //running
		pid->setState(RUNNING);
		addToQueue(pid);
		//need to halt current processes.
		break;

	case 2: //waiting
		pid->setState(WAITING);
		break;

	case 3: //blocked
		pid->setState(BLOCKED);
		addToQueue(pid);
		break;

	case 4: //terminated
		
		break;
	}
}
void incPriority(PID* pid); //increases priority after a certian wait time; DOES NOT NEED IMPLEMENTATION YET

ProcessState STS::checkState(PID* pid)
{
	return pid->getState();
}

void STS::Event_CPUHalted(Processor* src, PID* prgrm, HaltReason reason)
{
	if (reason == IO)
	{
		Node<PID*> *current = pList;
		while (current != NULL)
		{
			PID* id = current->getData();
			std::cout << "STS::Process " << id->id << "\tPriority: " << id->priority << std::endl;
			current = current->getNext();
		}
		return;
	}

	if (&prgrm == NULL)
		return;

	if (reason == INTERRUPTED)
	{
		//cpu interrupted. Time to load next process
		this->addToQueue(prgrm); //add program back to waiting queue
		prgrm->setState(WAITING); //set state back to waiting
	}
	else if (reason == PROGRAM_END)
	{
		//cpu got to the end of the program so set it as terminated
		//the LTS will be handling freed-memory by itself
		prgrm->setState(TERMINATED);
		std::cout << "Process: " << prgrm->id << " completed successfully." << std::endl;
	}
	
	//clear processor context
	src->clearContext();

	//check if there is a program in queue to send to the cpu
	if (this->pList != NULL)
	{
		PID* pInfo = this->pList->getData();
		pInfo->setState(RUNNING);
		src->setContext(pInfo);
		this->removeFrom(pInfo);
		src->resume();
	}
}
