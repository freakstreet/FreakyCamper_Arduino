#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "Arduino.h"

// 	see detailled protocol documentation on web page :
//	https://docs.google.com/spreadsheets/d/1y_QzfoCATAHboYDJxgC_1X2cZHdAuKI4OJYJv16NXBQ/edit#gid=1493827426

// PROTOCOL
#define PROT_HEADER			0xAA
#define PROT_FOOTER			0x55
#define PROT_REPEAT_HEADER	4

// TELECOMMANDS

#define	PROT_SWITCH_COLD_MODULE	10
#define	PROT_SWITCH_WATER_MODULE	11
#define	PROT_SWITCH_HEAT_MODULE	12
#define	PROT_SWITCH_LIGHT_MODULE	13
#define	PROT_SWITCH_AUX_MODULE		14
#define	PROT_SWITCH_SPARE_MODULE	15

#define	PROT_COLD_CONSIGNE			20
	
#define	PROT_HEATER				40

#define	PROT_SELECT_MANAGER_SOURCE	50
#define	PROT_SELECT_POWER_SOURCE	51


// TELEMETRY

#define	TM_CURRENT		70
#define	TM_TENSION		71
#define	TM_TEMPERATURE	72
#define	TM_WATER		73
#define	TM_SWITCH		74
#define	TM_LIGHT			75
#define	TM_COLD_HOT		76
#define	TM_ELECTRICAL_CONF	77

#define TELEMETRY_BUFFER_MAX_SIZE	24

extern byte rxBuffer[TELEMETRY_BUFFER_MAX_SIZE];
extern byte txBuffer[TELEMETRY_BUFFER_MAX_SIZE];

void processReceivedCommand(byte* data, int length);

byte tmBuilderCurrent(byte* data);
byte tmBuilderTension(byte* data);
byte tmBuilderTemperature(byte* data);
byte tmBuilderWater(byte* data);
byte tmBuilderSwitch(byte* data);
byte tmBuilderLight(byte* data, byte lightId);
byte tmBuilderColdHot(byte* data);

void printTM(byte* data, byte length);
void sendTM(byte length, byte* data);


#endif