#include "nel_parser.h"

using namespace parser;

std::vector<std::string> Parser::split(const std::string& s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, std::back_inserter(elems));
    return elems;
}

bool Parser::parse(const std::string& file_path, std::vector<nel::inst_t>& insts) {
    std::ifstream in(file_path);

    if (!in.is_open()) {
        std::cout << file_path << " cannot open" << std::endl;
        return false;
    }

    std::string data;
    while (in >> data) {
        //std::cout << data << std::endl;
        std::vector<std::string> elems = split(data, ',');
        nel::inst_t inst;

        //std::cout << "original: " << data << std::endl;
        if (elems[0] == "ADD") {
            assert(elems.size() == 4);
            inst.opr = nel::Add;
            inst.regs[0] = (int)std::stoul(elems[1].substr(1));
            inst.regs[1] = (int)std::stoul(elems[2].substr(1));
            inst.regs[2] = (int)std::stoul(elems[3].substr(1));
        } else if (elems[0] == "MUL") {
            assert(elems.size() == 4);
            inst.opr = nel::Mul;
            inst.regs[0] = (int)std::stoul(elems[1].substr(1));
            inst.regs[1] = (int)std::stoul(elems[2].substr(1));
            inst.regs[2] = (int)std::stoul(elems[3].substr(1));
        } else if (elems[0] == "SUB") {
            assert(elems.size() == 4);
            inst.opr = nel::Sub;
            inst.regs[0] = (int)std::stoul(elems[1].substr(1));
            inst.regs[1] = (int)std::stoul(elems[2].substr(1));
            inst.regs[2] = (int)std::stoul(elems[3].substr(1));
        } else if (elems[0] == "DIV") {
            assert(elems.size() == 4);
            inst.opr = nel::Div;
            inst.regs[0] = (int)std::stoul(elems[1].substr(1));
            inst.regs[1] = (int)std::stoul(elems[2].substr(1));
            inst.regs[2] = (int)std::stoul(elems[3].substr(1));
        } else if (elems[0] == "LD") {
            assert(elems.size() == 3);
            inst.opr = nel::Load;
            inst.regs[0] = (int)std::stoul(elems[1].substr(1));
            inst.ints[0] = (int)std::stoul(elems[2], 0, 16);
        } else if (elems[0] == "JUMP") {
            assert(elems.size() == 4);
            inst.opr = nel::Jump;
            inst.ints[0] = (int)std::stoul(elems[1], 0, 16);
            inst.regs[0] = (int)std::stoul(elems[2].substr(1));
            inst.ints[1] = (int)std::stoul(elems[3], 0, 16);
        } else {
            std::cout << "unknown opr: " << elems[0] << std::endl;
            std::cout << "\tline: " << data << std::endl;
            return false;
        }
        //inst.show();
        insts.push_back(inst);
    }
    return true;
}
