#include "nel_parser.h"
#include "cycle_counter.h"

using namespace parser;

int main(int argc, char** argv) {
    Parser parser;
    std::vector<nel::inst_t> insts;
    
    parser.parse("TestCase/0.basic.nel", insts);


    return 0;
}
