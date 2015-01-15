#include "current.h"

void poolCurrentSensors(){
	byte i;
	
	for (i=0; i<NB_CURRENT_SENSORS; i++){
		int inputValue = analogRead(currentPins[i]);
		float tmp = inputValue * currentCalFactors[i];
		currents[i] = 100 * tmp;
		#ifdef DEBUG
			Serial.print("getCurrentValue, ADC value:");
			Serial.print(inputValue, HEX);
			Serial.print(",  current float value:");
			Serial.print(tmp);
			Serial.print(",  current shortx100 value:");
			Serial.println(currents[i] );
		#endif
	}
}