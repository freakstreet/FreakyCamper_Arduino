#include "water.h"
#include "webasto.h"

byte waterFlowLitersMin;
long flowSensorIntsCount;
boolean useHotWater;

void poolWaterInfos(){
	// Do nothing, 
}

void onWaterFlowSensorInterrupt()
{
	flowSensorIntsCount++;
}

void waterModuleInit()
{
	useHotWater = false;
	waterFlowLitersMin = 0;
	flowSensorIntsCount = 0;
	// pin activate tank level sensing set to out and 0v
	pinMode(PIO_WATER_ACTIVATE_SENSING, OUTPUT);
	digitalWrite(PIO_WATER_ACTIVATE_SENSING, LOW);
	// sensing pin set to output and set to 0v
	//pinMode(TANK_WATER_LEVEL_INPUT, OUTPUT);
	//digitalWrite(TANK_WATER_LEVEL_INPUT, LOW);
		
	// Init interruptions
	pinMode(PINT_WATER_FLOW_OUT,INPUT);  //initializes digital pin as an input
	attachInterrupt(0, onWaterFlowSensorInterrupt,RISING);  
}// end of water module init


byte isDarkWaterTankFull()
{
	// PULL UP - entrée à 1 lorsque le réservoir est plein.
	return !digitalRead(PIN_TANK_DARK_FULL);
}

byte getCleanWaterLevel()
{
	byte res;
	int adc;
	// activate measuring pin
	digitalWrite(PIO_WATER_ACTIVATE_SENSING, HIGH);
	pinMode(TANK_WATER_LEVEL_INPUT, INPUT);

	// measure several times 
	int meas = 0;
	//Serial.print("tank levelx5 ");
	for (int i=0; i<2; i++) {
		adc = analogRead(TANK_WATER_LEVEL_INPUT);
		//Serial.print(adc);
		//Serial.print(",");
		meas += adc;
	}
	meas = meas / 2;
	//Serial.print(",moy=");
	//Serial.println(meas);
	
	// desactivate sensor function
	digitalWrite(PIO_WATER_ACTIVATE_SENSING, LOW);
	//pinMode(TANK_WATER_LEVEL_INPUT, OUTPUT);
	//digitalWrite(TANK_WATER_LEVEL_INPUT, LOW);
	
	
	if (meas-TANK_WATER_LEVEL_OFFSET<0)
		meas = TANK_WATER_LEVEL_OFFSET;
	
	res = (byte)((meas-TANK_WATER_LEVEL_OFFSET)/TANK_WATER_LEVEL_FACTOR);
	
	//Serial.print("Tank level:");
	//Serial.println(res);
	
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

// Assimile douche ouverte si débit d'eau existant et pas d'ouverture évier
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

void setHotWaterMode(boolean mode){
	if (mode)
		startWebasto();
	else {
		if (!needHeaterOn)
			stopWebasto();
	}
	useHotWater = mode;
}




