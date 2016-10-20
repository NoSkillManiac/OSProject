#include "ProcessInfo.h"


ProcessInfo::ProcessInfo()
{
	registers = new int[14];
}


ProcessInfo::~ProcessInfo()
{
	delete registers;
}
