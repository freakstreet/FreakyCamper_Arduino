#ifndef HEATER_H
#define HEATER_H

#include "masterConf.h"

void handleHeater();
void switchToHotAir();
void switchToFreshAir();
void heaterGotTC(eHeaterState status, byte fan1val, byte fan2val, byte newTemp);

#endif