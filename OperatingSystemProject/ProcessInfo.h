#pragma once
enum ProcessState
{
	STARTING,
	RUNNING,
	WAITING,
	BLOCKED,
	TERMINATED
};

class ProcessInfo
{
public:
	ProcessInfo();
	~ProcessInfo();

	int pid;
	ProcessState state;
	unsigned int program_file;
	unsigned short counter;
	int* registers; //The 14 registers (0-13)
	unsigned short base_addr;
	unsigned short end_addr;
	int num_instructions;
	int num_data;
	int num_other;
	unsigned long cputime;
	unsigned long waittime;
};

