#ifndef INCLUDE_MEMORY
#define INCLUDE_MEMORY

#pragma once
class Memory
{
public:
	Memory();
	~Memory();

	unsigned int get(unsigned int addr, bool byteaddr = false) const;
	void set(unsigned int addr, unsigned int data, bool byteaddr = false);

private:
	unsigned int* memory;
};

#endif