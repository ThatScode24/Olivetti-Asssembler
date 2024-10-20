#pragma once
#include "memory.h"

using Byte = unsigned char;   // 8 biti
using Word = unsigned short;  // 16 biti

struct CPU {
	//registri

	Word PC;  // PC
	Word SP; // stack pointer
	Byte CR; // registrul de flaguri
	Byte WR; // Write Register
	Byte RA; // Registru Auxiliar

	Byte M, A, R; // Registri I/O 

	Byte R_0, R_1, R_2, R_3, R_4, R_5, R_6, R_7, R_8, R_9, R_10, R_11, R_12, R_13, R_14, R_15; // cei 16 registri

	//Flags 

	Byte C : 1;   // Carry Flag - overflow or underflow
	Byte Z : 1;   // Zero Flag  
	Byte V : 1;   //Overflow Flag
	Byte N : 1;   // Negative Flag

	//opcodes instructiuni 
	enum Instructions {
		INSTRUCTION_LI =  0x00, //      3 cycles
		INSTRUCTION_RMC = 0x01,  //     4 cycles
		INSTRUCTION_PR =  0x02,  //     2 cycles 
		INSTRUCTION_LAX = 0x03,  //     2 cycles 
		INSTRUCTION_LCR = 0x04,   //    3 cycles
		INSTRUCTION_LR =  0x05,   //    3 cycles
		INSTRUCTION_SAX = 0x06,   //    2 cycles
		INSTRUCTION_LRZ = 0x07,    //   2 cycles
 		INSTRUCTION_LZ =  0x08,     //  3 cycles 
		INSTRUCTION_ARI = 0x09,     //  3 cycles 
		INSTRUCTION_AR =  0x0A,     //  3 cycles 
		INSTRUCTION_SRI = 0x0B,     //  3 cycles
		INSTRUCTION_SR  = 0x0C,      //  3 cycles  
		INSTRUCTION_FD =  0x0D,       // 2 cycles
		INSTRUCTION_KAC = 0x0E        // 2 cycles ??
	};

	//opcodes registri
	enum Registers {
		REG_0  = 0x00,
		REG_1  = 0x01,
		REG_2  = 0x02,
		REG_3  = 0x03,
		REG_4  = 0x04,
		REG_5  = 0x05,
		REG_6  = 0x06,
		REG_7  = 0x07,
		REG_8  = 0x08,
		REG_9  = 0x09,
		REG_10 = 0x0A,
		REG_11 = 0x0B,
		REG_12 = 0x0C,
		REG_13 = 0x0D,
		REG_14 = 0x0E,
		REG_15 = 0x0F,
		REG_WR = 0x10,
		REG_RA = 0x11,
		REG_CR = 0x12,
		REG_M  = 0x13,
		REG_A  = 0x14,
		REG_R  = 0x15
	};
	
	void Reset(Memory& memorie);
	Byte Fetch(unsigned int& NumCycles, Memory& memorie);
	Byte Read(unsigned int& NumCycles, Byte Address, Memory& memorie);
	void SET_FLAGS_LI(Byte Register);
	void Exec(unsigned int NumCycles, Memory& memorie);
	void Debug();
};