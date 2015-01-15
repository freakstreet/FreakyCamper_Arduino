#include "tension.h"

void poolTensionSensors(){
	byte i;
	
	for (i=0; i<NB_TENSION_SENSORS; i++){
		int inputValue = analogRead(tensionPins[i]);
		float tmp = inputValue * tensionCalFactors[i];
		tensions[i] = 100 * tmp;
		#ifdef DEBUG
			Serial.print("getTensionValue, ADC value:");
			Serial.print(inputValue, HEX);
			Serial.print(",  tension float value:");
			Serial.print(tmp);
			Serial.print(",  tension shortx100 value:");
			Serial.println(tensions[i] );
		#endif
	}
}