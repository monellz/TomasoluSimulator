#ifndef __FUNCTIONAL_UNIT_H
#define __FUNCTIONAL_UNIT_H

#include "reg.h"
#include "station.h"
#include "record.h"

struct FunctionalUnit {
    std::string name;
    ReservationStation* executing;
    int left_cycle;
    int result;
    int inst_idx;
};

template<unsigned NUM>
class FUs {
public:
    FunctionalUnit units[NUM];
    int working_fu;

    FUs(const std::string& prefix) {
        working_fu = 0;
        for (int i = 0; i < NUM; ++i) {
            units[i].name = prefix + std::to_string(i);
        }
        reset();
    }

    void reset() {
        for (int i = 0; i < NUM; ++i) {
            units[i].executing = nullptr;
            units[i].left_cycle = 0;
            units[i].result = 0;
        }
    }

    inline bool empty() {
        return working_fu == 0;
    }

    void show() {
        printf("Functinal Units:\n");
        printf("\t\tRS\tleft cycle\n");
        for (int i = 0; i < NUM; ++i) {
            printf("\t%s:", units[i].name.c_str());
            if (units[i].executing != nullptr) printf("\t%s", units[i].executing->get_name().c_str());
            else printf("\t ");
            printf("\t%d\n", units[i].left_cycle);
        }
    }

    bool push(ReservationStation* task, int inst_idx, int left_cycle, int result) {
        for (int i = 0; i < NUM; ++i) {
            if (units[i].executing == nullptr) {
                units[i].executing = task;
                units[i].left_cycle = left_cycle;
                units[i].result = result;
                units[i].inst_idx = inst_idx;
                working_fu++;

                task->exec_start();
                return true;
            }
        }
        return false;
    }
    void write_back(reg_t regs[], bool& stall) {
        //check left_cycle
        for (int i = 0; i < NUM; ++i) {
            if (units[i].executing != nullptr && units[i].left_cycle == 0) {
                ReservationStation* executing = units[i].executing;
                //write back
                Record::get_instance().update_write(units[i].inst_idx, CycleCounter::get_instance().counter);

                //update all regs
                for (int j = 0; j < executing->waiting_regs.size(); ++j) {
                    int reg_idx = executing->waiting_regs[j];
                    if (reg_idx == PC && regs[PC].status == executing) {
                        //must be jump
                        regs[PC].status = nullptr;
                        OpStation* rs = static_cast<OpStation*>(executing);
                        if (rs->v[0] == rs->v[1]) {
                            //jump
                            regs[PC].value = rs->inst_idx + rs->offset;
                        }

                        stall = false;
                    } else {
                        if (regs[reg_idx].status == executing) {
                            regs[reg_idx].status = nullptr;
                            regs[reg_idx].value = units[i].result;
                        }
                    }
                }
                executing->waiting_regs.clear();

                //update all waiting rs
                for (int j = 0; j < executing->waiting_rs.size(); ++j) {
                    auto rs = executing->waiting_rs[j];
                    rs->get_result_from(executing, units[i].result);
                }
                executing->waiting_rs.clear();

                executing->reset();
                units[i].executing = nullptr;
                working_fu--;
            }
        }
    }

    void update() {
        for (int i = 0; i < NUM; ++i) {
            units[i].left_cycle -= 1;
            if (units[i].left_cycle == 0) {
                Record::get_instance().update_exec(units[i].inst_idx, CycleCounter::get_instance().counter);
            }
        }
    }
};

#endif