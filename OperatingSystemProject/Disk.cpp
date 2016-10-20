#include "Disk.h"

Disk::Disk()
{
	for (int i = 0; i < DISK_SIZE; i++)
	{
		this->nodes[i] = NULL;
	}
}


Disk::~Disk()
{
	for (int i = 0; i < DISK_SIZE; i++)
	{
		if (this->nodes[i] != NULL)
		{
			delete this->nodes[i];
			this->nodes[i] = NULL;
		}
	}
}

unsigned int Disk::mkfile()
{
	//the first node of a file only holds that file's size
	unsigned int location = this->indexof(NULL);

	if (location >= DISK_SIZE)
		throw "Disk Full.";

	this->nodes[location] = new Node<unsigned int>(0);
	return location;
}

void Disk::rm(unsigned int location)
{
	if (location < 0 || location >= DISK_SIZE)
		throw "Index out of bounds.";

	Node<unsigned int>* hdr = this->nodes[location];
	//verify that this is a legit file
	if (hdr == NULL)
		throw "location does not point to a valid file header!";

	Node<unsigned int>* current = hdr->getNext();

	//zero out the nodes in the hdd array
	while (current != NULL)
	{
		unsigned int location = this->indexof(current);
		if (location < DISK_SIZE)
			this->nodes[location] = NULL;
		current = current->getNext();
	}

	delete hdr; //delete the actual nodes
}

void Disk::append(unsigned int location, int* data, std::size_t size)
{
	if (location < 0 || location >= DISK_SIZE)
		throw "Index out of bounds.";

	Node<unsigned int>* hdr = this->nodes[location];
	if (hdr == NULL)
		throw "location does not point to a valid file header!";

	//track to the last node on this file
	Node<unsigned int>* current = hdr;
	while (current->getNext() != NULL)
	{
		current = current->getNext();
	}

	//current is now set to the last node in the file
	//now append the actual data and store on the disk
	for (int i = 0; i < size; i++)
	{
		unsigned int loc = this->indexof(NULL);
		if (loc > DISK_SIZE)
			throw "Disk Full.";
		current->setNext(new Node<unsigned int>(data[i]));
		current = current->getNext();
		this->nodes[loc] = current;
	}

	//add to the file size
	hdr->setData(hdr->getData() + size);
}

void Disk::set(unsigned int location, int* data, std::size_t size)
{
	if (location < 0 || location >= DISK_SIZE)
		throw "Index out of bounds.";

	Node<unsigned int>* hdr = this->nodes[location];
	if (hdr == NULL)
		throw "location does not point to a valid file header!";

	//delete the previous contents of this file from the nodes array
	Node<unsigned int>* current = hdr->getNext();
	while (current != NULL)
	{
		unsigned int loc = this->indexof(current);
		if (loc > DISK_SIZE)
			continue;
		this->nodes[loc] = NULL;
		current = current->getNext();
	}
	delete hdr->getNext(); //clears memory for every node beyond the header node

	//now add the data
	current = hdr;
	for (int i = 0; i < size; i++)
	{
		unsigned int loc = this->indexof(NULL);
		if (loc > DISK_SIZE)
			throw "Disk Full.";
		current->setNext(new Node<unsigned int>(data[i]));
		current = current->getNext();
		this->nodes[loc] = current;
	}
	hdr->setData(size);
}

std::vector<unsigned int>* Disk::load(unsigned int location)
{
	if (location < 0 || location >= DISK_SIZE)
		throw "Index out of bounds.";

	Node<unsigned int>* hdr = this->nodes[location];
	if (hdr == NULL)
		throw "location does not point ot a valid file header!";

	//get the file size
	size_t filesize = hdr->getData();

	//now iterate through all the nodes in the file and append their data to this vector/array
	std::vector<unsigned int>* data = new std::vector<unsigned int>();
	std::vector<unsigned int>::iterator it = data->begin();
	Node<unsigned int>* current = hdr->getNext();
	int index = 0;
	while (current != NULL)
	{
		data->insert(it, current->getData());
		index++;
		it = data->begin() + index;
		current = current->getNext();
	}
	//while (current != NULL)
	//{
	//	data->push_back(current->getData());
	//	current = current->getNext();
	//}

	return data;
}

unsigned int Disk::indexof(Node<unsigned int>* othernode)
{
	for (unsigned int i = 0; i < DISK_SIZE; i++)
	{
		if (this->nodes[i] == othernode)
			return i;
	}
	return DISK_SIZE + 1; //a value > DISK_SIZE means no space left
}