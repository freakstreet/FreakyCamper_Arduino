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
	    
byte rxCounter = 0;
byte headerCount = 0;
byte footerCount = 0;
boolean readyForDatas = false;


// function integrated in main loop, call every time when not in timer 1s called procedure
void freakyCampManagerLoop(){
	uint8_t rcode;
	uint8_t msg[8] = { 0x00 };
	const char* recv = "Received: "; 

	//checkCampInputs();
	Usb.Task();
	if( adk.isReady() == false ) {
		return;
	}

	uint16_t len = 8;
   
	rcode = adk.RcvData(&len, msg);
	if( rcode & ( rcode != hrNAK )) {
		USBTRACE2("Data rcv. :", rcode );
	} 
	if(len > 0) {
		/*
		Serial.println("\r\nData Packet.");
		for( uint8_t i = 0; i < len; i++ ) {
			Serial.print("0x");
			Serial.print((byte)msg[i], HEX);
		}*/
		
		if (len == 1){
			// check data value
			if (msg[0] == PROT_HEADER) {
				footerCount = 0;
				if (headerCount < PROT_REPEAT_HEADER) headerCount++;
				// if still acquiring frame, resetting rx buffer
				if (readyForDatas) rxCounter = 0;
				// check if got all header bytes
				readyForDatas = (headerCount == PROT_REPEAT_HEADER);
			}
			else if (readyForDatas && (msg[0] == PROT_FOOTER)){
				footerCount++;
				headerCount = 0;
				// if reached end of footer
				if (footerCount == PROT_REPEAT_HEADER){
					Serial.println("");
					Serial.print("Got TC :");
					for( uint8_t i = 0; i < rxCounter; i++ ) {
						Serial.print(" 0x");
						Serial.print(rxBuffer[i], HEX);
					}
					Serial.println("");
					footerCount = 0;
					readyForDatas = false;
					
					// TODO : handle TC commane
					
					rxCounter = 0;
				}
			}
			else if (readyForDatas) 
				rxBuffer[rxCounter++] = msg[0];
				
		}
	}
	//else Serial.print(".");
	delay(25);
}
	    
void sendTM(byte length, byte* data){
	byte txBuffer[TELEMETRY_BUFFER_MAX_SIZE];
	uint16_t pos = 0;
	byte i, ack;
	
	// prepare header
	for (i=0; i<PROT_REPEAT_HEADER;i++) txBuffer[pos++] =PROT_HEADER;
	// copy datas
	for (i=0; i<length;i++){
		txBuffer[pos++] = data[i];
	}	
	// prepare footer
	for (i=0; i<PROT_REPEAT_HEADER;i++) txBuffer[pos++] =PROT_FOOTER;
	
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





