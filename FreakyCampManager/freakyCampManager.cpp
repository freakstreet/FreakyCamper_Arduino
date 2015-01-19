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
#include "water.h"

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

/*	USB Error codes list
TODO : manage each error type to handle eliable USB link
----------------------------------------------------------------------------------------------
	HSRLT  |	Label	|	Meaning
----------------------------------------------------------------------------------------------
	0x00	hrSUCCESS 	Successful Transfer
	0x01	hrBUSY 		SIE is busy, transfer pending
	0x02	hrBADREQ	Bad value in HXFR reg
	0x03	hrUNDEF	(reserved)
*	0x04	hrNAK		Peripheral returned NAK
	0x05	hrSTALL		Perpheral returned STALL
	0x06	hrTOGERR	Toggle error/ISO over-underrun
*	0x07	hrWRONGPID	Received the wrong PID
	0x08	hrBADBC		Bad byte count
*	0x09	hrPIDERR	Receive PID is corrupted
	0x0A	hrPKTERR	Packet error (stuff, EOP)
	0x0B	hrCRCERR	CRC error
	0x0C	hrKERR		K-state instead of response
	0x0D	hrJERR		J-state instead of response
	0x0E	hrTIMEOUT	Device did not respond in time
	0x0F	hrBABBLE	Device talked too long

*/


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
					Serial.print("Got TC :");
					for( uint8_t i = 0; i < rxCounter; i++ ) {
						Serial.print(" 0x");
						Serial.print(rxBuffer[i], HEX);
					}
					Serial.println("");

					processReceivedCommand(rxCounter);

					watchdog = 0;
					footerCount = 0;
					readyForDatas = false;
					rxCounter = 0;
				}
			}
			else if (readyForDatas) 
				rxBuffer[rxCounter++] = msg[0];
				
		}
	}
	delay(25);
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
	    
void sendTM(byte length){

	uint16_t pos = 0;
	byte i, ack;
	
	if (length==0) return;
	
	// prepare header
	for (i=0; i<PROT_REPEAT_HEADER;i++) 
		txBuffer[pos++] =PROT_HEADER;
	// copy datas
	for (i=0; i<length;i++)
		txBuffer[pos++] = txBuffer[i];
		
	// prepare footer
	for (i=0; i<PROT_REPEAT_HEADER;i++) 
		txBuffer[pos++] =PROT_FOOTER;
	
	ack = adk.SndData(pos, txBuffer);
	
	#ifdef DEBUG
		printTM(pos);
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
	Timer1.initialize(1000000*PROT_TM_CYCLE_SEC); 		// 1000000 µs --> 1s
	Timer1.attachInterrupt( timerIsr ); // attach the service routine here
	#ifdef DEBUG
		Serial.println("done timer");
		Serial.println("Starting timer1");
	#endif

	// activate interruptions
	sei();
}




void sendAllTelemetry(){
	byte len;
	len = tmBuilderTemperature();
	sendTM(len);
	len = tmBuilderSwitch();
	sendTM(len);
	len = tmBuilderTension();
	sendTM(len);
	len = tmBuilderCurrent();
	sendTM(len);	
	len = tmBuilderWater();
//	sendTM(len);
	len = tmBuilderLight(0);
	sendTM(len);
	len = tmBuilderColdHot();
//	sendTM(len);
}


void campManagerPool(){
	
	getWaterInfos();		// en premier pour être bien calé sur la seconde pour le débitmètre
	getTemperatures();
	getCurrents();
	getTensions();
	getInputs();
	
	sendAllTelemetry();
	
	watchdog++;
	if (watchdog > PROT_WATCHDOG_MAX_SEC/PROT_TM_CYCLE_SEC){
		// start reset USB connection
		
		watchdog = 0;
	}
}

void getWaterInfos() {
	poolWaterInfos();
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

/// --------------------------
/// Custom ISR Timer Routine
/// --------------------------
void timerIsr()
{
	campManagerPool();
}





