/*  

-> De adaugat suport pentru registri alfanumerici (dar si compatibilitati) exact cum e in Emulator.cpp in VS.                OK
-> De adaugat suport pentru registri scris in litere mici (de ex, merge cu RA dar nu cu ra)                                  OK
-> SHOULD try to minimise try catch blocks 
-> SHOULD implement all of the condition checking directly in the assembler                                                  OK
-> SHOULD include details about instructions in readme               

Probleme

-> de fiecare data cand e 0xA in toWrite, asta adauga un 0xD inainte                                                         OK
era problema cu modul de deschidere a fisierului, il deschideam si la inceput si la append in text mode 					 
-> Daca eu o linie de comment, ea este eliminata in vectorul care omite comentariile, dar                                    OK
linia de eroare (daca apare una) este eronata. 
-> Daca e o linie invalida / doar un spatiu => assertion failure!                                                            OK
-> std::invalid_argument daca sunt in mod exadecimal si e o problema cu sintaxa constantei (si are 0x in ea).                OK

Nume: Olivetti A5BAL8/P101-C6502

De schimbat:

->culoarea de la DE1 si DE2 sa fie mov, adica sa nu se califice ca o instructiune.                                           OK

Asamblarea se face in 2 etape:                                                                                               OK

-> prima scoate comentariile/liniile goale si pune liniile curate intr-un vector.
-> a doua oara se proceseaza liniile si se scrie codul masina in fisierul binar.

cmake --build .
*/

#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <vector>
#include <sstream>
#include <algorithm>
#include <stdio.h>
#include <utility>
#include "manip.h"



