#include "freakyCampManager.h"

#include <adk.h>
#include <Usb.h>
#include <TimerOne.h>

#include "temperatureSensors.h"
#include "masterConf.h"
#include "protocol.h"
#include "inputs.h"
#include "electrical.h"
#include "light.h"
#include "current.h"
#include "tension.h"

#define DEBUG

USB Usb;

ADK adk(&Usb,"Freakstreet",
            "Freakstreet",
            "Freakstreet",
            "1.0",
            "http://www.FreakyCamperTools.com",
            "0000000000000009");
	    
void sendTM(byte length, byte* data){
	byte txBuffer[TELEMETRY_BUFFER_MAX_SIZE];
	uint16_t pos = 0;
	byte i, ack;
	
	// copy datas
	for (i=0; i<length;i++){
		txBuffer[pos++] = data[i];
	}	
	#ifdef DEBUG
		printTM(txBuffer, pos);
	#endif
	ack = adk.SndData(pos, txBuffer);
	#ifdef DEBUG
		if (hrSUCCESS == ack)
			Serial.println("ok");
		else {
			Serial.print("USB error, code 0x");
			Serial.println(ack, HEX);
		}
	#endif
}

/*
Init temperature sensors

*/
void freakyCampManagerStart() {
	// Init Serial
	#ifdef DEBUG
		Serial.begin(115200);
		while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
		Serial.println("\r\nFreakyCampManager startup");
	#endif
        
        if (Usb.Init() == -1) {
		#ifdef DEBUG
			Serial.println("OSCOKIRQ failed to assert");
		#endif
		while(1); //halt
        }//if (Usb.Init() == -1...
	
	// Init electrical functions
	#ifdef DEBUG
		Serial.println("Init electrical functions");
	#endif
	initElectricalFunction();
	#ifdef DEBUG
		Serial.println("done electrical");
	#endif
	
	initLightFunction();
	#ifdef DEBUG
		Serial.println("done light");
	#endif
	
	
	// Init temp sensors
	#ifdef DEBUG
		Serial.println("Init temp sensors");
	#endif
	initTempSensors();
	#ifdef DEBUG
		Serial.println("done temp");
	#endif
	
	
	// Init 1 sec timer for pooling events
	#ifdef DEBUG
		Serial.println("Init timer1");
	#endif
	Timer1.initialize(1000000); 		// 1000000 µs --> 1s
	Timer1.attachInterrupt( timerIsr ); // attach the service routine here
	#ifdef DEBUG
		Serial.println("done timer");
		Serial.println("Starting timer1");
	#endif
	//Timer1.start();
	
	// activate interruptions
	sei();
}


// function integrated in main loop, call every time when not in timer 1s called procedure
void freakyCampManagerLoop(){
	uint8_t rcode;
	uint8_t msg[64] = { 0x00 };
	const char* recv = "Received: "; 

	//checkCampInputs();
	Usb.Task();
	if( adk.isReady() == false ) {
		return;
	}

	uint16_t len = 64;
   
	rcode = adk.RcvData(&len, msg);
	if( rcode & ( rcode != hrNAK )) {
		USBTRACE2("Data rcv. :", rcode );
	} 
	if(len > 0) {
		Serial.println("\r\nData Packet.");

		for( uint8_t i = 0; i < len; i++ ) {
			Serial.print((byte)msg[i]);
		}
	}
	//else Serial.print(".");
	delay(50);
}

void sendAllTelemetry(){
	byte len;
	len = tmBuilderTemperature(txBuffer);
	sendTM(len, txBuffer);
/*	len = tmBuilderSwitch(txBuffer);
	sendTM(len, txBuffer);
	len = tmBuilderTension(txBuffer);
	sendTM(len, txBuffer);
	len = tmBuilderCurrent(txBuffer);
	sendTM(len, txBuffer);	
	len = tmBuilderWater(txBuffer);
	sendTM(len, txBuffer);
	len = tmBuilderLight(txBuffer);
	sendTM(len, txBuffer);
	len = tmBuilderColdHot(txBuffer);
	sendTM(len, txBuffer);*/
}


void campManagerPool(){
	
	getWaterInfos();		// en premier pour être bien calé sur la seconde pour le débitmètre
	getTemperatures();
	getCurrents();
	getTensions();
	getInputs();
	
	sendAllTelemetry();
}

void getWaterInfos() {
}

void getTemperatures() {
	poolTemperatureSensors();
}

void getCurrents() {
	poolCurrentSensors();
}

void	getTensions() {
	poolTensionSensors();
}

void	getInputs() {
}

void freakyCampUSBPool(){
	uint16_t len = 0;
	uint8_t rcode;
	const char* recv = "Received: "; 

	Usb.Task();

	if( adk.isReady() == false ) {
		return;
	}
	//uint8_t RcvData(uint16_t *nbytesptr, uint8_t *dataptr);
	rcode = adk.RcvData(&len, &rxBuffer[0]);
	if( rcode & len > 0) {
		processReceivedCommand(rxBuffer, len);
	}
}

/// --------------------------
/// Custom ISR Timer Routine
/// --------------------------
void timerIsr()
{
	campManagerPool();
}





