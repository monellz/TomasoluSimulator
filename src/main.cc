#include "nel_parser.h"
#include "cycle_counter.h"
#include "tomasulo.h"

using namespace parser;

int main(int argc, char** argv) {
    Parser parser;
    std::vector<nel::inst_t> insts;
    
    parser.parse("TestCase/1.basic.nel", insts);
    printf("parse done, insts.len = %lu\n", insts.size());

    Tomasulo tmsl;
    //tmsl.run_bp(insts);
    tmsl.run(insts);

    return 0;
}
