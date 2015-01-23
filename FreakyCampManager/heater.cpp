#include "heater.h"
#include "webasto.h"
#include "masterConf.h"


eHeaterState actualState, askedState;
byte targetTemp;
boolean transientState;

void computeFanSpeedsAuto(byte* f1, byte* f2);



void heaterModuleInit()
{
	targetTemp = 20;
	transientState = false;
	actualState = HEATER_OFF;
	askedState = actualState;
	pinMode(POUT_WEBASTO_FAN_1, OUTPUT);
	pinMode(POUT_WEBASTO_FAN_2, OUTPUT);
	
}// end of heater module init

void switchToFreshAir(){
	
}

void switchToHotAir(){

}

void handleHeater(){
	byte f1, f2;
	if (needHeaterOn){
		startWebasto();
		switchToHotAir();
	}
	else{
		switchToFreshAir();
		if (!useHotWater)
			stopWebasto();
	}
	
	switch(actualState){
		case HEATER_OFF :
			if (askedState != HEATER_OFF){
				if (isWebastoOn()){
					stopWebasto();
				}
				analogWrite(POUT_WEBASTO_FAN_1, 0);
				analogWrite(POUT_WEBASTO_FAN_2, 0);
				askedState == HEATER_OFF;
				return;
			}
			break;
			
		case HEATER_HOT_AUTO :
		case HEATER_AUTO :
			computeFanSpeedsAuto(&f1, &f2);
			
		case HEATER_HOT_MANUAL :
		case HEATER_MANUAL :

			analogWrite(POUT_WEBASTO_FAN_1, f1);
			analogWrite(POUT_WEBASTO_FAN_2, f2);
			break;
		
		default :
			return;
	}
}

void startHeatingMachine(){
	// TODO ensure to be able to switch on the webasto
}

void computeFanSpeedsAuto(byte* f1, byte* f2){
	byte pwm;
	byte t;
	f1 = &pwm;
	f2 = &pwm;
}
	


void heaterGotTC(eHeaterState status, byte fan1val, byte fan2val, byte newTemp){
	targetTemp = newTemp/2;
	switch(status){
		case HEATER_OFF :
			needHeaterOn = false;
			analogWrite(POUT_WEBASTO_FAN_1, 0);
			analogWrite(POUT_WEBASTO_FAN_2, 0);
			break;
			
		case HEATER_AUTO :
			// override the TC fan pwm values
			computeFanSpeedsAuto(&fan1val, &fan2val);
		
		case HEATER_MANUAL :
			needHeaterOn = false;
			if (actualState == status){
				analogWrite(POUT_WEBASTO_FAN_1, fan1val);
				analogWrite(POUT_WEBASTO_FAN_2, fan2val);
			}
			else {
				askedState = status;
			}
			break;
			
		case HEATER_HOT_AUTO :
			// override the TC fan pwm values
			computeFanSpeedsAuto(&fan1val, &fan2val);

		case HEATER_HOT_MANUAL :
			needHeaterOn = true;
			if (actualState == status){
				analogWrite(POUT_WEBASTO_FAN_1, fan1val);
				analogWrite(POUT_WEBASTO_FAN_2, fan2val);
			}
			else {
				askedState = status;
			}
			break;
			

		
		
	}
	
}





