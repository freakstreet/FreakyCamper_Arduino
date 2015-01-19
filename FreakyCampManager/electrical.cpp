#include "electrical.h"




void initElectricalFunction(){
	int i, pin;

	// init relays statuses
	for (i=0; i<NB_ELEC_SWITCHES; i++)
	{
		pin = electricalLinesPins[i];
		pinMode(pin, OUTPUT);
		digitalWrite(pin, LOW);
	}
}

void switchRelay(byte relayId, boolean status){
	digitalWrite(relayId, status);
}



boolean getRelayStatus(eElecRelays relayId){
	return digitalRead(electricalLinesPins[relayId]);
}