int main(void) {
	std::ifstream file("code.olvasm");
	std::regex comma(",");    
	std::string linie;          //   linie de cod 
	std::string word;                    //     buffer pentru fiecare element din token 

	std::vector<std::pair<std::string, int>> fara_com = {};

	int current_line = 1;     // numarul liniei de cod
	manip::init_binary();        //   cream o fila noua output.bin

	while(std::getline(file, linie)) {
		std::string linie_curata = manip::removeComments(linie);
		if(!linie_curata.empty()) fara_com.emplace_back(linie_curata, current_line);
		current_line++;
	}

	bool numberMode = 0;
	    
	for( std::pair<std::string,int>& line : fara_com ){
		if(line.first.empty()) continue;
		//         scoatem toate \n si \r care or exista
		
		line.first.erase(std::remove(line.first.begin(), line.first.end(), '\n'), line.first.end());
		line.first.erase(std::remove(line.first.begin(), line.first.end(), '\r'), line.first.end());
			
		std::string line_without_comma = std::regex_replace(line.first, comma, "");   //     scoatem virgula eventuala
		
		//       cu asta separam instructiunea intr-un vector 	
		std::istringstream iss(line_without_comma);

		std::vector<std::string> token;     // aici o sa fie elementele din linie separate

		while ( iss >> word ) {
			token.push_back(word);    //      generam o lista dand split dupa spatiu
		}

		std::string mnemonic = token[0];    //     fiecare rand are pe prima pozitie mnemonic
	
		std::vector<std::uint8_t> toWrite = {};
		
		if( mnemonic == "LI" || mnemonic == "li" ) {
			if( manip::validate_expression(token[1], '%') && manip::validate_expression(token[2], '$') ) {
				int Register = manip::validate_register( manip::remove_occurences(token[1], '%') );		
				if(Register == 0x10 || Register == 0x12) printf("Line %d: Cannot load integer into this register.\n", line.second);
				else if(Register != -1) {   // sa existe si sa nu fie WR si CR
					if( numberMode ) {
						if(manip::containsHexPrefix(token[2])) printf("Line %d: Invalid constant format for this number mode.\n", line.second);
						else {
							try {
								int Constant = std::stoi( manip::remove_occurences(token[2], '$') );
								if( manip::validate_constant(Constant) ) {
									toWrite.push_back(0x00);    // opcode instructiune LI 
									toWrite.push_back(Register);
									toWrite.push_back(Constant);

									manip::write_binary(toWrite);

									manip::printVector(toWrite);

									//      printf("%d %d %d", toWrite[0], toWrite[1], toWrite[2]);
								} else printf("Line %d: Unsigned integer exceeds 8 bits.\n", line.second);
							} catch( const std::invalid_argument& arg ) {
								printf("Line %d: Expected an integer.\n", line.second);   // daca ajungem aici, nu era o constanta
							}
						}
					} else {   // handle hexa mode
						if(!manip::containsHexPrefix(token[2])) printf("Line %d: Invalid constant format for this number mode.\n", line.second);
						else {
							try{
								int numBaseTen = std::stoi(manip::remove_occurences(token[2], '$'), nullptr, 16);
								if( manip::validate_constant(numBaseTen) ) {
									toWrite.push_back(0x00);    // opcode instructiune LI 
									toWrite.push_back(Register);
									toWrite.push_back(numBaseTen);
									
									manip::write_binary(toWrite);
									manip::printVector(toWrite);
								} else printf("Line %d: Unsigned integer exceeds 8 bits.\n", line.second);
							} catch( const std::invalid_argument &arg ) {
								printf("Line %d: Expected an integer.\n", line.second);
							}
						}
					}
				} else printf("Line %d: Register does not exist.\n", line.second);
			} else printf("Line %d: Wrong/Missing prefix for Register/Constant.\n", line.second);   
		
		} else if( mnemonic == "PR" || mnemonic == "pr" ) {
			if ( manip::validate_expression(token[1], '%' ) ) {
				int Register = manip::validate_register( manip::remove_occurences(token[1], '%') );
				if( Register != -1) {
					toWrite.push_back(0x02);
					toWrite.push_back(Register);

					manip::write_binary(toWrite);
					manip::printVector(toWrite);
				} else printf("Line %d: Invalid Register.\n", line.second);
			} else printf("Line %d: Wrong/Missing prefix for Register/Constant.\n", line.second);
		
		} else if( mnemonic == "LAX" || mnemonic == "lax") {
			if( manip::validate_expression(token[1], '%') ) {
				int Register = manip::validate_register( manip::remove_occurences(token[1], '%') );
				if ( Register != -1 && Register<=15 ) {
					toWrite.push_back(0x03);
					toWrite.push_back(Register);

					manip::write_binary(toWrite);
					manip::printVector(toWrite);
				} else printf("Line %d: Cannot execute LAX instruction on this register.\n", line.second);
			} else printf("Line %d: Wrong/Missing prefix for Register/Constant.\n", line.second);
		
		} else if ( mnemonic == "SAX" || mnemonic == "sax" ) {
			if( manip::validate_expression(token[1], '%') ) {
				int Register = manip::validate_register( manip::remove_occurences(token[1], '%') );
				if ( Register != -1 && Register<=15 ) {
					toWrite.push_back(0x06);
					toWrite.push_back(Register);

					manip::write_binary(toWrite);
					manip::printVector(toWrite);
				} else printf("Line %d: Cannot execute SAX instruction on this register.\n", line.second);
			} else printf("Line %d: Wrong/Missing prefix for Register/Constant.\n", line.second);
		}

		else if ( mnemonic == "LRZ" || mnemonic == "lrz" ) {
			if ( manip::validate_expression(token[1], '~') ) {
				int Register_Count = std::stoi(manip::remove_occurences(token[1], '~'));
				
				if ( Register_Count <1 || Register_Count > 0x14) printf("Line %d: Invalid Register Count Logic.\n", line.second);
				else {
					toWrite.push_back(0x07);
					toWrite.push_back(Register_Count);

					manip::write_binary(toWrite);

					manip::printVector(toWrite);
				}
			} else printf("Line %d: Wrong/Missing prefix for Register Count.\n", line.second);
		}
		
		else if ( mnemonic == "ARI" || mnemonic == "ari" ) {
			if( manip::validate_expression(token[1], '%') && manip::validate_expression(token[2], '$') ) {
				int Register = manip::validate_register(manip::remove_occurences(token[1], '%'));
				int Constant; 
				if(Register != -1) {
					if(Register == 0x12 || Register == 0x10) printf("Line %d: Cannot execute ARI on this register.\n", line.second);
					else {
						if(numberMode) {
							if(manip::containsHexPrefix(token[2])) printf("Line %d: Invalid constant format for this number mode.\n", line.second);
							else {
								try {
									Constant = std::stoi( manip::remove_occurences(token[2], '$') );
									if( manip::validate_constant(Constant) ) {
										toWrite.push_back(0x09);    // opcode instructiune ARI
										toWrite.push_back(Register);
										toWrite.push_back(Constant);

										manip::write_binary(toWrite);
										manip::printVector(toWrite);

										//      printf("%d %d %d", toWrite[0], toWrite[1], toWrite[2]);
									} else printf("Line %d: Unsigned integer exceeds 8 bits.\n", line.second);
								} catch( const std::invalid_argument& arg ) {
									printf("Line %d: Expected an integer.\n", line.second);   // daca ajungem aici, nu era o constanta
								}
							}
						} else {
							if(!manip::containsHexPrefix(token[2])) printf("Line %d: Invalid constant format for this number mode.\n", line.second);
							else {
								try {
									int numBaseTen = std::stoi(manip::remove_occurences(token[2], '$'), nullptr, 16);
									if( manip::validate_constant(numBaseTen) ) {

										toWrite.push_back(0x09);    // opcode instructiune ARI
										toWrite.push_back(Register);
										toWrite.push_back(numBaseTen);

										manip::write_binary(toWrite);
										manip::printVector(toWrite);
									} else printf("Line %d: Unsigned integer exceeds 8 bits.\n", line.second);
								} catch( const std::invalid_argument& arg ) {
									printf("Line %d: Expected an integer.\n", line.second);
								}
							}
						}
					}
				} else printf("Line %d: Register does not exist.\n", line.second);

			} else printf("Line %d: Wrong/Missing prefix for Register/Constant.\n", line.second);
		}

		else if ( mnemonic == "LZ" || mnemonic == "lz") {
			if( manip::validate_expression(token[1], '%') && manip::validate_expression(token[2], '~') ) {
				int Register = manip::validate_register(manip::remove_occurences(token[1], '%'));
				if(Register != -1 || Register == 0x10 || Register == 0x12) {
					try {
						int Next = ( std::stoi( manip::remove_occurences(token[2], '~') ) ) > 15 ? -1 : std::stoi( manip::remove_occurences(token[2], '~') );
						if(Next == -1) printf("Line %d: Too many target registers.\n", line.second);
						else {
							toWrite.push_back(0x08);
							toWrite.push_back(Register);
							toWrite.push_back(Next);

							manip::write_binary(toWrite);
							manip::printVector(toWrite);
						}
					} catch(const std::invalid_argument& arg) {
						printf("Line %d: Expected an integer.\n", line.second);
					}
				} else printf("Line %d: Register does not exit/cannot execute LZ on this Register.\n", line.second);
			
			} else printf("Line %d: Wrong/Missing prefix for Register Count/Register.\n", line.second);
		}

		else if( mnemonic == "SR" || mnemonic == "sr") {
			if( manip::validate_expression(token[1], '%') && manip::validate_expression(token[2], '%') ) {
				int Register1 = manip::validate_register( manip::remove_occurences(token[1], '%'));
				int Register2 = manip::validate_register( manip::remove_occurences(token[2], '%'));

				if(Register1 == -1 || Register2 == -1) printf("Line %d: Register not found.\n", line.second);
				else {
					if ((Register1 >= 0 && Register1 <= 15 && Register2 >= 0 && Register2 <= 15) ||
					((Register1 == 0x11 || Register1 == 0x15 || Register1 == 0x14 || Register1 == 0x13) &&
					(Register2 == 0x11 || Register2 == 0x15 || Register2 == 0x14 || Register2 == 0x13))) {
						toWrite.push_back(0xC);
						toWrite.push_back(Register1);
						toWrite.push_back(Register2);
						manip::write_binary(toWrite);
						manip::printVector(toWrite);
					} else printf("Line %d: Incorrect combination of registers.\n", line.second);
				}
			} else printf("Line %d: Wrong/Missing prefix for Register.\n", line.second);
		}

		else if( mnemonic == "AR" || mnemonic == "ar") {
			if( manip::validate_expression(token[1], '%') && manip::validate_expression(token[2], '%') ) {
				int Register1 = manip::validate_register( manip::remove_occurences(token[1], '%'));
				int Register2 = manip::validate_register( manip::remove_occurences(token[2], '%'));

				if(Register1 == -1 || Register2 == -1) printf("Line %d: Register not found.\n", line.second);
				else {
					if ((Register1 >= 0 && Register1 <= 15 && Register2 >= 0 && Register2 <= 15) ||
					((Register1 == 0x11 || Register1 == 0x15 || Register1 == 0x14 || Register1 == 0x13) &&
					(Register2 == 0x11 || Register2 == 0x15 || Register2 == 0x14 || Register2 == 0x13))) {
						toWrite.push_back(0xA);
						toWrite.push_back(Register1);
						toWrite.push_back(Register2);
						manip::write_binary(toWrite);
						manip::printVector(toWrite);
					} else printf("Line %d: Incorrect combination of registers.\n", line.second);
				}
			} else printf("Line %d: Wrong/Missing prefix for Register.\n", line.second);
		}

		else if( mnemonic == "LR" || mnemonic == "lr") {
			if( manip::validate_expression(token[1], '%') && manip::validate_expression(token[2], '%') ) {
				int Register1 = manip::validate_register( manip::remove_occurences(token[1], '%'));
				int Register2 = manip::validate_register( manip::remove_occurences(token[2], '%'));

				if(Register1 == -1 || Register2 == -1) printf("Line %d: Register not found.\n", line.second);
				else {
					if ( (Register1 >= 0 && Register1 <= 15 && Register2 >= 0 && Register2 <= 15) || 
					((Register1 == 0x11 || Register1 == 0x15 || Register1 == 0x14 || Register1 == 0x13) &&
				 	(Register2 == 0x11 || Register2 == 0x15 || Register2 == 0x14 || Register2 == 0x13))) {
						toWrite.push_back(0x5);
						toWrite.push_back(Register1);
						toWrite.push_back(Register2);

						manip::write_binary(toWrite);

						manip::printVector(toWrite);
					} else printf("Line %d: Incorrect combination of registers.\n", line.second);
				}
			} else printf("Line %d: Wrong/Missing prefix for Register.\n", line.second);
		}

		else if ( mnemonic == "SRI" || mnemonic == "sri" ) {
			if( manip::validate_expression(token[1], '%') && manip::validate_expression(token[2], '$') ) {
				int Register = manip::validate_register(manip::remove_occurences(token[1], '%'));
				int Constant; 
				if(Register != -1) {
					if(Register == 0x12 || Register == 0x10) printf("Line %d: Cannot execute ARI on this register.\n", line.second);
					else {
						if( numberMode) {
							if(manip::containsHexPrefix(token[2])) printf("Line %d: Invalid constant format for this number mode.\n", line.second);
							else {
								try {
									Constant = std::stoi( manip::remove_occurences(token[2], '$') );
									if( manip::validate_constant(Constant) ) {
										toWrite.push_back(0x0B);    // opcode instructiune SRI
										toWrite.push_back(Register);
										toWrite.push_back(Constant);

										manip::write_binary(toWrite);
										manip::printVector(toWrite);
										//      printf("%d %d %d", toWrite[0], toWrite[1], toWrite[2]);
									} else printf("Line %d: Unsigned integer exceeds 8 bits.\n", line.second);
								} catch( const std::invalid_argument& arg ) {
									printf("Line %d: Expected an integer.\n", line.second);   // daca ajungem aici, nu era o constanta
								}
							}
						} else {
							if(!manip::containsHexPrefix(token[2])) printf("Line %d: Invalid constant format for this number mode.\n", line.second);
							else {
								try{
									int numBaseTen = std::stoi(manip::remove_occurences(token[2], '$'), nullptr, 16);
									if( manip::validate_constant(numBaseTen) ) {

										toWrite.push_back(0x0B);    // opcode instructiune ARI
										toWrite.push_back(Register);
										toWrite.push_back(numBaseTen);

										manip::write_binary(toWrite);
										manip::printVector(toWrite);
									} else printf("Line %d: Unsigned integer exceeds 8 bits.\n", line.second);
								} catch( const std::invalid_argument& arg) {
									printf("Line %d: Expected an integer.\n", line.second);
								}
							}
						}
					}
				} else printf("Line %d: Register does not exist.\n", line.second);

			} else printf("Line %d: Wrong/Missing prefix for Register/Constant.\n", line.second);
		}

		else if( mnemonic == "LCR" || mnemonic == "lcr" ) {
			if( manip::validate_expression(token[1], '%') && manip::validate_expression(token[2], '%') ) {
				int Register1 = manip::validate_register(manip::remove_occurences(token[1], '%'));
				int Register2 = manip::validate_register(manip::remove_occurences(token[2], '%'));

				if(Register1 == -1 || Register2 == -1) printf("Line %d: Register not found.\n", line.second);
				else {
					if ((Register1 >= 0 && Register1 <= 15 && Register2 >= 0 && Register2 <= 15) ||
					((Register1 == 0x11 || Register1 == 0x15 || Register1 == 0x14 || Register1 == 0x13) &&
					(Register2 == 0x11 || Register2 == 0x15 || Register2 == 0x14 || Register2 == 0x13))) {
						toWrite.push_back(0x04);
						toWrite.push_back(Register1);
						toWrite.push_back(Register2);

						manip::write_binary(toWrite);
						manip::printVector(toWrite);

					} else printf("Line %d: Wrong Register logic.\n", line.second);
				}
			} else printf("Line %d: Wrong/Missing prefix for Register.\n", line.second);
		}

		else if( mnemonic == "KAC" || mnemonic == "kac") {
			if( manip::validate_expression(token[1], '&') ) {
				int system_pointer = std::stoi( manip::remove_occurences(token[1], '&') );
				if(system_pointer == 0x1 || system_pointer == 0x2) {
					toWrite.push_back(0x0E);
					toWrite.push_back(system_pointer);
					manip::write_binary(toWrite);
					manip::printVector(toWrite);
				} else printf("Line %d: Wrong System Pointer Number.\n", line.second);
			} else printf("Line %d: Wrong/Missing prefix for System Pointer.\n", line.second);
		}

		else if( mnemonic == "FD" || mnemonic == "fd" ) {
			if( manip::validate_expression(token[1], '$')) {
				if( numberMode ) {
					if(manip::containsHexPrefix(token[1])) printf("Line %d: Invalid constant format for this number mode.\n", line.second);
					else {
						try{
							int Constante = std::stoi( manip::remove_occurences(token[1], '$' ) );
							if( manip::validate_constant(Constante) ) {
								toWrite.push_back(0x0D);
								toWrite.push_back(Constante);

								manip::write_binary(toWrite);
								manip::printVector(toWrite);
							} else printf("Line %d: Unsigned integer exceeds 8 bits.\n", line.second);
						} catch( const std::invalid_argument &arg ) {
							printf("Line %d: Expected an integer.\n", line.second); 
						}
					}
				} else {
					if(!manip::containsHexPrefix(token[1])) printf("Line %d: Invalid constant format for this number mode.\n", line.second);
					else {
						try {
							int numBaseTen = std::stoi(manip::remove_occurences(token[1], '$'), nullptr, 16);
							if( manip::validate_constant(numBaseTen) ) {

								toWrite.push_back(0x0D);    // opcode instructiune FD
								toWrite.push_back(numBaseTen);

								manip::write_binary(toWrite);
								manip::printVector(toWrite);
							} else printf("Line %d: Unsigned integer exceeds 8 bits.\n", line.second);	
						} catch( const std::invalid_argument& arg) {
							printf("Line %d: Expected an integer.\n", line.second);
						}
					}			
				}
			} else printf("Line %d: Incorrect/Missing prefix for constant.\n", line.second);
		}

		else if ( mnemonic == "HLT" || mnemonic == "hlt" ) {
			toWrite.push_back(0x11);
			manip::write_binary(toWrite);
			manip::printVector(toWrite);
		}

		else if( mnemonic == "ON" || mnemonic == "on") {
			if(manip::validate_expression(token[1], '$')) {
				try{
					int Constante = std::stoi( manip::remove_occurences(token[1], '$' ) );
					if(Constante>=0 && Constante <=8) {
						toWrite.push_back(0x12);
						toWrite.push_back(Constante);
						manip::write_binary(toWrite);
						manip::printVector(toWrite);
					} else printf("Line %d: Expected a value between 0 and 8.\n", line.second);
				} catch(const std::invalid_argument& arg) {
					printf("Line %d: Expected an integer.\n", line.second); 
				}
			}
		}

		else if( mnemonic == "OFF" || mnemonic == "off") {
			if(manip::validate_expression(token[1], '$')) {
				try{
					int Constante = std::stoi( manip::remove_occurences(token[1], '$' ) );
					if(Constante>=0 && Constante <=8) {
						toWrite.push_back(0x14);
						toWrite.push_back(Constante);
						manip::write_binary(toWrite);
						manip::printVector(toWrite);
					} else printf("Line %d: Expected a value between 0 and 8.\n", line.second);
				} catch(const std::invalid_argument& arg) {
					printf("Line %d: Expected an integer.\n", line.second); 
				}
			}
		}

		else if( mnemonic == "FFO1" || mnemonic == "ffo1" ) {
			numberMode = 0;
			printf("Number mode set to hexadecimal.\n\n");
		}

		else if( mnemonic == "KEP" || mnemonic == "kep" ) {
			toWrite.push_back(0x0F);
			manip::write_binary(toWrite);
			manip::printVector(toWrite);
			printf("Output mode set to Terminal.\n\n");
		}

		else if( mnemonic == "KES" || mnemonic == "kes" ) {
			toWrite.push_back(0x10);
			manip::write_binary(toWrite);
			manip::printVector(toWrite);
			printf("Output mode set to Olivetti Display.\n");
		}

		else if( mnemonic == "STIO" || mnemonic == "stio" ) {      //     pentru assembler, mai incolo
			printf("Start of Program.\n");
		}
		else if( mnemonic == "END" || mnemonic == "end" ) {      //     pentru assembler, mai incolo
			toWrite.push_back(0x13);
			manip::write_binary(toWrite);
			manip::printVector(toWrite);
			printf("\nEnd of Program.\n");
		}

		else if( mnemonic == "FFO2" || mnemonic == "ffo2" ) {
			numberMode = 1;
			printf("Number mode set to base 10.\n\n");
		} 
		
		else if( mnemonic == "TITLE" || mnemonic == "title" ) {
			size_t first =  token[1].find('\'');
			size_t second = token[1].find('\'');

			if( first != std::string::npos && second != std::string::npos ) {
				std::cout << "Assembling '" << token[1].substr(first+1, second-first-1) << std::endl;
			} else printf("Line %d: Syntax error for title.\n", current_line);
		}
		else std::cout<< "Line " << line.second << ": <" << line.first << "> Syntax Error." << std::endl;
	}
	return 0;
}


