#ifndef __BRANCH_HISTORY_TABLE_H
#define __BRANCH_HISTORY_TABLE_H

#include <cstdio>

//two bit prediction
class BHT {
private:
    unsigned int table;
    int addr;
public:
    BHT();
    void reset();

    //-1: go on    0...: predicted jump addr
    int predict();
    void succ();
    void fail();
    void set_addr(int a);
    int get_addr();
    unsigned get_table();
};


#endif