#ifndef LIGHT_H
#define LIGHT_H

#include <Arduino.h>


void initLightFunction();

void lightButtonPushed();

void setLightDimmedRGBValue(byte dim, byte r, byte g, byte b);

long HSV_to_RGB( float h, float s, float v );


#endif
