#ifndef ELECTRICAL_H
#define ELECTRICAL_H

#include "Arduino.h"
#include "masterConf.h"

void initElectricalFunction();

void switchRelay(byte relayId, boolean status);


boolean getRelayStatus(eElecRelays relayId);
		


#endif