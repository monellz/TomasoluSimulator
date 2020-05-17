#ifndef __REG_H
#define __REG_H

#include "reservation_station.h"

#define PC 32
#define REG_NUM 33

struct reg_t {
    ReservationStation* status;
    int value;
};


#endif