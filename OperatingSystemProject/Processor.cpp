#include "Processor.h"


Processor::Processor(Kernel* kernel, Memory* ram)
{
	this->registers = new int[16];
	this->physical_ram = ram;
	this->run_thread = NULL;
	this->kernel = kernel;
}

Processor::~Processor()
{
	if (this->run_thread->joinable())
		this->run_thread->join();
	delete this->run_thread;
	delete this->registers;
}

PID* Processor::getContext()
{
	return this->context;
}

void Processor::clearContext()
{
	for (int i = 0; i < 14; i++)
	{
		this->registers[i] = 0;
	}
	this->context = NULL;
}

void Processor::setContext(PID* pInfo)
{
	for (int i = 0; i < 14; i++)
	{
		this->registers[i] = pInfo->registers[i];
	}
	this->context = pInfo;
	this->pc = pInfo->counter;
}

void Processor::resume()
{
	halted = false;
};

void Processor::run()
{
	while (running)
	{
		//check every 100 microseconds if it's time to wake back up
		while (halted)
		{
			std::this_thread::sleep_for(std::chrono::microseconds(100));
		}

		while (!halted)
		{
			unsigned int instruction = fetch(); //get the next instruction from memory
			decode_and_execute(instruction); //decode and execute instruction
			this->pc++; //increment the program counter
		}

		//throw halted event
		this->kernel->Event_CPUHalted(this, this->context, PROGRAM_END);
	}
};

int Processor::fetch()
{
	unsigned int addr = this->pc + this->context->base_addr;
	return this->physical_ram->get(addr, false);
};

void Processor::systemTerminate()
{
	this->running = false;
}

void Processor::decode_and_execute(unsigned int instruction)
{
	unsigned int type = (instruction & 0xC0000000); //isolate the two leftmost bits
	unsigned int opcode = (instruction & 0x3F000000) >> 24; //isolate the 6 opcode bits and shift them to the far right

	if (opcode == OP_NOP)
	{
		this->NOP();
		return;
	}

	//type does not get shifted. The two type bits simply get isolated and then compared to save time from bitshifting
	//opcode has the 6 opcode bits isolated and then shifted because easier

	if (type == INSTRUCTION_ARITHMETIC)
	{
		unsigned char s1 = (unsigned char)((instruction & 0x00F00000) >> 20);
		unsigned char s2 = (unsigned char)((instruction & 0x000F0000) >> 16);
		unsigned char dr = (unsigned char)((instruction & 0x0000F000) >> 12);

		if (type == INSTRUCTION_ARITHMETIC)
			this->pc = this->pc;

		switch (opcode)
		{
		case OP_MOV:
			this->MOV(s1, dr);
			break;
		case OP_ADD:
			this->ADD(s1, s2, dr);
			break;
		case OP_SUB:
			this->SUB(s1, s2, dr);
			break;
		case OP_MUL:
			this->MUL(s1, s2, dr);
			break;
		case OP_DIV:
			this->DIV(s1, s2, dr);
			break;
		case OP_AND:
			this->AND(s1, s2, dr);
			break;
		case OP_OR:
			this->OR(s1, s2, dr);
			break;
		case OP_SLT:
			this->SLT(s1, s2, dr);
			break;
		default:
			this->pc = this->pc;
			break;
		}
	}
	else if (type == INSTRUCTION_CONDITIONAL)
	{
		unsigned char br = (unsigned char)((instruction & 0x00F00000) >> 20);
		unsigned char dr = (unsigned char)((instruction & 0x000F0000) >> 16);
		unsigned short addr = (unsigned short)(instruction & 0x0000FFFF);

		if (type == INSTRUCTION_CONDITIONAL)
			this->pc = this->pc;

		switch (opcode)
		{
		case OP_BEQ:
			this->BEQ(br, dr, addr);
			break;
		case OP_BNE:
			this->BNE(br, dr, addr);
			break;
		case OP_BEZ:
			this->BEZ(br, addr);
			break;
		case OP_BNZ:
			this->BNZ(br, addr);
			break;
		case OP_BGZ:
			this->BGZ(br, addr);
			break;
		case OP_BLZ:
			this->BLZ(br, addr);
			break;
		case OP_ST:
			this->ST(br, addr);
			break;
		case OP_ADDI:
			this->ADDI(addr, dr);
			break;
		case OP_MOVI:
			this->MOVI(addr, dr);
			break;
		case OP_LDI:
			this->LDI(addr, dr);
			break;
		case OP_LW:
			this->LW(dr, addr);
			break;
		default:
			this->pc = this->pc;
			break;
		}
	}
	else if (type == INSTRUCTION_IO)
	{
		unsigned char r1 = (unsigned char)((instruction & 0x00F00000) >> 20);
		unsigned char r2 = (unsigned char)((instruction & 0x000F0000) >> 16);
		unsigned short addr_data = (unsigned short)(instruction & 0x0000FFFF);

		if (type == INSTRUCTION_IO)
			this->pc = this->pc;

		switch (opcode)
		{
		case OP_RD:
			this->RD(addr_data, r1, r2);
			break;
		case OP_WR:
			this->WR(addr_data, r1, r2);
			break;
		case OP_MULI:
			this->MULI(addr_data, r1);
			break;
		case OP_DIVI:
			this->DIVI(addr_data, r1);
			break;
		case OP_SLTI:
			this->SLTI(r1, addr_data, r2);
			break;
		default:
			this->pc = this->pc;
			break;
		}
	}
	else /*if(type == INSTRUCTION_JUMP*/
	{
		//unsigned short instead of unsigned int (even though the instruction supports
		//up to 24 bits for the addr parameter) because this virtual machine doesn't have
		//anywhere close to that much ram.

		unsigned short addr = (unsigned short)(instruction & 0x00FFFFFF);

		switch (opcode)
		{
		case OP_HLT:
			this->HLT();
			break;
		case OP_JMP:
			this->JMP(addr);
			break;
		default:
			this->pc = this->pc;
			break;
		}
	}
};

