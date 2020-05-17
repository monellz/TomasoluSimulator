#ifndef __STATION_H
#define __STATION_H

#include "nel.h"
#include "cycle_counter.h"
#include "reservation_station.h"
#include "reorder_buffer.h"

class OpStation: public ReservationStation{
public:
    bool busy;
    int ready; //-1: 未就绪, 0..: 就绪cycle
    nel::op_t op;
    int v[2];
    int offset;
    rob_iter it;
    ReservationStation* q[2];

    OpStation();
    void get_result_from(ReservationStation* rs, int result);
    void reset();
};

class LoadBuffer: public ReservationStation {
public:
    bool busy;
    int ready;
    int addr;
    int reg;
    rob_iter it;

    LoadBuffer();
    void get_result_from(ReservationStation* rs, int result);
    void reset();
};

template<unsigned NUM>
class LoadBuffers {
public:
    LoadBuffer bufs[NUM];
    LoadBuffers(const std::string& prefix) {
        for (int i = 0; i < NUM; ++i) {
            bufs[i].set_name(prefix + std::to_string(i));
        }
        reset();
    }

    void reset() {
        for (int i = 0; i < NUM; ++i) {
            bufs[i].reset();
        }        
    }

    int available() {
        for (int i = 0; i < NUM; ++i) {
            if (!bufs[i].busy) return i;
        }
        return -1;
    }

    ReservationStation* push_at(int i, int inst_idx, int addr, int write_reg, rob_iter it) {
        assert(i >= 0 && i < NUM && !bufs[i].busy);
        bufs[i].busy = true;
        bufs[i].addr = addr;
        bufs[i].reg = write_reg;
        bufs[i].inst_idx = inst_idx;
        bufs[i].ready = CycleCounter::get_instance().counter;
        bufs[i].it = it;
        return &bufs[i];
    }

    ReservationStation* push(int inst_idx, int addr, int write_reg) {
        for (int i = 0; i < NUM; ++i) {
            if (!bufs[i].busy) {
                bufs[i].busy = true;
                bufs[i].addr = addr;
                bufs[i].reg = write_reg;
                bufs[i].inst_idx = inst_idx;
                bufs[i].ready = CycleCounter::get_instance().counter;
                return &bufs[i];
            }
        }
        return nullptr;
    }

    ReservationStation* select_task(int& inst_idx, int& result, int& left_cycle, rob_iter& it) {
        //find the task
        int min_ready = INT32_MAX;
        ReservationStation* target = nullptr;
        for (int i = 0; i < NUM; ++i) {
            if (bufs[i].busy && !bufs[i].exec && bufs[i].ready != -1 && bufs[i].ready < min_ready) {
                target = &bufs[i];
                result = bufs[i].addr;
                inst_idx = bufs[i].inst_idx;
                it = bufs[i].it;
                left_cycle = LOAD_CYCLE + 1;
            }
        }
        return target;
    }

    void show() {
        printf("Load Buffer:\n");
        printf("\t\tBusy\tAddress\n");
        for (int i = 0; i < NUM; ++i) {
            printf("\t%s:", bufs[i].get_name().c_str());
            if (bufs[i].busy) printf("\tYes");
            else printf("\tNo");
            printf("\t%x\n", bufs[i].addr);
        }
    }
};

template<unsigned NUM>
class OpStations {
public:
    OpStation ops[NUM];
    OpStations(const std::string& prefix) {
        for (int i = 0; i < NUM; ++i) {
            ops[i].set_name(prefix + std::to_string(i));
        }
        reset();
    }

    void reset() {
        for (int i = 0; i < NUM; ++i) {
            ops[i].reset();
        }
    }

