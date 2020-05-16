#ifndef __NEL_PARSER_H
#define __NEL_PARSER_H

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>
#include "nel.h"
#include "util.h"

namespace parser {
class Parser {
public:
    template<typename Out>
    void split(const std::string &s, char delim, Out result) {
        std::istringstream iss(s);
        std::string item;
        while (std::getline(iss, item, delim)) {
            *result++ = item;
        }
    }
    std::vector<std::string> split(const std::string& s, char delim); 
    bool parse(const std::string& file_path, std::vector<nel::inst_t>& insts);
};

};
#endif
