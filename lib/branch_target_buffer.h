#ifndef __BRANCH_TARGET_BUFFER_H
#define __BRANCH_TARGET_BUFFER_H

#include <vector>
#include "branch_history_table.h"
#include "record.h"

struct BTBLine {
    int inst_idx;
    BHT bht;
};

class BTB {
public:
    std::vector<BTBLine> buf;

    BTB();

    void reset();

    int predict(int inst_idx);

    void branch_succ(int inst_idx, int addr);

    void branch_fail(int inst_idx);

    void show();
};

#endif