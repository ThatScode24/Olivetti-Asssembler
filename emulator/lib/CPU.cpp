#include "CPU.h"
#include <stdio.h>

void CPU::Debug() {
	printf("                   =======================================Debug=======================================\n\n"
		"R_0: %d\nR_1: %d\nR_2: %d\nR_3: %d\nR_4: %d\nR_5: %d\nR_6: %d\nR_7: %d\n"
		"R_8: %d\nR_9: %d\nR_10: %d\nR_11: %d\nR_12: %d\nR_13: %d\nR_14: %d\nR_15: %d\n"
		"M: %d\nA: %d\nR: %d\nWR: %d\nRA: %d\nCR: %d\n",
		R_0, R_1, R_2, R_3, R_4, R_5, R_6, R_7, R_8, R_9,
		R_10, R_11, R_12, R_13, R_14, R_15, M, A, R, WR, RA, CR);
}


void CPU::Reset(Memory & memorie)      // Initializare
{
	PC = 0xFFF0;   // de inteles chestia asta
	SP = 0x0100;

	// flaguri la 0
	C = Z = V = N;   // am scos decimal flag 

	// registri la 0
	R_0 = R_1 = R_2 = R_3 = R_4 = R_5 = R_6 = R_7 = R_8 = R_9 = R_10 = R_11 = R_12 = R_13 = R_14 = R_15 = 0;

	M = A = R = WR = RA = CR = 0;
	

}

Byte CPU::Fetch(unsigned int& NumCycles, Memory& memorie)  // luam urmatoarea instrucitune din memorie
{
	Byte Data = memorie[PC];
	PC++;  // incrementam adresa in registru PC
	NumCycles--;
	return Data;
}

Byte CPU::Read(unsigned int& NumCycles, Byte Address, Memory& memorie)  // luam urmatoarea instrucitune din memorie
{
	Byte Data = memorie[Address];
	NumCycles--;
	return Data;
}

void CPU::SET_FLAGS_LI(Byte Register) {
	Z = (Register == 0);
	N = (Register & 0b10000000) > 0;
}

