#include "protocol.h"
#include "masterConf.h"
#include "electrical.h"
#include "current.h"
#include "water.h"
#include "light.h"
#include "heater.h"

#include "temperatureSensors.h"

byte watchdog = 0;

byte rxBuffer[TELEMETRY_BUFFER_MAX_SIZE];
byte txBuffer[TELEMETRY_BUFFER_MAX_SIZE];

void handleUsbError(byte ack);

byte tmBuilderIsAlive(){
	byte p = 0;
	txBuffer[p++] = TM_IS_ALIVE;
	return p;
}

void sendTM(byte length){

	byte i, ack, len;
	
	if (length==0) return;
	
	len = length+2*PROT_REPEAT_HEADER;
	
	// copy datas
	for (i=length; i>0;i--)
		txBuffer[i-1+PROT_REPEAT_HEADER] = txBuffer[i-1];
	
	// prepare header
	for (i=0; i<PROT_REPEAT_HEADER;i++) 
		txBuffer[i] =PROT_HEADER;
			
	// prepare footer
	for (i=0; i<PROT_REPEAT_HEADER;i++) 
		txBuffer[i+length+PROT_REPEAT_HEADER] =PROT_FOOTER;
	
	ack = adk.SndData(len, txBuffer);
	
	#ifdef DEBUG
		printTM(len);
		if (hrSUCCESS == ack){
			Serial.println("ok");
			watchdog = 0;
		}
		else {
			Serial.print("USB error 0x");
			Serial.println(ack, HEX);
		}
	#endif
		
	if (hrSUCCESS != ack)
		handleUsbError(ack);
}


void handleUsbError(byte ack){
	switch (ack){

		case hrBUSY :
			break;

		case hrBADREQ :
			break;

		case hrUNDEF :
			break;

		case hrNAK :
			break;

		case hrSTALL :
			break;

		case hrTOGERR :
			break;

		case hrWRONGPID :	// PID error, wrong or corrupted --> reset device
		case hrPIDERR :
				adk.Release();
			break;

		case hrBADBC :
			break;


		case hrPKTERR :
			break;

		case hrCRCERR :
			break;

		case hrKERR :
			break;

		case hrJERR :
			break;

		case hrTIMEOUT :
			break;

		case hrBABBLE :
			break;

	}
}
	   

void processReceivedCommand(int length){
	int pData = 0;
	int i;
	#ifdef DEBUG
		printTC(data, length);
	#endif
	
	switch((byte)rxBuffer[pData]){
		case PROT_KEEP_ALIVE :
			Serial.print("TC keep alive.");
			watchdog = 0;
			i = tmBuilderIsAlive();
			sendTM(i);
			break;
			
		case PROT_SWITCH_COLD_MODULE :
			Serial.print("TC Switch Cold: ");
			switchRelay(POUT_RELAY_FRIDGE, rxBuffer[pData+1]==1);
			Serial.println("");
			break;
		
		case PROT_SWITCH_WATER_MODULE :
			Serial.print("TC Switch Water: ");
			switchRelay(POUT_RELAY_WATER, rxBuffer[pData+1]==1);	
			Serial.println("");
			break;
		
		case PROT_SWITCH_HEAT_MODULE :
			Serial.print("TC Switch Heat:");
			switchRelay(POUT_RELAY_HEATING, rxBuffer[pData+1]==1);
			Serial.println("");
			break;
		
		case PROT_SWITCH_LIGHT_MODULE :
			Serial.println("TC Light :");
			if (length < 7) {
				//#ifdef DEBUG
					Serial.println("ERROR: processManagedFrame.PROT_SWITCH_LIGHT_MODULE:insufficient data length -> skip data processiing");
				//#endif
				Serial.println("");
				return;
			}
			if (rxBuffer[pData+1]==LIGHT_TYPE_NORMAL) 
				lightOnOff(rxBuffer[pData+2], rxBuffer[pData+3]);
			else 
				lightCommand(rxBuffer[pData+2],rxBuffer[pData+3],rxBuffer[pData+4],rxBuffer[pData+5], rxBuffer[pData+6]);
			Serial.println("");
			break;
		
		case PROT_SWITCH_AUX_MODULE :
			Serial.print("TC Switch AUX: ");
			switchRelay(POUT_RELAY_AUX, rxBuffer[pData+1]==1);
			Serial.println("");
			break;
		
		case PROT_SWITCH_SPARE_MODULE :
			Serial.print("TC Switch spare:");
			switchRelay(POUT_RELAY_SPARE, rxBuffer[pData+1]==1);
			Serial.println("");
			break;
		
		case PROT_COLD_CONSIGNE :
			Serial.print("TC Cold:");
			//setFridgeTempConsigne(data[pData+1]);
			Serial.println("");
			break;
		
		case PROT_HEATER :
			Serial.print("TC Heater: ");
			Serial.println("");
			heaterGotTC((eHeaterState)rxBuffer[pData+1], (byte)rxBuffer[pData+2], (byte)rxBuffer[pData+3], rxBuffer[(byte)pData+4]);
			break;
	
	}
	
}

