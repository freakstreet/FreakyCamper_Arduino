#include "light.h"
#include "masterConf.h"


void initLightFunction() {
	// init lights commands
	byte i, pin;
	for (i=0; i<NB_LIGHTS; i++)
	{
		switch (lightsConf[i][LIGHT_TYPE]) {
			case LIGHT_TYPE_NORMAL :
				pinMode(lightsConf[i][LIGHT_POUT_R], OUTPUT);
				digitalWrite(lightsConf[i][LIGHT_POUT_R], LOW);
				break;
			case LIGHT_TYPE_DIMMABLE :
				pinMode(lightsConf[i][LIGHT_POUT_R], OUTPUT);
				analogWrite(lightsConf[i][LIGHT_POUT_R], LOW);
				break;

			case LIGHT_TYPE_RGB_DIMMABLE :
				pinMode(lightsConf[i][LIGHT_POUT_R], OUTPUT);
				digitalWrite(lightsConf[i][LIGHT_POUT_R], LOW);
				pinMode(lightsConf[i][LIGHT_POUT_G], OUTPUT);
				digitalWrite(lightsConf[i][LIGHT_POUT_G], LOW);
				pinMode(lightsConf[i][LIGHT_POUT_B], OUTPUT);
				digitalWrite(lightsConf[i][LIGHT_POUT_B], LOW);
			break;
		}
	}
}

void lightCommand(byte lightId, byte dim, byte red, byte green, byte blue){
	if (lightsConf[lightId][LIGHT_TYPE] == LIGHT_TYPE_DIMMABLE)
		setLightDimmedRGBValue(lightId, dim, 0, 0, 0);
	else
		setLightDimmedRGBValue(lightId, dim, red, green, blue);
}

void lightOnOff(byte lightId, byte status)
{
	byte st = lightsConf[lightId][LIGHT_STAT_DIMM];
	if (st != status)
	{
		digitalWrite(lightsConf[lightId][LIGHT_POUT_R], status>0);
		lightsConf[lightId][LIGHT_STAT_DIMM] = status;
	}
}

void setLightDimmedRGBValue(byte lightId, byte dim, byte r, byte g, byte b){
	// get LED strip pins
	analogWrite(lightsConf[lightId][LIGHT_POUT_R], r * dim/256);
	analogWrite(lightsConf[lightId][LIGHT_POUT_G], g * dim/256);
	analogWrite(lightsConf[lightId][LIGHT_POUT_B], b * dim/256);
}

long HSV_to_RGB( float h, float s, float v ) {
	/* modified from Alvy Ray Smith's site: http://www.alvyray.com/Papers/hsv2rgb.htm */
	// H is given on [0, 6]. S and V are given on [0, 1].
	// RGB is returned as a 24-bit long #rrggbb
	int i;
	float m, n, f;

	// not very elegant way of dealing with out of range: return black
	if ((s<0.0) || (s>1.0) || (v<1.0) || (v>1.0)) {
		return 0L;
	}

	if ((h < 0.0) || (h > 6.0)) {
		return long( v * 255 ) + long( v * 255 ) * 256 + long( v * 255 ) * 65536;
	}
	i = floor(h);
	f = h - i;
	if ( !(i&1) ) {
		f = 1 - f; // if i is even
	}
	m = v * (1 - s);
	n = v * (1 - s * f);
	
	switch (i) {
		case 6:
		case 0: 
			return long(v * 255 ) * 65536 + long( n * 255 ) * 256 + long( m * 255);
		case 1: 
			return long(n * 255 ) * 65536 + long( v * 255 ) * 256 + long( m * 255);
		case 2: 
			return long(m * 255 ) * 65536 + long( v * 255 ) * 256 + long( n * 255);
		case 3: 
			return long(m * 255 ) * 65536 + long( n * 255 ) * 256 + long( v * 255);
		case 4: 
			return long(n * 255 ) * 65536 + long( m * 255 ) * 256 + long( v * 255);
		case 5: 
			return long(v * 255 ) * 65536 + long( m * 255 ) * 256 + long( n * 255);
	}
} 