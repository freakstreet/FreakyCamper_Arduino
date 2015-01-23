#ifndef WEBASTO_H
#define WEBASTO_H

#include "Arduino.h"
#include "masterConf.h"

void initWebasto();
void poolWebasto();
boolean isWebastoOn();
void stopWebasto();
void startWebasto();

void resetAllModules();
void startAirFan();
void stopAirFan();
boolean pulseFuelPump(boolean stateChangeInterrupt, boolean tempChangeInterrupt);
void startWaterPump();
void stopWaterPump();
void startGlowPlug();
void stopGlowPlug();
void startCabinHeater();
void stopCabinHeater();
double measureTemp(int maxRetries);
boolean wait(unsigned long milliseconds, boolean stateChangeInterrupt, boolean tempChangeInterrupt);



#endif