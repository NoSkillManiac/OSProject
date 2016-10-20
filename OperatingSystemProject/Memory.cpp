#include "Memory.h"
#include <iostream>


Memory::Memory()
{
	this->memory = new unsigned int[2048];
}

Memory::~Memory()
{
	delete this->memory;
}

unsigned int Memory::get(unsigned int addr, bool byteaddr) const
{
	if (!byteaddr)
		return this->memory[addr];
	else
	{
		unsigned int raddr = addr / 4;
		if (addr % 4 == 0)
			return this->memory[raddr];
	}
}

void Memory::set(unsigned int addr, unsigned int data, bool byteaddr)
{
	if (!byteaddr)
		this->memory[addr] = data;
	else
	{
		unsigned int raddr = addr / 4;
		if (addr % 4 == 0)
			this->memory[raddr] = data;
		else
		{
			this->memory[raddr] = data;
			std::cout << "ACCESSING BYTE MEMORY WHERE %4 != 0!!!";
		}
	}
}
