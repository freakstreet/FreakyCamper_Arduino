#include "protocol.h"
#include "masterConf.h"
#include "electrical.h"
#include "current.h"
#include "water.h"

#include "temperatureSensors.h"


byte rxBuffer[TELEMETRY_BUFFER_MAX_SIZE];
byte txBuffer[TELEMETRY_BUFFER_MAX_SIZE];


void processReceivedCommand(byte* data, int length){
	int pData = 0;
	int i;
	#ifdef DEBUG
		printTC(data, length);
	#endif
	
	if (length < 2+PROT_REPEAT_HEADER) {
		#ifdef DEBUG
			Serial.println("ERROR: processManagedFrame:insufficient data length -> skip data processiing");
		#endif
		return;
	}
	
	for (i=0; i<PROT_REPEAT_HEADER; i++){
		if (data[pData++] != PROT_HEADER){
			#ifdef DEBUG
				Serial.print("ERROR: processManagedFrame: header not recognised, symbol 0x");
				Serial.print(PROT_HEADER, HEX);
				Serial.print(" repeated ");
				Serial.print(pData);
				Serial.println(" times");
			#endif
			return;
		}
	}

	#ifdef DEBUG
	Serial.println("Header check passed");
	Serial.print("Received command [");
	Serial.print(data[pData]);
	Serial.println(" ");
	#endif
	
	switch((byte)data[pData]){
		case PROT_SWITCH_COLD_MODULE :
			switchRelay(POUT_RELAY_FRIDGE, data[pData+1]==1);
			break;
		
		case PROT_SWITCH_WATER_MODULE :
			switchRelay(POUT_RELAY_WATER, data[pData+1]==1);	
			break;
		
		case PROT_SWITCH_HEAT_MODULE :
			switchRelay(POUT_RELAY_HEATING, data[pData+1]==1);
			break;
		
		case PROT_SWITCH_LIGHT_MODULE :
			if (length < 8) {
				#ifdef DEBUG
					Serial.println("ERROR: processManagedFrame.PROT_SWITCH_LIGHT_MODULE:insufficient data length -> skip data processiing");
				#endif
				return;
			}
			if (data[pData+1]==0) lightOff(data[pData+2]);
			else lightCommand(data[pData+2],data[pData+3],data[pData+4],data[pData+5],data[pData+6]);
			break;
		
		case PROT_SWITCH_AUX_MODULE :
			switchRelay(POUT_RELAY_AUX, data[pData+1]==1);
			break;
		
		case PROT_SWITCH_SPARE_MODULE :
			switchRelay(POUT_RELAY_SPARE, data[pData+1]==1);
			break;
		
		case PROT_COLD_CONSIGNE :
			//setFridgeTempConsigne(data[pData+1]);
			break;
		
		case PROT_HEATER :
			break;
	
	}
	
}

byte tmBuilderCurrent(byte* data){
	byte i, msb, lsb;
	byte p = 0;
	
	data[p++] = TM_CURRENT;
	
	for (i=0;i<NB_CURRENT_SENSORS; i++)
		data[p++] = highByte(currents[i]); 
		data[p++] = lowByte(currents[i]);
	return p;
}

byte tmBuilderTension(byte* data){
	byte i, msb, lsb;
	byte p = 0;
	
	data[p++] = TM_TENSION;
	
	for (i=0;i<NB_TENSION_SENSORS; i++)
		data[p++] = highByte(tensions[i]); 
		data[p++] = lowByte(tensions[i]);
	return p;
}
/*//////////////////////////////////////////////////////////////////////////////
temp data allow -64 to +64 °C with 0,5 digit
	0 	--> 	-64°C
	128	-->	0°c
	256	-->	64°C
///////////////////////////////////////////////////////////////////////////////*/
byte formatTempData(float temp){
	int val = 128+(int)(temp*2);
	byte encodedTemp = val;
	#ifdef DEBUG
		Serial.print("formatTempData, initial temp:");
		Serial.print(temp);
		Serial.print("°C,  encoded value:");
		Serial.println(encodedTemp);
	#endif
	return encodedTemp;
}

// E.G. : 0x48 0xA6 0xA3 0xA3 0xA3 0xA4 0xA3 0xA4
byte tmBuilderTemperature(byte* data){
	byte p = 0;
	byte tempEncoded, i;

	data[p++] = TM_TEMPERATURE;
	
	for (i=0;i<NB_TEMP_SENSORS; i++) {
		tempEncoded = formatTempData(getTemperatureFromIndex(i));
		data[p++] = tempEncoded;
	}
	
	return p;
}

byte tmBuilderWater(byte* data){
	byte p = 0;
	data[p++] = TM_WATER;
	data[p++] = isPumpActive();
	data[p++] = getCleanWaterLevel();
	data[p++] = isDarkWaterTankFull();
	data[p++] = waterFlowLitersMin;
	return p;
}

byte tmBuilderSwitch(byte* data){
	byte p = 0;
	byte i;
	boolean sw = 0;
	data[p++] = TM_SWITCH;
	for (i=0;i<NB_ELEC_SWITCHES; i++) {
		data[p++] = (getRelayStatus((eElecRelays)i)?1:0);
	}
	return p;
}

byte tmBuilderLight(byte* data){
	byte p = 0;
	
	return p;
}

byte tmBuilderColdHot(byte* data){
	byte p = 0;
	
	return p;
}

void printDatas(byte* data, byte length){
	byte i;
	for (i=0; i<length; i++){
		Serial.print("0x");
		if (data[i] < 0x10) Serial.print("0");
		Serial.print(data[i], HEX);
		Serial.print(" ");
	}
}

void printTM(byte* data, byte length){
	byte i;
	Serial.print("TM: ");
	printDatas(data, length);
}

void printTC(byte* data, byte length){
	byte i;
	Serial.print("TC: ");
	printDatas(data, length);
	Serial.println(".");
}