    void show() {
        printf("OpStations:\n");
        printf("\t\tBusy\tOp\tVj\tVk\tQj\tQk\n");
        for (int i = 0; i < NUM; ++i) {
            printf("\t%s:", ops[i].get_name().c_str());
            if (ops[i].busy) printf("\tYes");
            else printf("\tNo");
            printf("\t%s", nel::op_name[ops[i].op]);
            printf("\t%x", ops[i].v[0]);
            printf("\t%x", ops[i].v[1]);
            std::string rs;
            rs = " ";
            if (ops[i].q[0] != nullptr) rs = ops[i].q[0]->get_name().c_str();
            printf("\t%s", rs.c_str());
            rs = " ";
            if (ops[i].q[1] != nullptr) rs = ops[i].q[1]->get_name().c_str();
            printf("\t%s\n", rs.c_str());
        }
    }

    int available() {
        for (int i = 0; i < NUM; ++i) {
            if (!ops[i].busy) return i;
        }
        return -1;
    }

    ReservationStation* push_at(int i, nel::op_t op, int inst_idx, int v0, int v1, ReservationStation* q0, ReservationStation* q1, rob_iter it, int offset = 0) {
        assert(i >= 0 && i < NUM && !ops[i].busy);
        ops[i].busy = true;
        ops[i].op = op;
        ops[i].inst_idx = inst_idx;
        ops[i].v[0] = v0;
        ops[i].v[1] = v1;
        ops[i].q[0] = q0;
        ops[i].q[1] = q1;
        ops[i].offset = offset;
        ops[i].it = it;
        if (q0 == nullptr && q1 == nullptr) {
            ops[i].ready = CycleCounter::get_instance().counter;
        } else {
            ops[i].ready = -1;
        }
        return &ops[i];
    }

    ReservationStation* push(nel::op_t op, int inst_idx, int v0, int v1, ReservationStation* q0, ReservationStation* q1, int offset = 0) {
        for (int i = 0; i < NUM; ++i) {
            if (!ops[i].busy) {
                ops[i].busy = true;
                ops[i].op = op;
                ops[i].inst_idx = inst_idx;
                ops[i].v[0] = v0;
                ops[i].v[1] = v1;
                ops[i].q[0] = q0;
                ops[i].q[1] = q1;
                ops[i].offset = offset;
                if (q0 == nullptr && q1 == nullptr) {
                    ops[i].ready = CycleCounter::get_instance().counter;
                } else {
                    ops[i].ready = -1;
                }
                return &ops[i];
            }
        }
        return nullptr;
    }

    ReservationStation* select_task(int& inst_idx, int& result, int& left_cycle, rob_iter& it) {
        //find the task
        int min_ready = INT32_MAX;
        int min_inst_idx = INT32_MAX;
        ReservationStation* target = nullptr;
        for (int i = 0; i < NUM; ++i) {
            if (ops[i].ready != -1 && !ops[i].exec) {
                if (ops[i].ready < min_ready || (ops[i].ready == min_ready && ops[i].inst_idx < min_inst_idx)) {
                    target = &ops[i];
                    inst_idx = ops[i].inst_idx;
                    it = ops[i].it;
                    switch (ops[i].op) {
                        case nel::Add: {
                            result = ops[i].v[0] + ops[i].v[1];
                            left_cycle = ADD_CYCLE + 1;
                            break;
                        }
                        case nel::Sub: {
                            result = ops[i].v[0] - ops[i].v[1];
                            left_cycle = SUB_CYCLE + 1;
                            break;
                        }
                        case nel::Mul: {
                            result = ops[i].v[0] * ops[i].v[1];
                            left_cycle = MUL_CYCLE + 1;
                            break;
                        }
                        case nel::Div: {
                            if (ops[i].v[1] == 0) {
                                result = ops[i].v[0];
                                left_cycle = DIV_ZERO_CYCLE + 1;
                            } else {
                                result = ops[i].v[0] / ops[i].v[1];
                                left_cycle = DIV_CYCLE + 1;
                            }
                            break;
                        }
                        case nel::Jump: {
                            left_cycle = JUMP_CYCLE + 1;
                            break;
                        }
                        default: {
                            printf("unknown op: %d", ops[i].op);
                            break;
                        }
                    }
                } 
            }
        }
        return target;
    }
};




#endif