void Processor::interrupt()
{
	this->halted = true; //set halted so the execution loop thread will stop itself

	//now that the CPU is halted, call the interrupt event through the Kernel
	this->kernel->Event_CPUHalted(this, this->context, INTERRUPTED);
};

void Processor::RD(unsigned short addr, unsigned char reg, unsigned char sreg)
{
	unsigned int faddr;
	if (addr != 0)
		faddr = addr + (unsigned int)(this->context->base_addr*4);
	else
		faddr = (unsigned int)this->registers[sreg] + (unsigned int)(this->context->base_addr*4);
	this->registers[reg] = this->physical_ram->get(faddr, true);
};

void Processor::WR(unsigned short addr, unsigned char reg, unsigned char sreg)
{
	unsigned int faddr;
	if (addr != 0)
		faddr = addr + (unsigned int)(this->context->base_addr * 4);
	else
		faddr = (unsigned int)this->registers[sreg] + (unsigned int)(this->context->base_addr * 4);
	this->physical_ram->set(faddr, (unsigned int)this->registers[reg], true);
};

void Processor::ST(unsigned char reg1, unsigned short addr)
{
	unsigned int faddr = addr + (unsigned int)(this->context->base_addr * 4);
	this->physical_ram->set(addr, this->registers[reg1], true);
};

void Processor::LW(unsigned char reg1, unsigned short addr)
{
	this->registers[reg1] = this->physical_ram->get(addr, true);
};

void Processor::MOV(unsigned char reg1, unsigned char reg2)
{
	this->registers[reg2] = this->registers[reg1];
};

void Processor::ADD(unsigned char sreg1, unsigned char sreg2, unsigned char dreg)
{
	this->registers[dreg] = this->registers[sreg1] + this->registers[sreg2];
};

void Processor::SUB(unsigned char sreg1, unsigned char sreg2, unsigned char dreg)
{
	this->registers[dreg] = this->registers[sreg1] - this->registers[sreg2];
};

void Processor::MUL(unsigned char sreg1, unsigned char sreg2, unsigned char dreg)
{
	this->registers[dreg] = this->registers[sreg1] * this->registers[sreg2];
};

void Processor::DIV(unsigned char sreg1, unsigned char sreg2, unsigned char dreg)
{
	this->registers[dreg] = this->registers[sreg1] / this->registers[sreg2];
};

void Processor::AND(unsigned char sreg1, unsigned char sreg2, unsigned char dreg)
{
	this->registers[dreg] = this->registers[sreg1] && this->registers[sreg2];
};

void Processor::OR(unsigned char sreg1, unsigned char sreg2, unsigned char dreg)
{
	this->registers[dreg] = this->registers[sreg1] || this->registers[sreg2];
};

void Processor::MOVI(int data, unsigned char dreg)
{
	this->registers[dreg] = data;
};

void Processor::ADDI(int imm, unsigned char dreg)
{
	this->registers[dreg] += imm;
};

void Processor::MULI(int imm, unsigned char dreg)
{
	this->registers[dreg] *= imm;
};

void Processor::DIVI(int data, unsigned char dreg)
{
	this->registers[dreg] /= data;
};

void Processor::LDI(short data, unsigned char dreg)
{
	//I am assuming this sets the value of dreg to the value held at the memory location
	//this->registers[dreg] = this->physical_ram->get((unsigned short)data + (this->context->base_addr*4), true);
	this->registers[dreg] = data;
};

void Processor::SLT(unsigned char sreg, unsigned char breg, unsigned char dreg)
{
	if (this->registers[sreg] < this->registers[breg])
		this->registers[dreg] = 1;
	else
		this->registers[dreg] = 0;
};

void Processor::SLTI(unsigned char sreg, int imm, unsigned char dreg)
{
	//not ready to implement
	if (this->registers[sreg] > imm)
		this->registers[dreg] = 1;
	else
		this->registers[dreg] = 0;
};

void Processor::HLT()
{
	this->halted = true;
};

void Processor::NOP()
{
	//no-op, this function intentionally left blank
};

void Processor::JMP(unsigned short addr)
{
	this->pc = addr;
}

void Processor::BEQ(unsigned char breg, unsigned char dreg, unsigned short addr)
{
	if (this->registers[breg] == this->registers[dreg])
		this->pc = addr/4;
};

void Processor::BNE(unsigned char breg, unsigned char dreg, unsigned short addr)
{
	if (this->registers[breg] != this->registers[dreg])
		this->pc = addr/4;
};

void Processor::BEZ(unsigned char breg, unsigned short addr)
{
	if (this->registers[breg] == 0)
		this->pc = addr/4;
};

void Processor::BNZ(unsigned char breg, unsigned short addr)
{
	if (this->registers[breg] != 0)
		this->pc = addr/4;
};

void Processor::BGZ(unsigned char breg, unsigned short addr)
{
	if (this->registers[breg] > 0)
		this->pc = addr/4;
};

void Processor::BLZ(unsigned char breg, unsigned short addr)
{
	if (this->registers[breg] < 0)
		this->pc = addr/4;
};