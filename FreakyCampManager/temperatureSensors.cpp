#include "temperatureSensors.h"

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(PIN_TEMPERATURE_DS8020);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

void initTempSensors() {
	uint8_t i;
	uint8_t j;
	
	DeviceAddress addr;
	#ifdef DEBUG
		Serial.print("Initializing Temperature Control Library Version ");
		Serial.println(DALLASTEMPLIBVERSION);
	#endif
	// Initialize the Temperature measurement library
	sensors.begin();

	// set the resolution to 10 bit (Can be 9 to 12 bits .. lower is faster)
	for (i=0;i<NB_TEMP_SENSORS; i++) {
		sensors.setResolution(TempProbes[i], 10);
		
		sensors.getAddress(addr, i);
		Serial.print("/* Sensor #");
		Serial.print(i);
		Serial.print(" */ {");
		for (j=0; j<8;j++){
			Serial.print("0x");
			Serial.print(addr[j], HEX);
			Serial.print(",");
		}
		Serial.println("} ");
		
	}
	poolTemperatureSensors();

	Serial.println("Temperature sensors init OK");
	
}//--(end setup )---


void poolTemperatureSensors() {
	uint8_t i;
	// Command all devices on bus to read temperature  
	sensors.requestTemperatures(); 
	#ifdef DEBUG
		for (i=0;i<NB_TEMP_SENSORS; i++) {
				Serial.print("Sensor ");
				Serial.print(i);
				Serial.print(": ");
				serialPrintTemperature(getTemperatureFromIndex(i));
				Serial.println(".");
		}
	#endif
}

float getTemperatureFromIndex(uint8_t idx)
{
	return sensors.getTempC(TempProbes[idx]);
}

void serialPrintTemperature(float tempC)
{
	if (tempC == -127.00) {
		Serial.print("Error getting temperature  ");
	} 
	else{
		Serial.print(tempC);
		Serial.print("°C");
	}
}// End printTemperature

float getTemperature(eTempSensorId sensor) {
	return sensors.getTempC(TempProbes[sensor]);
}



	
