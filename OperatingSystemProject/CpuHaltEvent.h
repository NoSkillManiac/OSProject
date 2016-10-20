#ifndef INCLUDE_CPU_HALT_EVENT_
#define INCLUDE_CPU_HALT_EVENT_

#include "Processor.h"
#include "PID.h"

class CpuHaltEvent
{
public:
	CpuHaltEvent(Processor* src, PID* pInfo, HaltReason reason);
	~CpuHaltEvent();
	Processor* getSrc();
	PID* getProcess();
	HaltReason getHaltReason();

private:
	Processor* src;
	PID* pInfo;
	HaltReason reason;
};

#endif