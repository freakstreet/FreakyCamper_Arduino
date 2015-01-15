#ifndef ELECTRICAL_H
#define ELECTRICAL_H

#include "Arduino.h"
#include "masterConf.h"

void initElectricalFunction();

void switchRelay(byte relayId, boolean status);

void lightCommand(byte lightId,byte dimm, byte red, byte green, byte blue);
void lightOff(byte lightId);
boolean getRelayStatus(eElecRelays relayId);
		


#endif