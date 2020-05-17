#include "nel_parser.h"
#include "cycle_counter.h"
#include "tomasulo.h"

using namespace parser;

int main(int argc, char** argv) {
    Parser parser;
    std::vector<nel::inst_t> insts;

    if (argc != 2 && argc != 3) {
        printf("Usage: ./simulator <#file_name> <#print_cycle>\n");
        printf("       ./simulator <#file_name>\n");
        return -1;
    }

    int print_cycle = -1;
    if (argc == 3)  print_cycle = atoi(argv[2]);

    parser.parse(argv[1], insts);
    printf("parse done, insts.len = %lu\n", insts.size());

    Tomasulo tmsl;
    //tmsl.run_bp(insts, print_cycle);
    tmsl.run(insts, print_cycle);

    return 0;
}
