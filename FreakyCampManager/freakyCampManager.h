#ifndef FREAKYCAMPMANAGER_H
#define FREAKYCAMPMANAGER_H




void freakyCampManagerStart();
void freakyCampManagerLoop();
void freakyCampUSBPool();

void timerIsr();

void getWaterInfos();	
void getTemperatures();
void getCurrents();
void getTensions();
void getInputs();	
	
	
#endif