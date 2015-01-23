#ifndef HEATER_H
#define HEATER_H

#include "masterConf.h"

void heaterGotTC(eHeaterState status, byte fan1val, byte fan2val, byte newTemp);

#endif