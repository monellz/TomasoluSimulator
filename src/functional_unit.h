#ifndef __FUNCTIONAL_UNIT_H
#define __FUNCTIONAL_UNIT_H

#include "reg.h"
#include "station.h"
#include "record.h"
#include "reorder_buffer.h"
#include "branch_target_buffer.h"

struct FunctionalUnit {
    std::string name;
    ReservationStation* executing;
    rob_iter it;
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
        working_fu = 0;
    }

    inline bool empty() {
        return working_fu == 0;
    }

    void show() {
        printf("Functinal Units(working: %d):\n", working_fu);
        printf("\t\tRS\tleft cycle\n");
        for (int i = 0; i < NUM; ++i) {
            printf("\t%s:", units[i].name.c_str());
            if (units[i].executing != nullptr) printf("\t%s", units[i].executing->get_name().c_str());
            else printf("\t ");
            printf("\t%d\n", units[i].left_cycle);
        }
    }

    bool push_bp(ReservationStation* task, int inst_idx, int left_cycle, int result, rob_iter it) {
        for (int i = 0; i < NUM; ++i) {
            if (units[i].executing == nullptr) {
                units[i].executing = task;
                units[i].left_cycle = left_cycle;
                units[i].result = result;
                units[i].inst_idx = inst_idx;
                units[i].it = it;
                working_fu++;

                task->exec_start();
                return true;
            }
        }
        return false;


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
    void write_back_bp(reg_t regs[], BTB& btb) {
         //check left_cycle
        for (int i = 0; i < NUM; ++i) {
            if (units[i].executing != nullptr && units[i].left_cycle == 0) {
                ReservationStation* executing = units[i].executing;
                //write back
                Record::get_instance().update_write(units[i].inst_idx, CycleCounter::get_instance().counter);

                //update reg
                int reg_idx = executing->waiting_reg;
                if (reg_idx >= 0 && executing->waiting_reg < REG_NUM) {
                    if (reg_idx == PC) {
                        //must be jump instruction

                        OpStation* rs = static_cast<OpStation*>(executing);
                        if (rs->v[0] == rs->v[1]) {
                            //jump succ
                            btb.branch_succ(rs->inst_idx, rs->inst_idx + rs->offset);

                            //write result into rob
                            units[i].it->ready = true;
                            units[i].it->result = rs->inst_idx + rs->offset;

                        } else {
                            //jump fail
                            btb.branch_fail(rs->inst_idx);

                            //write result into rob
                            units[i].it->ready = true;
                            units[i].it->result = rs->inst_idx + 1;
                        }
                    } else {
                        units[i].it->ready = true;
                        units[i].it->result = units[i].result;

                        printf("set ready for rob, wr: %d, op: %s, rs: %s\n", units[i].it->write_reg, nel::op_name[units[i].it->op], units[i].it->rs->get_name().c_str());
                    }

                }
                executing->waiting_reg = -1;

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

    void write_back(reg_t regs[], bool& stall) {
        //check left_cycle
        for (int i = 0; i < NUM; ++i) {
            if (units[i].executing != nullptr && units[i].left_cycle == 0) {
                ReservationStation* executing = units[i].executing;
                //write back
                Record::get_instance().update_write(units[i].inst_idx, CycleCounter::get_instance().counter);

                //update reg
                int reg_idx = executing->waiting_reg;
                if (reg_idx >= 0 && executing->waiting_reg < REG_NUM) {
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
                executing->waiting_reg = -1;

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