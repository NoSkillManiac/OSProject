#include "STS.h"

STS::STS()
{
	this->pList = NULL;
}

STS::~STS()
{
	while (pList->getNext() != NULL)
	{
		Node<PID> *t = pList;
		pList = pList -> getNext();
		delete t;
	}

}
void STS::addToQueue(PID pi) //adds to end of queue
{
	Node<PID> *t = new Node<PID>(pi);
	while (pList -> getNext() != NULL)
		pList = pList ->getNext();
	pList -> setNext(t);
}

void STS::removeFrom(PID pid)
{
	Node<PID> *t;
	if (pid.getID(pList -> getData()) == pid.getID(pid))
	{
		t = pList;
		pList = pList -> getNext();
		delete t;
	}
	else
		while (pList -> getNext() != NULL)
			if (pid.getID(pList -> getNext() -> getData()) == pid.getID(pid))
			{
				t = pList -> getNext();
				pList -> setNext(t->getNext());
				delete t;
			}
			else
				pList = pList -> getNext();
}

void interrupt(PID pid); //sends interrupt signal to processor in case of higher priority. Not needed right now.
void STS::action(PID pid)
{
	ProcessState ps = checkState(pid);
	switch (ps)
	{
	case 0: //starting
		pid.setState(pid,2);
		addToQueue(pid);
		break; 

	case 1: //running
		pid.setState(pid,2);
		addToQueue(pid);
		//need to halt current processes.
		break;

	case 2: //waiting
		//check cpu usage
		// if(cpu usage > 0)
			// pid.setState(pid,1);
			// removeFrom(pid);
		break;

	case 3: //blocked
		pid.setState(pid,2);
		addToQueue(pid);
		break;

	case 4: //terminated
		break;
	}
}
void incPriority(PID pid); //increases priority after a certian wait time; DOES NOT NEED IMPLEMENTATION YET


ProcessState STS::checkState(PID pid)
{
	return pid.getState(pid);
}
