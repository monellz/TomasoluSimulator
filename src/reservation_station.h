#ifndef __RESERVATION_STATION_H
#define __RESERVATION_STATION_H

#include <string>
#include <vector>

class ReservationStation {
private:
    std::string name;
public:
    //rs which wait for the result of this rs
    std::vector<ReservationStation*> waiting_rs;
    int waiting_reg;
    int inst_idx;
    bool exec;
    std::string get_name() {
        return name;
    }

    void set_name(const std::string& n) {
        name = n;
    }

    inline void exec_start() {
        exec = true;
    }

    virtual void get_result_from(ReservationStation* rs, int result) = 0;
    virtual void reset() = 0;
};

#endif
