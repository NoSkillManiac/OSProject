#include "PID.h"


PID::PID()
{
	registers = new int[14];
}


PID::~PID()
{
	delete registers;
}


ProcessState PID::getState()
{
	return state;
}

void PID::setState(ProcessState s)
{
	state = ProcessState(s);
}

int PID::getID(PID pid)
{
	return id;
}