#ifndef __NEL_H
#define __NEL_H

#include <cstdio>
#include "util.h"

namespace nel {

enum op_t {
    Add,    // regs[0] regs[1] regs[2]
    Mul,    // regs[0] regs[1] regs[2]
    Sub,    // regs[0] regs[1] regs[2]
    Div,    // regs[0] regs[1] regs[2]
    Load,   // regs[0] ints[0]
    Jump    // ints[0] regs[0] ints[1]
};

struct inst_t {
    op_t opr; 
    int regs[3];
    int ints[2];

    int inst_idx;
    
    void show() {
        switch (opr) {
            case Add: {
                printf("Add R%d R%d R%d\n", regs[0], regs[1], regs[2]);
                break;
            };
            case Mul: {
                printf("Mul R%d R%d R%d\n", regs[0], regs[1], regs[2]);
                break;
            };
            case Sub: {
                printf("Sub R%d R%d R%d\n", regs[0], regs[1], regs[2]);
                break;
            };
            case Div: {
                printf("Div R%d R%d R%d\n", regs[0], regs[1], regs[2]);
                break;
            };
            case Load: {
                printf("Load R%d 0x%x\n", regs[0], ints[0]);
                break;
            };
            case Jump: {
                printf("Jump 0x%x R%d 0x%x\n", ints[0], regs[0], ints[1]);
                break;
            };
            default: {
                printf("unknown opr: %d", (int)opr);
                break;
            }
        }
    }
};

struct inst_status_t {
    bool done = false;
    int issue;
    int exec_comp;
    int write_result;
};

};

#endif
