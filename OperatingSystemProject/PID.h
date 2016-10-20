#ifndef INCLUDE_PID
#define INCLUDE_PID

#pragma once
enum ProcessState
{
	STARTING,
	RUNNING,
	WAITING,
	BLOCKED,
	TERMINATED
};

class PID
{
public:
	PID();
	~PID();

	ProcessState getState();
	void setState(ProcessState s);
	int getID(PID pid);

	int id;
	ProcessState state;
	int priority;
	unsigned int program_file;
	unsigned short counter;
	int* registers; //The 14 registers (0-13)
	unsigned short base_addr;
	unsigned short end_addr;
	int num_instructions;
	int in_buf_len;
	int out_buf_len;
	int temp_buf_len;
	unsigned long cputime;
	unsigned long waittime;
};

#endif