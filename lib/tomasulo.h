#ifndef __TOMASULO_H
#define __TOMASULO_H

#include <vector>

#include "nel.h"
#include "station.h"
#include "functional_unit.h"
#include "reg.h"
#include "record.h"
#include "reorder_buffer.h"
#include "branch_target_buffer.h"

#define MRS_NUM 3
#define ARS_NUM 6
#define LOADBUFF_NUM 3
#define ADD_NUM 3
#define MULTI_NUM 2
#define LOAD_NUM 2

class Tomasulo {
public:
    typedef FUs<ADD_NUM> AddFUs_t;
    typedef FUs<MULTI_NUM> MultFUs_t;
    typedef FUs<LOAD_NUM> LoadFUs_t;

    typedef OpStations<ARS_NUM> Ars_t;
    typedef OpStations<MRS_NUM> Mrs_t;
    typedef LoadBuffers<LOADBUFF_NUM> Loadbuffer_t;

    ROB rob;
    BTB btb;

    Ars_t ars; 
    Mrs_t mrs;
    Loadbuffer_t load_buffer;

    reg_t regs[REG_NUM];
    bool stall;

    AddFUs_t add_fus;
    MultFUs_t mult_fus;
    LoadFUs_t load_fus;

    Tomasulo();
    void regs_show();
    void run(std::vector<nel::inst_t>& insts, int print_cycle);
    void run_bp(std::vector<nel::inst_t>& insts, int print_cycle);
    void reset(int inst_num = 0);
};

#endif