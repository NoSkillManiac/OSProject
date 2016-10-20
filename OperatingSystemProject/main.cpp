#include "Disk.h"
#include "Memory.h"
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <queue>
#include "Kernel.h"
#include <iostream>

using namespace std;

Disk* hdd;
Memory* mem;
vector<unsigned int>* directory;
Kernel* kernel;

enum readstate { NONE, TEXT, DATA };

int* splitstr(string line)
{
	stringstream ss;
	string token;
	int* hdr = new int[3];
	int curlen;

	//first piece
	curlen = line.find(" ");
	token = line.substr(0, curlen);
	ss << hex << token;
	ss >> hdr[0];
	ss.clear();
	line = line.substr(curlen + 1); //cut off the token amt

	//second piece
	curlen = line.find(" ");
	token = line.substr(0, curlen);
	ss << hex << token;
	ss >> hdr[1];
	ss.clear();
	line = line.substr(curlen + 1);

	//third piece should just be the last number and then end of string
	ss << hex << line;
	ss >> hdr[2];

	return hdr;
}

int* queueToArray(queue<int>* inputdata)
{
	size_t length = inputdata->size();
	int* values = new int[length];
	int currentindex = 0;
	while (currentindex < length)
	{
		values[currentindex] = inputdata->front();
		inputdata->pop();
		currentindex++;
	}
	return values;
}

void loadPrograms()
{
	//load the program file
	try
	{
		ifstream file;
		file.open("Program-File.txt");
		if (file.is_open())
			cout << "FILE IS OPEN!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
		//file.exceptions(std::ifstream::failbit);
		string line("");
		readstate state = NONE;
		unsigned int currentfile = -1;
		queue<int> instructions;

		while (getline(file, line))
		{
			if (line.find("END") != string::npos)
				continue;
			else if (line.find("JOB") != string::npos)
			{
				//save previous bits of data to hdd
				if (currentfile != -1)
				{
					int size = instructions.size();
					int* datawrite = queueToArray(&instructions);
					hdd->append(currentfile, datawrite, size);
				}

				//std::cout << line << endl;
				state = TEXT;

				//start a new file
				currentfile = hdd->mkfile();
				directory->push_back(currentfile);

				//remove "// JOB "
				line = line.substr(7);

				//parse line into pieces
				int* hdr = splitstr(line);

				//append the numbers to the hdd file
				hdd->append(currentfile, hdr, 3);

				//mem clean
				delete hdr;
			}
			else if (line.find("Data ") != string::npos)
			{
				//save previous bits of data to hdd
				if (currentfile != -1)
				{
					int size = instructions.size();
					int* datawrite = queueToArray(&instructions);
					hdd->append(currentfile, datawrite, size);
				}

				//std::cout << line << endl;
				state = DATA;

				//remove "// Data "
				line = line.substr(9);

				//split into the three number components
				int* hdr = splitstr(line);

				//write to file
				hdd->append(currentfile, hdr, 3);

				//mem cleanup
				delete hdr;
			}
			else
			{
				//std::cout << line << endl;
				//some data being read
				line = line.substr(2); //cuf off the 0x bit
				stringstream ss;
				ss << hex << line;
				unsigned int ins;
				ss >> ins;
				instructions.push(ins);
			}
		}
	}
	catch (exception e)
	{
		std::cout << e.what() << endl;
	}
}

int main()
{
	std::cout << "Starting Virtual Machine..." << endl;

	//initialize the directory
	directory = new vector<unsigned int>();

	//initialize hard drive
	hdd = new Disk();

	//initialize virtual memory
	mem = new Memory();

	std::cout << "Virtual Hardware Initialized..." << endl;

	//load programs from host file into virtual hard drive
	try
	{
		loadPrograms();
	}
	catch (exception e)
	{
		cout << e.what() << endl;
	}

	std::cout << "Programs loaded from host into virtual drive..." << endl;
	std::cout << "Initializing Kernel..." << endl;

	//initialize the kernel and its subsystems
	kernel = new Kernel(mem, hdd, directory);
	std::cout << "Kernel closing..." << endl;

	std::cout << "Press any key to continue...";
	std::cin.get(); //wait for keypress before ending

	return 0;
}