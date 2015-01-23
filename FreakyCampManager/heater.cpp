#include "heater.h"

void poolHeatInfos(){
	// Do nothing, 
}



void heaterModuleInit()
{
	
}// end of heater module init

void heaterGotTC(eHeaterState status, byte fan1val, byte fan2val, byte newTemp){
	int temp;
	temp = newTemp/2;
	switch(status){
		case HEATER_OFF :
			analogWrite(POUT_WEBASTO_FAN_1, 0);
			analogWrite(POUT_WEBASTO_FAN_2, 0);
			break;
			
		case HEATER_MANUAL :
			analogWrite(POUT_WEBASTO_FAN_1, fan1val);
			analogWrite(POUT_WEBASTO_FAN_2, fan2val);
			break;
			
		case HEATER_AUTO :
			break;
			
		case HEATER_HOT_MANUAL :
			analogWrite(POUT_WEBASTO_FAN_1, fan1val);
			analogWrite(POUT_WEBASTO_FAN_2, fan2val);
			break;
			
		case HEATER_HOT_AUTO :
			break;
		
		
	}
	
}





