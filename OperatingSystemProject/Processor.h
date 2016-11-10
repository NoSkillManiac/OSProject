#ifndef INCLUDE_PROCESSOR
#define INCLUDE_PROCESSOR

#include <thread>
#include <mutex>
#include "Memory.h"
#include "PID.h"

#define INSTRUCTION_IO			0xC0000000 //11
#define INSTRUCTION_JUMP		0x80000000 //10
#define INSTRUCTION_CONDITIONAL	0x40000000 //01
#define INSTRUCTION_ARITHMETIC	0x00000000 //00

#define OP_RD					0x00
#define OP_WR					0x01
#define OP_ST					0x02
#define OP_LW					0x03
#define OP_MOV					0x04
#define OP_ADD					0x05
#define OP_SUB					0x06
#define OP_MUL					0x07
#define OP_DIV					0x08
#define OP_AND					0x09
#define OP_OR					0x0A
#define OP_MOVI					0x0B
#define OP_ADDI					0x0C
#define OP_MULI					0x0D
#define OP_DIVI					0x0E
#define OP_LDI					0x0F
#define OP_SLT					0x10
#define OP_SLTI					0x11
#define OP_HLT					0x12
#define OP_NOP					0x13
#define OP_JMP					0x14
#define OP_BEQ					0x15
#define OP_BNE					0x16
#define OP_BEZ					0x17
#define OP_BNZ					0x18
#define OP_BGZ					0x19
#define OP_BLZ					0x1A

enum HaltReason
{
	PROGRAM_END,
	INTERRUPTED,
	IO,
	PAGE_FAULT
};

#include "Kernel.h"
class Kernel;

class Processor
{
public:
	Processor(Kernel* kernel, Memory* ram);
	~Processor();
	void interrupt();
	void resume();
	PID* getContext();
	void clearContext();
	void setContext(PID* context);

	void systemTerminate();
	
private:
	//registers[0] is the accumulator
	//registers[1] is the zero register and cannot be changed
	//registers[2] --> registers[15] are general purpose registers
	int* registers; //holds 16 integers, initialized in the constructor
	unsigned short pc; //program counter
	bool halted = false; //is the processor halted right now?

	PID* context;
	Memory* physical_ram;
	std::thread* run_thread = NULL;
	std::mutex context_lock;
	Kernel* kernel;
	bool running = true;

	void run(); //runs the fetch, decode, execute loop
	int fetch(); //fetch the next instruction
	void decode_and_execute(unsigned int instruction); //decode the instruction given

	void RD(unsigned short addr, unsigned char reg, unsigned char sreg); //not sure how to implement yet
	void WR(unsigned short addr, unsigned char reg, unsigned char sreg); //not sure how to implement yet
	void ST(unsigned char reg1, unsigned short addr); //store contents of register into address
	void LW(unsigned char reg1, unsigned short addr); //load contents of address into register
	void MOV(unsigned char reg1, unsigned char reg2); //move contents from reg1 to reg2
	void ADD(unsigned char sreg1, unsigned char sreg2, unsigned char dreg); //add content of both sregs to dreg
	void SUB(unsigned char sreg1, unsigned char sreg2, unsigned char dreg); //sub sreg2 from sreg1 and store in dreg
	void MUL(unsigned char sreg1, unsigned char sreg2, unsigned char dreg); //multiplies sregs and store in dreg
	void DIV(unsigned char sreg1, unsigned char sreg2, unsigned char dreg); //sreg1/sreg2 --> dreg
	void AND(unsigned char sreg1, unsigned char sreg2, unsigned char dreg); //logical AND of sreg1 and sreg2, store in dreg
	void OR(unsigned char sreg1, unsigned char sreg2, unsigned char dreg); //logical OR of sreg1 and sreg2, store in dreg
	void MOVI(int data, unsigned char dreg); //transfer data/addr into dreg
	void ADDI(int imm, unsigned char dreg); //add imm to dreg
	void MULI(int imm, unsigned char dreg); //multiply imm data by dreg data
	void DIVI(int imm, unsigned char dreg); //divide dreg data by imm data
	void LDI(short data, unsigned char dreg); //load data/address into dreg
	void SLT(unsigned char sreg, unsigned char breg, unsigned char dreg); //set dreg to 1 if sreg < breg, 0 if otherwise
	void SLTI(unsigned char sreg, int imm, unsigned char dreg); //set dreg to 1 if sreg < imm, 0 if otherwise
	void HLT(); //logical end of the program
	void NOP(); //no-op
	void JMP(unsigned short addr); //jump to the set address (set the pc to the address of the instruction to jump to)
	void BEQ(unsigned char breg, unsigned char dreg, unsigned short addr); //set pc to addr if breg == dreg
	void BNE(unsigned char breg, unsigned char dreg, unsigned short addr); //set pc to addr if breg != dreg
	void BEZ(unsigned char breg, unsigned short addr); //set pc to addr if breg == 0
	void BNZ(unsigned char breg, unsigned short addr); //set pc to addr if breg != 0
	void BGZ(unsigned char breg, unsigned short addr); //set pc to addr if breg > 0
	void BLZ(unsigned char breg, unsigned short addr); //set pc to addr if breg < 0
};

#endif