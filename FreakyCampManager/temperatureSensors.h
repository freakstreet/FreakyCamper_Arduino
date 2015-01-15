#ifndef TEMPERATURESENSORS_H
#define TEMPERATURESENSORS_H

#include "masterConf.h"

// Get 1-wire Library here: http://www.pjrc.com/teensy/td_libs_OneWire.html
#include <OneWire.h>

//Get DallasTemperature Library here:  http://milesburton.com/Main_Page?title=Dallas_Temperature_Control_Library
#include <DallasTemperature.h>

void initTempSensors();
void poolTemperatureSensors();
void serialPrintTemperature(float tempC);
float getTemperatureFromIndex(uint8_t idx);



#endif