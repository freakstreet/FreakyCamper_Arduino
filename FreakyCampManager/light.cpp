#include "light.h"
#include "masterConf.h"


void initLightFunction() {
	// init lights commands
	byte i, pin;
	for (i=0; i<NB_INPUT_SWITCHS; i++)
	{
		pin = lightsConf[i][1];
		pinMode(pin, OUTPUT);
		digitalWrite(pin, LOW);
	}
}

void setLightDimmedRGBValue(byte lightId, byte dim, byte r, byte g, byte b){
	// get LED strip pins
	
	analogWrite(lightsConf[lightId][3], r * dim/256);
	analogWrite(lightsConf[lightId][4], g * dim/256);
	analogWrite(lightsConf[lightId][5], b * dim/256);
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