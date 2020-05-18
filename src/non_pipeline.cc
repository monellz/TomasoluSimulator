#include <vector>
#include "nel_parser.h"

using namespace parser;

int main(int argc, char** argv) {
    Parser parser;
    std::vector<nel::inst_t> insts;
    if (argc != 2) {
        printf("Usage: ./non_pipeline <#file_name>\n");
        return -1;
    }

    parser.parse(argv[1], insts);
    int total_cycle = 0;
    int regs[REG_NUM];
    for (int i = 0; i < REG_NUM; ++i) {
        regs[i] = 0;
    }
    int inst_idx = 0;
    while (true) {
        if (inst_idx >= insts.size()) break;
        switch (insts[inst_idx].opr) {
            case nel::Add: {
                regs[insts[inst_idx].regs[0]] = regs[insts[inst_idx].regs[1]] + regs[insts[inst_idx].regs[2]];
                total_cycle += ADD_CYCLE + 1; //one cycle for issue
                inst_idx++;
                break;
            };
            case nel::Sub: {
                regs[insts[inst_idx].regs[0]] = regs[insts[inst_idx].regs[1]] - regs[insts[inst_idx].regs[2]];
                total_cycle += SUB_CYCLE + 1;
                inst_idx++;
                break;
            };
            case nel::Mul: {
                regs[insts[inst_idx].regs[0]] = regs[insts[inst_idx].regs[1]] * regs[insts[inst_idx].regs[2]];
                total_cycle += MUL_CYCLE + 1;
                inst_idx++;
                break;
            };
            case nel::Div: {
                if (regs[insts[inst_idx].regs[2]] == 0) {
                    regs[insts[inst_idx].regs[0]] = regs[insts[inst_idx].regs[1]];
                    total_cycle += DIV_ZERO_CYCLE + 1;
                } else {
                    total_cycle += DIV_CYCLE + 1;
                    regs[insts[inst_idx].regs[0]] = regs[insts[inst_idx].regs[1]] / regs[insts[inst_idx].regs[2]];
                }
                inst_idx++;
                break;
            };
            case nel::Load: {
                regs[insts[inst_idx].regs[0]] = insts[inst_idx].ints[0];
                total_cycle += LOAD_CYCLE + 1;
                inst_idx++;
                break;
            };
            case nel::Jump: {
                if (regs[insts[inst_idx].regs[0]] == insts[inst_idx].ints[0]) {
                    //jump
                    inst_idx += insts[inst_idx].ints[1];
                } else {
                    inst_idx ++;
                }
                total_cycle += JUMP_CYCLE + 1;
                break;
            };
        }
    }

    printf("Regs:\n");
    for (int reg_start = 0; reg_start < 32; reg_start += 8) {
        printf("\t");
        for (int i = reg_start; i < reg_start + 8; ++i) {
            printf("(R%d)%x\t", i, regs[i]);
        }
        printf("\n");
    }

    printf("Non Pipeline Total Cycle: %d\n", total_cycle);
    return 0;
}