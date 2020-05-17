#include "station.h"

OpStation::OpStation() {
    reset();
}

void OpStation::get_result_from(ReservationStation* rs, int result) {
    if (q[0] == rs) {
        q[0] = nullptr;
        v[0] = result;
    }

    if (q[1] == rs) {
        q[1] = nullptr;
        v[1] = result;
    }
    
    if (q[0] == nullptr && q[1] == nullptr) ready = CycleCounter::get_instance().counter;
    else {
        ready = -1;
    }
}

void OpStation::reset() {
    busy = false;
    exec = false;
    ready = -1;
    q[0] = q[1] = nullptr;
    inst_idx = -1;
    v[0] = v[1] = 0;
    op = nel::None;
    waiting_reg = -1;
    waiting_rs.clear();
}

LoadBuffer::LoadBuffer() {
    reset();
}

void LoadBuffer::get_result_from(ReservationStation* rs, int result) {
    reset();
}

void LoadBuffer::reset() {
    busy = false; 
    addr = 0;
    exec = false;
    ready = -1;
    reg = -1;
    inst_idx = -1;
    waiting_reg = -1;
    waiting_rs.clear();
}
