#ifndef __REG_H
#define __REG_H

#include "station.h"

#define PC 32

struct reg_t {
    ReservationStation* status;
    int value;
};


#endif