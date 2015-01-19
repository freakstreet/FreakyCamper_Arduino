#ifndef WATER_H
#define WATER_H

#include "Arduino.h"
#include "masterConf.h"


extern byte waterFlowLitersMin;
extern long flowSensorIntsCount;

void poolWaterInfos();

void initWaterModule();
byte getCleanWaterLevel();            		// 0 (empty) to 100 (full)
byte isDarkWaterTankFull();		        // 0 (empty) to 100 (full)

boolean getPumpRelayStatus();
boolean isPumpActive();                      	// 1 if active, 0 is off
unsigned long getPumpConsoMa();      	// returns the water pump conso in miliAmps. 

boolean isEvierOpened();
boolean isShowerOpened();

void onWaterFlowTimer();			// to call each second to compute the water flow measured


#endif