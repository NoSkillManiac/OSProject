#include "CpuHaltEvent.h"

CpuHaltEvent::CpuHaltEvent(Processor* src, PID* pInfo, HaltReason reason)
{
	this->src = src;
	this->pInfo = pInfo;
	this->reason = reason;
}

CpuHaltEvent::~CpuHaltEvent(){}

Processor* CpuHaltEvent::getSrc()
{
	return this->src;
}

PID* CpuHaltEvent::getProcess()
{
	return this->pInfo;
}

HaltReason CpuHaltEvent::getHaltReason()
{
	return this->reason;
}