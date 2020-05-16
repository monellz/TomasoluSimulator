#ifndef __FUNCTIONAL_UNIT_H
#define __FUNCTIONAL_UNIT_H

#include "reg.h"
#include "station.h"

struct FunctionalUnit {
    ReservationStation* executing;
    int left_cycle;
    int result;
};

template<unsigned NUM>
class FUs {
public:
    FunctionalUnit units[NUM];

    FUs() {
        reset();
    }

    void reset() {
        for (int i = 0; i < NUM; ++i) {
            units[i].executing = nullptr;
            units[i].left_cycle = 0;
            units[i].result = 0;
        }
    }

    bool push(ReservationStation* task, int left_cycle, int result) {
        for (int i = 0; i < NUM; ++i) {
            if (units[i].executing == nullptr) {
                units[i].executing = task;
                units[i].left_cycle = left_cycle;
                units[i].result = result;
                return true;
            }
        }
        return false;
    }
    void write_back(reg_t regs[]) {
        //check left_cycle
        for (int i = 0; i < NUM; ++i) {
            if (units[i].left_cycle == 0) {
                assert(units[i].executing != nullptr);
                ReservationStation* executing = units[i].executing;
                //write back

                //update all regs
                for (int j = 0; j < executing->waiting_regs.size(); ++j) {
                    int reg_idx = executing->waiting_regs[j];
                    if (regs[reg_idx].status == executing) {
                        regs[reg_idx].status = nullptr;
                        regs[reg_idx].value = units[i].result;
                    }
                }
                executing->waiting_regs.clear();

                //update all waiting rs
                for (int j = 0; j < executing->waiting_rs.size(); ++j) {
                    auto rs = executing->waiting_rs[j];
                    rs->get_result_from(executing, units[i].result);
                }
                executing->waiting_rs.clear();

                units[i].executing = nullptr;
            } else {
                units[i].left_cycle -= 1;
            }
        }
    }
};

#endif