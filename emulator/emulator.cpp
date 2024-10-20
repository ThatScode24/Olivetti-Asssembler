/*	

. CPU::Debug() pentru a afisa continutul registrilor.


. Cateva precizari despre instructiuni.

																	========LCR=========

=>  Se pot interschimba registri 0, 1 ... 15 intre ei | RA, M, A, R intre ei dar !NU! se poate face LCR Registru_numeric, (RA, M, A, R).	
	Daca vrem, de pilda, sa interschimbam 2 si M, scriem in felul urmator:    (COD TESTAT) 

	machine code pentru el:           0x03 0x02 0x04 0x11 0x13 0x06 0x02 0x02 0x02 0x02 0x13   ( 11 cicluri )

				LAX 2        Mutam in RA valoarea din registrul 2
				LCR RA, M    Interschimbam registrul M si RA 
				SAX 2        Mutam in registrul 2 continutul registrului RA


																	========LAX========
										
=> LAX Load Auxiliar Register (RA)
=> Muta in Registrul Auxiliar (RA) valoarea continuta in RR, unde RR este un registru intre 0 si 15.

																	========SAX========
=> SAX Store Auxiliar Register (RA)
=> Muta valoare registrului RR in Registrul Auxiliar (RA), unde RR este un registru intre 0 si 15.

																	========LI=========

=> Load Integer  
=> se poate genera o constantra in oricare registru in afara de WR si CR.

																	========LRZ========

=> Load Register Zero (LRZ)
=> Sintaxa LRZ k, unde 0 <= k <=15  | Reseteaza k+1 registri. 

																	========LZ========
=> Load Zero 
=> Sintaxa LZ RR, nn unde 0 < RR <= 15 si nn 0 < nn << 15
=> Reseteaza registru RR si inca nn registri.

																	========ARI=======

=> Add Register Integer
=> Sintaxa ARI RR, k unde RR este orice registru in afara de CR si WR

																	========AR========
=> Add Register
=> Sintaxa AR RA, RB unde 0 <= RA <= RB <= 15 sau RA, RB apartin { M, A, R, RA } 

																	========FD=======

-> Genereaza K in WR 
*/


#include <stdio.h>
#include <stdlib.h>
#include "lib/memory.h"
#include "lib/CPU.h"
#include <iostream>


int main(void) {

	Memory memorie;
	CPU cpu;
	cpu.Reset(memorie);

	/*
	
	LI %10, $5    
	LI %15, $10
	AR %10, %15
	LR %15, %10
	LRZ ~14
	
	*/


	memorie[0xFFF0] = 0x0E;
	memorie[0xFFF1] = 0x01;    
	/*
	memorie[0xFFF2] = 0x05;
	memorie[0xFFF3] = 0x00;
	memorie[0xFFF4] = 0x0F;
	memorie[0xFFF5] = 0x0A;
	memorie[0xFFF6] = 0x0A;
	memorie[0xFFF7] = 0x0A;
	memorie[0xFFF8] = 0x0F;
	memorie[0xFFF9] = 0x05;
	memorie[0xFFFA] = 0x0F;
	memorie[0xFFFB] = 0x0A;
	memorie[0xFFFC] = 0x07;
	memorie[0xFFFD] = 0x0E;
	*/
	cpu.Exec(2, memorie);
	
	cpu.Debug();

	printf("\n");
	return 0;
}



