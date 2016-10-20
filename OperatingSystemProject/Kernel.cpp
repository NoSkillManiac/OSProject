#include "Kernel.h"
#include <vector>
#include <iostream>

Kernel::Kernel(Memory* virtualram, Disk* virtualdisk, std::vector<unsigned int>* directory)
{
	//accept the parameters
	this->files = directory;
	this->ram = virtualram;
	this->hdd = virtualdisk;

	//initialize the pcb
	this->pcb = new PCB();

	//initialize the processor(s)
	this->processors = new Processor*[NUM_PROCESSORS];
	for (int i = 0; i < NUM_PROCESSORS; i++)
	{
		//this->processors[i] = new Processor(this, virtualram);
		this->processors[i] = new Processor(this, virtualram);
	}

	//initialize the long term scheduler
	this->lts = new LTS(this);

	//Initialize short term scheduler
	this->sts = new STS(this, processors);

	std::cout << "Kernel Initialized... Loading Programs..." << std::endl;

	//load all 30 programs as processes, and send them to the LTS
	//the LTS will then decide which processes to bring into memory and when
	for (int i = 0; i < this->files->size(); i++)
	{
		std::cout << "Creating process from file: " << (*this->files)[i] << std::endl;
		PID* pInfo = CreateProcess((*this->files)[i]);
		std::cout << "Process (ID: " << pInfo->id << ") Created." << std::endl;
		this->lts->addToQueue(pInfo);
		std::cout << "Added process to lts" << std::endl;
	}

	while (true) {}
}


Kernel::~Kernel()
{
	//dispose of cpus
	for (int i = 0; i < NUM_PROCESSORS; i++)
	{
		Processor* proc = this->processors[i];
		delete proc;
	}
	delete this->processors;

	//dispose of data structures
	delete files;
	delete ram;
	delete hdd;
	delete pcb;
}

PID* Kernel::CreateProcess(unsigned int file)
{
	std::vector<unsigned int> data = *hdd->load(file);
	
	//get job description data
	int index = data[0];
	int instruction_len = data[1];
	int priority = data[2];

	//for now, memory is used contiguously from 0 to pcb->GetProcess(index-1)->end_addr
	//therefore base_addr can be end_addr + 1
	unsigned int base_addr = 0;
	if (index > 0)
		base_addr = this->lastUsedAddr;

	//load data parameters
	unsigned int base_data = base_addr + instruction_len; //location where data starts
	size_t data_info_start = 2 + instruction_len;
	int input_buffer_len = data[data_info_start + 1];
	int output_buffer_len = data[data_info_start + 2];
	int temp_buffer_len = data[data_info_start + 3];
	int data_length = input_buffer_len + output_buffer_len + temp_buffer_len;

	//calculate end_addr
	int end_addr = base_data + data_length;

	//create the processinfo
	PID* pInfo = new PID();
	pInfo->id = index;
	pInfo->base_addr = base_addr;
	pInfo->end_addr = end_addr;
	pInfo->in_buf_len = input_buffer_len;
	pInfo->out_buf_len = output_buffer_len;
	pInfo->temp_buf_len = temp_buffer_len;
	pInfo->num_instructions = instruction_len;
	pInfo->state = (ProcessState)STARTING;
	pInfo->priority = priority;
	pInfo->program_file = file;

	this->pcb->AddProcess(pInfo);
	//delete datafile;
	return pInfo;
}

void Kernel::TerminateProcess(PID* pInfo)
{
	this->lts->removeFrom(pInfo);
	pInfo->state = (ProcessState)TERMINATED;

	//remove process from lts and sts
	this->sts->removeFrom(pInfo);
	this->lts->removeFrom(pInfo);

	//free memory up
	for (unsigned int index = pInfo->base_addr; index <= pInfo->end_addr; index++)
	{
		this->ram->set(index, 0x0);
	}
}

STS* Kernel::GetSTS()
{
	return this->sts;
}

Disk* Kernel::GetDisk()
{
	return this->hdd;
}

Memory* Kernel::GetMemory()
{
	return this->ram;
}

void Kernel::Event_CPUHalted(Processor* src, PID* prgrm, HaltReason reason)
{
	this->lts->Event_CPUHalted(src, prgrm, reason);
	this->sts->Event_CPUHalted(src, prgrm, reason);
}