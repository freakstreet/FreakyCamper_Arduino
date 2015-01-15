#include "water.h"

byte waterFlowLitersMin;
long flowSensorIntsCount;

void onWaterFlowSensorInterrupt()
{
	flowSensorIntsCount++;
}

void waterModuleInit()
{
	waterFlowLitersMin = 0;
	flowSensorIntsCount = 0;
	
	// Init interruptions
	pinMode(PINT_WATER_FLOW_OUT,INPUT);  //initializes digital pin as an input
	attachInterrupt(0, onWaterFlowSensorInterrupt,RISING);  
}// end of water module init


byte isDarkWaterTankFull()
{
	// PULL UP - entr�e � 1 lorsque le r�servoir est plein.
	return !digitalRead(PIN_TANK_DARK_FULL);
}

byte getCleanWaterLevel()
{
	byte res;
	int meas = analogRead(TANK_WATER_LEVEL_INPUT);
	if (meas-TANK_WATER_LEVEL_OFFSET<0)
		meas = TANK_WATER_LEVEL_OFFSET;
	
	res = (byte)((meas-TANK_WATER_LEVEL_OFFSET)*TANK_WATER_LEVEL_FACTOR);
	
	if (res > 100)
		return 100;
	else
		return res;
}// 0 (empty) to 100 (full)
     

boolean isPumpActive()
{
	return (getPumpConsoMa()>1);
}// 1 if active, 0 is off


boolean getPumpRelayStatus()
{
	return digitalRead(POUT_RELAY_WATER);
}// 1 if active, 0 is off

unsigned long getPumpConsoMa()
{
	int tmp = analogRead(PIN_CURRENT_WATER_MODULE);

	return tmp*CONST_PUMP_WATER_FACTOR;
}

boolean isEvierOpened() {
	return digitalRead(PIN_WATER_SWITCH_EVIER);
}

// Assimile douche ouverte si d�bit d'eau existant et pas d'ouverture �vier
boolean isShowerOpened() {
	return (waterFlowLitersMin > 1) && !isEvierOpened();
}

void onIntWaterFlowSensor(){
	flowSensorIntsCount++;
}

void onWaterFlowTimer(){
	waterFlowLitersMin = flowSensorIntsCount * 60 / CONST_FLOW_SENSOR_FACTOR;
	flowSensorIntsCount = 0;
}






