#ifndef LIGHT_H
#define LIGHT_H

#include <Arduino.h>


void initLightFunction();
void lightCommand(byte lightId, byte dim, byte red, byte green, byte blue);
void lightOnOff(byte lightId, byte status);

void lightButtonPushed();

void setLightDimmedRGBValue(byte lightId, byte dim, byte r, byte g, byte b);

long HSV_to_RGB( float h, float s, float v );


#endif
