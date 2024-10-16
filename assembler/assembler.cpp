/*  

-> De adaugat suport pentru registri alfanumerici (dar si compatibilitati) exact cum e in Emulator.cpp in VS.
-> De adaugat suport pentru registri scris in litere mici (de ex, merge cu RA dar nu cu ra)                              OK
-> should try to minimise try catch blocks 
-> should implement all of the condition checking directly in the assembler
-> should include details about instructions in readme

Nume: Olivetti A5BAL8/P101-C6502


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
#include "manip.h"


int main(void) {
	std::ifstream file("code.txt");
	std::regex comma(",");    
	std::string line;          //   linie de cod 
	std::string word;                    //     buffer pentru fiecare element din token 

	short current_line = 1;     // numarul liniei de cod
	manip::init_binary();        //   cream o fila noua output.bin


	while(std::getline(file, line)) {
		if(line.empty()) continue;
		
		//         scoatem toate \n si \r care or exista
		
		line.erase(std::remove(line.begin(), line.end(), '\n'), line.end());
		line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
			
		std::string line_without_comma = std::regex_replace(line, comma, "");   //     scoatem virgula eventuala
		
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
				int Constant;
				if(Register == 0x10 || Register == 0x12) printf("Line %d: Cannot load integer into this register.\n", current_line);
				else if(Register != -1) {   // sa existe si sa nu fie WR si CR
					try {
						Constant = std::stoi( manip::remove_occurences(token[2], '$') );
						if( manip::validate_constant(Constant) ) {
							toWrite.push_back(0x00);    // opcode instructiune LI 
							toWrite.push_back(Register);
							toWrite.push_back(Constant);

							manip::write_binary(toWrite);

							//      printf("%d %d %d", toWrite[0], toWrite[1], toWrite[2]);
						} else printf("Line %d: Unsigned integer exceeds 8 bits.\n", current_line);
					} catch( const std::invalid_argument& arg ) {
						printf("Line %d: Expected an integer.\n", current_line);   // daca ajungem aici, nu era o constanta
					}
				} else printf("Line %d: Register does not exist.\n", current_line);
			} else printf("Line %d: Wrong/Missing prefix for Register/Constant.\n", current_line);   
		
		} else if( mnemonic == "PR" || mnemonic == "pr" ) {
			if ( manip::validate_expression(token[1], '%' ) ) {
				int Register = manip::validate_register( manip::remove_occurences(token[1], '%') );
				if( Register != -1) {
					toWrite.push_back(0x02);
					toWrite.push_back(Register);

					manip::write_binary(toWrite);
				} else printf("Line %d: Invalid Register.\n", current_line);
			} else printf("Line %d: Wrong/Missing prefix for Register/Constant.\n", current_line);
		
		} else if( mnemonic == "LAX" || mnemonic == "lax") {
			if( manip::validate_expression(token[1], '%') ) {
				int Register = manip::validate_register( manip::remove_occurences(token[1], '%') );
				if ( Register != -1 && Register<=15 ) {
					toWrite.push_back(0x03);
					toWrite.push_back(Register);

					manip::write_binary(toWrite);
				} else printf("Line %d: Cannot execute LAX instruction on this register.\n", current_line);
			} else printf("Line %d: Wrong/Missing prefix for Register/Constant.\n", current_line);
		
		} else if ( mnemonic == "SAX" || mnemonic == "sax" ) {
			if( manip::validate_expression(token[1], '%') ) {
				int Register = manip::validate_register( manip::remove_occurences(token[1], '%') );
				if ( Register != -1 && Register<=15 ) {
					toWrite.push_back(0x06);
					toWrite.push_back(Register);

					manip::write_binary(toWrite);
				} else printf("Line %d: Cannot execute SAX instruction on this register.\n", current_line);
			} else printf("Line %d: Wrong/Missing prefix for Register/Constant.\n", current_line);
		}

		else if ( mnemonic == "LRZ" || mnemonic == "lrz" ) {
			if ( manip::validate_expression(token[1], '~') ) {
				int Register_Count = std::stoi(manip::remove_occurences(token[1], '~'));
				
				if ( Register_Count <1 || Register_Count > 0x15) printf("Line %d: Invalid Register Logic.\n", current_line);
				else {
					toWrite.push_back(0x07);
					toWrite.push_back(Register_Count);

					manip::write_binary(toWrite);
				}
			} else printf("Line %d: Wrong/Missing prefix for Register Count.\n", current_line);
		}
		
		else if ( mnemonic == "ARI" || mnemonic == "ari" ) {
			if( manip::validate_expression(token[1], '%') || manip::validate_expression(token[2], '$') ) {
				int Register = manip::validate_register(manip::remove_occurences(token[1], '%'));
				int Constant; 
				if(Register != -1) {
					if(Register == 0x12 || Register == 0x10) printf("Line %d: Cannot execute ARI on this register.\n", current_line);
					else {
						try {
							Constant = std::stoi( manip::remove_occurences(token[2], '$') );
							if( manip::validate_constant(Constant) ) {
								toWrite.push_back(0x09);    // opcode instructiune ARI
								toWrite.push_back(Register);
								toWrite.push_back(Constant);

								manip::write_binary(toWrite);

								//      printf("%d %d %d", toWrite[0], toWrite[1], toWrite[2]);
							} else printf("Line %d: Unsigned integer exceeds 8 bits.\n", current_line);
						} catch( const std::invalid_argument& arg ) {
							printf("Line %d: Expected an integer.\n", current_line);   // daca ajungem aici, nu era o constanta
						}
					}
				} else printf("Line %d: Register does not exist.\n", current_line);

			} else printf("Line %d: Wrong/Missing prefix for Register/Constant.\n", current_line);
		}

		else if ( mnemonic == "LZ" || mnemonic == "lz") {
			if( manip::validate_expression(token[1], '%') && manip::validate_expression(token[2], '~') ) {
				int Register = manip::validate_register(manip::remove_occurences(token[1], '%'));
				if(Register != -1 || Register == 0x10 || Register == 0x12) {
					try {
						int Next = ( std::stoi( manip::remove_occurences(token[2], '~') ) ) > 15 ? -1 : std::stoi( manip::remove_occurences(token[2], '~') );
						if(Next == -1) printf("Line %d: Too many target registers.\n", current_line);
						else {
							toWrite.push_back(0x08);
							toWrite.push_back(Register);
							toWrite.push_back(Next);

							manip::write_binary(toWrite);
						}
					} catch(const std::invalid_argument& arg) {
						printf("Line %d: Expected an integer.\n", current_line);
					}
				} else printf("Line %d: Register does not exit/cannot execute LZ on this Register.\n", current_line);
			
			} else printf("Line %d: Wrong/Missing prefix for Register Count/Register.\n", current_line);
		}

		else if ( mnemonic == "SRI" || mnemonic == "sri" ) {
			if( manip::validate_expression(token[1], '%') || manip::validate_expression(token[2], '$') ) {
				int Register = manip::validate_register(manip::remove_occurences(token[1], '%'));
				int Constant; 
				if(Register != -1) {
					if(Register == 0x12 || Register == 0x10) printf("Line %d: Cannot execute ARI on this register.\n", current_line);
					else {
						try {
							Constant = std::stoi( manip::remove_occurences(token[2], '$') );
							if( manip::validate_constant(Constant) ) {
								toWrite.push_back(0x0B);    // opcode instructiune SRI
								toWrite.push_back(Register);
								toWrite.push_back(Constant);

								manip::write_binary(toWrite);

								//      printf("%d %d %d", toWrite[0], toWrite[1], toWrite[2]);
							} else printf("Line %d: Unsigned integer exceeds 8 bits.\n", current_line);
						} catch( const std::invalid_argument& arg ) {
							printf("Line %d: Expected an integer.\n", current_line);   // daca ajungem aici, nu era o constanta
						}
					}
				} else printf("Line %d: Register does not exist.\n", current_line);

			} else printf("Line %d: Wrong/Missing prefix for Register/Constant.\n", current_line);
		}
		current_line++;
	}
	printf("merge\n");
	return 0;
}


