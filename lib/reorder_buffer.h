#ifndef __REORDER_BUFFER_H
#define __REORDER_BUFFER_H

#include "nel.h"
#include "reg.h"
#include "record.h"
#include "cycle_counter.h"
#include <list>

struct ReorderBuffer {
    nel::op_t op;
    int write_reg;

    ReservationStation* rs;
    int predict_result;

    int result;
    bool ready;

    int inst_idx;
};

typedef std::list<ReorderBuffer>::iterator rob_iter;

class ROB {
public:
    std::list<ReorderBuffer> buf;
    ROB();
    void reset(reg_t regs[]);
    rob_iter push(nel::op_t op, int write_reg);

    int vregs[REG_NUM];

    //true: do noting, false: need reset all(due to jump fail)
    bool commit(reg_t regs[]);
    void show();
    int get_reg_value(int reg_idx);
    void update_vregs(int reg_idx, int value);
};

#endif