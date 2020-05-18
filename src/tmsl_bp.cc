#include "nel_parser.h"
#include "cycle_counter.h"
#include "tomasulo.h"

using namespace parser;

int main(int argc, char** argv) {
    Parser parser;
    std::vector<nel::inst_t> insts;

    if (argc != 3 && argc != 4) {
        printf("Usage: ./tmsl_bp <#file_name> <#output_fname> <#print_cycle>\n");
        printf("       ./tmsl_bp <#file_name> <#output_fname>\n");
        return -1;
    }

    int print_cycle = -1;
    if (argc == 4)  print_cycle = atoi(argv[3]);

    parser.parse(argv[1], insts);

    Tomasulo tmsl_bp;
    tmsl_bp.run_bp(insts, print_cycle);
    //Record::get_instance().output(argv[2]);

    return 0;
}
