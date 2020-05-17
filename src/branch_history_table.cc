#include "branch_history_table.h"
BHT::BHT() {
    reset();
}

void BHT::reset() {
    table = 0;
    addr = 0;
}

void BHT::succ() {
    switch (table) {
        case 1:
        case 2:
        case 3: table = 3; break;
        case 0: table = 1; break;
        default: printf("BHT succ() error table: %d\n", table); break;
    }
}

void BHT::fail() {
    switch (table) {
        case 0:
        case 1:
        case 2: table = 0; break;
        case 3: table = 1; break;
        default: printf("BHT fail() error table: %d\n", table); break;
    }

}

int BHT::predict() {
    if (((table >> 1) & 0x1) == 1) {
        //jump
        return addr;
    } else {
        return -1;
    }
}

void BHT::set_addr(int a) {
    addr = a;
}

int BHT::get_addr() {
    return addr;
}

unsigned int BHT::get_table() {
    return table;
}