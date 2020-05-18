#ifndef __REG_H
#define __REG_H

#include "util.h"
#include "reservation_station.h"

struct reg_t {
    ReservationStation* status;
    int value;
};

#endif