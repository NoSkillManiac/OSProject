#ifndef INCLUDE_DISK
#define INCLUDE_DISK

#pragma once
#include "Node.h"
#include <vector>

#define DISK_SIZE 4096

//unlike memory, the disk is expected to have it's own drivers which is why I am implementing
//the disk in this manner. Unlike memory where the operating system must specify the value of
//every word of ram individually while also reading every word of ram individually,
//with the disk, the os must simply perform file operations while the disk does the rest.

//I am not implementing this as a set of linked lists with a counter (the easier way) because
//I wanted to more closely simulate an actual hdd by having fake sectors like a hdd stored
//as an array. Since the hdd is so small, each sector is only one word long rather than
//64K or more like a real hdd.

class Disk
{
public:
	Disk();
	~Disk();
	
	//Creates a new blank file on the disk and returns its file location
	unsigned int mkfile();
	//Deletes a file from the disk
	void rm(unsigned int location);
	//appends data to a file on the disk
	void append(unsigned int location, int* data, std::size_t size);
	//sets the data of a file, overwriting any previous data
	void set(unsigned int location, int* data, std::size_t size);
	//loads data from the disk
	std::vector<unsigned int>* load(unsigned int location);

private:
	Node<unsigned int>* nodes[DISK_SIZE];

	//gets the lowest index holding the specified pointer. Passing nullptr will find the index of the first available word
	unsigned int indexof(Node<unsigned int>* othernode);
};

#endif