void CPU::Exec(unsigned int NumCycles, Memory& memorie)
{
	while (NumCycles > 0)
	{
		Byte Instruction = Fetch(NumCycles, memorie);   // determinam instructiunea urmatoare (instructiune = 1byte)

		Byte* Regs[] = { &R_0, &R_1, &R_2, &R_3, &R_4, &R_5, &R_6, &R_7, &R_8, &R_9, &R_10, &R_11, &R_12, &R_13, &R_14, &R_15, &WR, &RA, &CR, &M, &A, &R };

		switch (Instruction) {

		case INSTRUCTION_LI:                           //        Genera la constante k nel registro RR
		{
			Byte Register = Fetch(NumCycles, memorie);   // registrul cu care vom lucra
			Byte Constant = Fetch(NumCycles, memorie);  // constanta de care avem nevoie

			//   verificam ca registri sunt corecti in assembler pentru motive de performanta
			*Regs[Register] = Constant;
			SET_FLAGS_LI(Register);
		} break;

		case INSTRUCTION_RMC:                          //       Load into RR value at 0x....
		{	
			// functioneaza doar pentru M, A, R si 0...15
			Byte Register = Fetch(NumCycles, memorie);
			Byte Adresa = Fetch(NumCycles, memorie);
			Byte Valoare = Read(NumCycles, Adresa, memorie);

			if ((Register >= 0 && Register <= 15) || (Register == 0x13 || Register == 0x14 || Register == 0x15)) { 
				*Regs[Register] = Valoare; SET_FLAGS_LI(Register);
			}
		} break;

		case INSTRUCTION_PR:                           //        Stampa decimale di Registro RR.
		{											  //      putem printa continutul oricarui registru	 
			Byte Register = Fetch(NumCycles, memorie); if (Register >= 0 && Register <= 21) printf("%d", *Regs[Register]);
		} break;

		case INSTRUCTION_LAX:                         //        Trasferisce dal Registro RR (numerico) al Registro Ausiliario.
		{
			Byte Register = Fetch(NumCycles, memorie);
			RA = *Regs[Register]; SET_FLAGS_LI(Register);    // setem RR = Aux reg 
		} break;

		case INSTRUCTION_LCR:                         //         Scambia il contenuto di due registri.
		{
			Byte Register1 = Fetch(NumCycles, memorie);
			Byte Register2 = Fetch(NumCycles, memorie);

			if ((Register1 >= 0 && Register1 <= 15 && Register2 >= 0 && Register2 <= 15) ||
				((Register1 == 0x11 || Register1 == 0x15 || Register1 == 0x14 || Register1 == 0x13) &&
					(Register2 == 0x11 || Register2 == 0x15 || Register2 == 0x14 || Register2 == 0x13))) {
				Byte aux = *Regs[Register1];
				*Regs[Register1] = *Regs[Register2];
				*Regs[Register2] = aux;
			}
		} break;

		case INSTRUCTION_LR:                        //           Transferice, nel Registro RA, il Registro RB  (MOV)
		{
			Byte Register1 = Fetch(NumCycles, memorie);
			Byte Register2 = Fetch(NumCycles, memorie);

			*Regs[Register1] = *Regs[Register2];
		} break;

		case INSTRUCTION_SAX:                      //              Trasferisce dal Registro Ausiliario al Registro RR (numerico).
		{
			Byte Register = Fetch(NumCycles, memorie);
			*Regs[Register] = RA; SET_FLAGS_LI(Register);    // setem RR = Aux reg 
		} break;

		case INSTRUCTION_LRZ:                     //               Azzera nn + 1 registri.
		{
			Byte NumRegisters = Fetch(NumCycles, memorie);
			for (int i = 0; i <= NumRegisters; i++) *Regs[i] = 0;    //  Reseteaza NumRegisters+1 registri.
			Z = 0;                                                   //  Setam flagul Zero
		} break;                                                     // trebuie adaugate securitati aici 

		case INSTRUCTION_LZ:
		{
			Byte Register = Fetch(NumCycles, memorie);
			Byte Urmatori = Fetch(NumCycles, memorie);

			for (int i = Register; i <= Register + Urmatori; i++) {
				*Regs[i] = 0;
				if (i >= 15) break;
			}
		} break;

		case INSTRUCTION_ARI:                    //                 Addiziona k a RR (Risultato in RR)
		{
			Byte Register = Fetch(NumCycles, memorie);
			Byte Constant = Fetch(NumCycles, memorie);
			*Regs[Register] += Constant;
		} break;

		case INSTRUCTION_AR:
		{
			Byte Register1 = Fetch(NumCycles, memorie);
			Byte Register2 = Fetch(NumCycles, memorie);

			*Regs[Register1] += *Regs[Register2];
		} break;

		case INSTRUCTION_SRI:      // se pare ca exista probleme de underflow. (e normal, am definite Byte = unsigned char)
		{
			Byte Register = Fetch(NumCycles, memorie);
			Byte Integer = Fetch(NumCycles, memorie);
			*Regs[Register] -= Integer;
		} break; 

		case INSTRUCTION_SR:
		{
			Byte Register1 = Fetch(NumCycles, memorie);
			Byte Register2 = Fetch(NumCycles, memorie);

			*Regs[Register1] -= *Regs[Register2];
		}

		case INSTRUCTION_FD:
		{
			Byte Constante = Fetch(NumCycles, memorie);  
			WR = Constante;
		}
		case INSTRUCTION_KAC:
		{
			Byte NumeroPointer = Fetch(NumCycles, memorie);	 // aici o sa se implementeze logica cu cei 2 pointeri
			if(NumeroPointer == 1) {
				scanf("%d", &WR);
			} else {
				printf("De implementat scrierea in memoria pointata de catre pointer 2.\n");
			}
		} break;

		default: printf("Instruction %d not handled.", Instruction);
		}
	}
};