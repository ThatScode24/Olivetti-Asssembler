#include "manip.h"

bool manip::validate_expression( const std::string &expression, char caracter ) {
	bool valid = ( expression.find(caracter) != std::string::npos );  // npos e char pentru not found
	return valid;
}

std::string manip::remove_occurences(std::string expression, char caracter) {
	expression.erase( std::remove(expression.begin(), expression.end(), caracter), expression.end() );
	return expression;
}

void manip::write_binary(const std::vector<std::uint8_t>& vec) {
	std::ofstream fout( "output.bin",  std::ios::out | std::ios::binary | std::ios::app);

	fout.write( reinterpret_cast<const char*>(vec.data()), vec.size() );   // ii dam cast la char* pentru ca char = 1 byte 
	
	fout.close();
}

void manip::init_binary() {
	std::ofstream fout( "output.bin", std::ios::out | std::ios::binary);
	fout.close();
}

bool manip::validate_constant( int Constant ) {
	return (Constant >=0 && Constant <=255);
}

int manip::find_index(const std::string &elem, const std::vector<std::string>& vec) {
	std::vector<std::string>::const_iterator it = std::find(vec.begin(), vec.end(), elem);
	return (it != vec.end()) ? std::distance(vec.begin(), it) + 0x10 : -1;
}

std::string manip::toUpperCase(std::string str) {
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
    return str;
}

int manip::validate_register( const std::string &Register) {
	std::vector<std::string> Regs = {"WR", "RA", "CR", "M", "A", "R"};

	try{
		int reg = std::stoi(Register);
		if(reg >=0 && reg <=15) return reg;
	} catch( const std::invalid_argument& arg ) {
		std::string final_reg = manip::toUpperCase(Register);
		return manip::find_index(final_reg, Regs);   
	}
	return -1;
}

std::string manip::removeComments(const std::string& linie) {
	std::string trim = manip::flameSpaces(linie);
	size_t pozitie = trim.find(';');
	return (pozitie != std::string::npos) ? trim.substr(0, pozitie) : trim;
}


void manip::printVector(const std::vector<std::uint8_t> &vec) {
	for(const std::uint8_t &num : vec) printf("     0x%X     ", num);        //       format exadecimal
	printf("\n");
}

bool manip::containsHexPrefix(const std::string& potential) { return (potential.find("0x") != std::string::npos || potential.find("0X") != std::string::npos); }

std::string manip::flameSpaces(const std::string& start) {
	size_t st = start.find_first_not_of(' ');
	if ( st == std::string::npos) return "";
	std::string::size_type end = start.find_last_not_of(' ');
    return start.substr(st, end - st + 1);
}