#ifndef __REORDER_BUFFER_H
#define __REORDER_BUFFER_H

#include "nel.h"
#include "reg.h"
#include <list>

struct ReorderBuffer {
    nel::op_t op;
    int write_reg;

    ReservationStation* rs;
    int predict_result;

    int result;
    bool ready;
};

typedef std::list<ReorderBuffer>::iterator rob_iter;

class ROB {
public:
    std::list<ReorderBuffer> buf;
    ROB();
    void reset();
    rob_iter push(nel::op_t op, int write_reg);

    //true: do noting, false: need reset all(due to jump fail)
    bool commit(reg_t regs[]);
    void show();
};

#endif