#ifndef __STATION_H
#define __STATION_H

#include "nel.h"
#include "cycle_counter.h"
#include "reservation_station.h"

class OpStation: public ReservationStation{
public:
    bool busy;
    int ready; //-1: 未就绪, 0..: 就绪cycle
    nel::inst_t op;
    int inst_idx;
    int v[2];
    ReservationStation* q[2];

    OpStation();
    void get_result_from(ReservationStation* rs, int result);
    void reset();
};

class LoadBuffer: public ReservationStation {
public:
    bool busy;
    int addr;

    LoadBuffer();
    void get_result_from(ReservationStation* rs, int result);
    void reset();
};

template<unsigned NUM>
class OpStations {
public:
    OpStation ops[NUM];
    int ops_to_pq[NUM];
    int pq[NUM];
    OpStations(const std::string& prefix) {
        for (int i = 0; i < NUM; ++i) {
            ops[i].set_name(prefix + std::to_string(i));
            pq[i] = i;
            ops_to_pq[i] = i;
        }
    }

    bool push(const nel::inst_t& inst, int v0, int v1, ReservationStation* q0, ReservationStation* q1) {
        for (int i = 0; i < NUM; ++i) {
            if (!ops[i].busy) {
                ops[i].busy = true;
                ops[i].op = inst.opr;
                ops[i].inst_idx = inst.inst_idx;
                ops[i].v[0] = v0;
                ops[i].v[1] = v1;
                ops[i].q[0] = q0;
                ops[i].q[1] = q1;
                if (q0 == nullptr && q1 == nullptr) {
                    ops[i].ready = CycleCounter::get_instance().counter;

                    //sort pq
                    //TODO
                } else {
                    ops[i].ready = -1;
                }
                return true;
            }
        }
        return false;
    }

    void send_to_fus() {

    }
};




#endif