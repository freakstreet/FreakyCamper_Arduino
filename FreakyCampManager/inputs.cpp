#include "inputs.h"

void initInputs(){
/*	for (byte i=0; i<NB_INPUT_SWITCHS; i++){
		// set PIN as input
		pinMode(inputs_switchs[i], INPUT);
		// set PIN as PULL-UP
		digitalWrite(inputs_switchs[i], HIGH);
	}
*/	
	cli();				// switch interrupts off while messing with their settings  
	PCICR =0x02;          	// Enable PCINT1 interrupt
	PCMSK1 = 0b00000111;
	sei();
}

// a modifier, les interrupteurs seront placés sur interruption
// pour une meilleure réactivité
void checkCampInputs() {
	for (byte i=0; i<NB_INPUT_SWITCHS; i++)
	{
		Button* bt = inputSwitchs[i];
		bt->read();
		
		if (bt->wasReleased()) {
			
			boolean state = digitalRead(lightsConf[i][1]);
			digitalWrite(lightsConf[i][1], !state);
		}
	}
}

