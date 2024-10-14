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
	std::ofstream fout( "output.bin", std::ios::app );

	fout.write( reinterpret_cast<const char*>(vec.data()), vec.size() );   // ii dam cast la char* pentru ca char = 1 byte 
	
	fout.close();
}

void manip::init_binary() {
	std::ofstream fout( "output.bin", std::ios::binary );
	fout.close();
}

bool manip::validate_constant( int Constant ) {
	return (Constant >=0 && Constant <=255);
}

int manip::find_index(const std::string &elem, const std::vector<std::string>& vec) {
	auto it = std::find(vec.begin(), vec.end(), elem);
	return (it != vec.end()) ? std::distance(vec.begin(), it) + 0x10 : -1;
}

int manip::validate_register( const std::string &Register) {
	std::vector<std::string> Regs = {"WR", "RA", "CR", "M", "A", "R"};
	
	try{
		int reg = std::stoi(Register);
		if(reg >=0 && reg <=15) return reg;
	} catch( const std::invalid_argument& arg ) {
		return manip::find_index(Register, Regs);   
	}
	return -1;
}
