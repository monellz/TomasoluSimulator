#ifndef __TOMASULO_H
#define __TOMASULO_H

#include <vector>

#include "nel.h"
#include "station.h"
#include "functional_unit.h"
#include "reg.h"

#define MRS_NUM 3
#define ARS_NUM 6
#define LOADBUFF_NUM 3
#define REG_NUM 32
#define ADD_NUM 3
#define MULTI_NUM 2
#define LOAD_NUM 2

#define LOAD_CYCLE 3
#define JUMP_CYCLE 1
#define ADD_CYCLE 3
#define SUB_CYCLE 3
#define MUL_CYCLE 4
#define DIV_CYCLE 4
#define DIV_ZERO_CYCLE 1

class Tomasulo {
public:
    typedef FUs<ADD_NUM> AddFUs_t;
    typedef FUs<MULTI_NUM> MultFUs_t;
    typedef FUs<LOAD_NUM> LoadFUs_t;

    OpStation ars[ARS_NUM]; 
    OpStation mrs[MRS_NUM];
    LoadBuffer load_buffer[LOADBUFF_NUM];

    reg_t regs[REG_NUM];

    AddFUs_t add_fus;
    MultFUs_t mult_fus;
    LoadFUs_t load_fus;

    Tomasulo();
    void run(std::vector<nel::inst_t>& insts);
    void reset();
};

#endif