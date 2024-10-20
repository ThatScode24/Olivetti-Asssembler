#ifndef MANIP_H
#define MANIP_H

#include <string>
#include <vector>
#include <cstdint>
#include <sstream>
#include <algorithm>
#include <fstream>


namespace manip {
    bool validate_expression( const std::string &expression, char caracter );
    int validate_register( const std::string &Register );     //  ne returneaza opcode daca registru e valabil, daca nu ret val = -1
    bool validate_constant( int Constant );     
    std::string remove_occurences(std::string expression, char caracter);
    void write_binary(const std::vector<std::uint8_t>& vec);
    void init_binary();
    int find_index(const std::string &elem, const std::vector<std::string>& vec);
    std::string toUpperCase(std::string str);
    std::string removeComments(const std::string& linie);
    void printVector(const std::vector<std::uint8_t> &vec);
}

#endif
