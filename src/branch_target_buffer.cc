#include "branch_target_buffer.h"

BTB::BTB() {
    reset();
}

void BTB::reset() {
    buf.clear();
}

int BTB::predict(int inst_idx) {
    for (int i = 0; i < buf.size(); ++i) {
        if (buf[i].inst_idx == inst_idx) {
            //match
            return buf[i].bht.predict();
        }
    }

    //not match
    printf("info: predict not match for inst: %d, insert\n", inst_idx);
    BTBLine line;
    line.inst_idx = inst_idx;
    int predict_addr = line.bht.predict();
    buf.push_back(line);
    return predict_addr;
}


void BTB::branch_succ(int inst_idx, int addr) {
    for (int i = 0; i < buf.size(); ++i) {
        if (buf[i].inst_idx == inst_idx) {
            buf[i].bht.succ();
            buf[i].bht.set_addr(addr);
            return;
        }
    }
    printf("error: not found branch (succ) for inst: %d\n", inst_idx);
}

void BTB::branch_fail(int inst_idx) {
    for (int i = 0; i < buf.size(); ++i) {
        if (buf[i].inst_idx == inst_idx) {
            buf[i].bht.fail();
            return;
        }
    }
    printf("error: not found branch (fail) for inst: %d\n", inst_idx);
}

void BTB::show() {
    printf("BranchTargetBuffer:\n");
    printf("\t\t%8s\t%8s\t%8s\n", "Inst", "Addr", "Table");
    for (int i = 0; i < buf.size(); ++i) {
        int inst_idx = buf[i].inst_idx;
        int addr = buf[i].bht.get_addr();
        unsigned int table = buf[i].bht.get_table();
        printf("\t\t%8d\t%8x\t%8u\n", inst_idx, addr, table);
    }
}
