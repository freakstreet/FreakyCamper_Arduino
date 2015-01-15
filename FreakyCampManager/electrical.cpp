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

void lightCommand(byte lightId,byte dimm, byte red, byte green, byte blue){
	
}

void lightOff(byte lightId)
{
	
}

boolean getRelayStatus(eElecRelays relayId){
	return digitalRead(electricalLinesPins[relayId]);
}

