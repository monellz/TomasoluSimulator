#ifndef __NEL_H
#define __NEL_H

#include <cstdio>
#include "util.h"

#define LOAD_CYCLE 3
#define JUMP_CYCLE 1
#define ADD_CYCLE 3
#define SUB_CYCLE 3
#define MUL_CYCLE 4
#define DIV_CYCLE 4
#define DIV_ZERO_CYCLE 1

namespace nel {

enum op_t {
    None = 0,
    Add = 1,    // regs[0] regs[1] regs[2]
    Mul = 2,    // regs[0] regs[1] regs[2]
    Sub = 3,    // regs[0] regs[1] regs[2]
    Div = 4,    // regs[0] regs[1] regs[2]
    Load = 5,   // regs[0] ints[0]
    Jump = 6    // ints[0] regs[0] ints[1]
};

const char op_name[][5] = {
    "none", "add", "mul", "sub", "div", "load", "jump"
};

struct inst_t {
    op_t opr; 
    int regs[3];
    int ints[2];
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