byte tmBuilderCurrent(){
	byte i, msb, lsb;
	byte p = 0;
	
	txBuffer[p++] = TM_CURRENT;
	
	for (i=0;i<NB_CURRENT_SENSORS; i++){
		txBuffer[p++] = highByte(currents[i]); 
		txBuffer[p++] = lowByte(currents[i]);
	}
	return p;
}

byte tmBuilderTension(){
	byte i, msb, lsb;
	byte p = 0;
	
	txBuffer[p++] = TM_TENSION;
	
	for (i=0;i<NB_TENSION_SENSORS; i++){
		txBuffer[p++] = highByte(tensions[i]); 
		txBuffer[p++] = lowByte(tensions[i]);
	}
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
byte tmBuilderTemperature(){
	byte p = 0;
	byte tempEncoded, i;

	txBuffer[p++] = TM_TEMPERATURE;
	
	for (i=0;i<NB_TEMP_SENSORS; i++) {
		tempEncoded = formatTempData(getTemperatureFromIndex(i));
		txBuffer[p++] = tempEncoded;
	}
	
	return p;
}

byte tmBuilderWater(){
	byte p = 0;
	txBuffer[p++] = TM_WATER;
	txBuffer[p++] = isPumpActive();
	txBuffer[p++] = getCleanWaterLevel();
	txBuffer[p++] = isDarkWaterTankFull();
	txBuffer[p++] = waterFlowLitersMin;
	txBuffer[p++] = isEvierOpened();
	txBuffer[p++] = isShowerOpened();
	return p;
}

byte tmBuilderSwitch(){
	byte p = 0;
	byte i;
	boolean sw = 0;
	txBuffer[p++] = TM_SWITCH;
	for (i=0;i<NB_ELEC_SWITCHES; i++) {
		txBuffer[p++] = (getRelayStatus((eElecRelays)i)?1:0);
	}
	return p;
}

byte tmBuilderLight(byte lightId){
	byte p = 0;
	byte i;
	txBuffer[p++] = TM_LIGHT;
	txBuffer[p++] = lightId;
	txBuffer[p++] = lightsConf[lightId][LIGHT_TYPE];
	txBuffer[p++] = lightsConf[lightId][LIGHT_STAT_DIMM];
	txBuffer[p++] = lightsConf[lightId][LIGHT_STAT_R];
	txBuffer[p++] = lightsConf[lightId][LIGHT_STAT_G];
	txBuffer[p++] = lightsConf[lightId][LIGHT_STAT_B];
	return p;
}

byte tmBuilderColdHot(){
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
	Serial.println(".");
}

void printTM(byte length){
	byte i;
	Serial.print("TM: ");
	printDatas(txBuffer, length);
}

void printTC(byte length){
	byte i;
	Serial.print("TC: ");
	printDatas(rxBuffer, length);
}























