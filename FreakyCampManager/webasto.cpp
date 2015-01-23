//#include <max6675.h>

// Define constants
#define ON 	1
#define OFF 	0
#define REMOTE_STARTING 	2
#define REMOTE_STOPPING 	3
#define REMOTE_SHUTDOWN 	4
#define SYSTEM_STOPPING 	5
#define SYSTEM_SHUTDOWN 	6

#define DELAY_60_SEC	60000
#define DELAY_15_SEC	15000
#define DELAY_5_SEC	5000

#define ON_STATE_CHANGE_INTERRUPT	true
#define NO_STATE_CHANGE_INTERRUPT	false
#define ON_TEMP_CHANGE_INTERRUPT	true
#define NO_TEMP_CHANGE_INTERRUPT	false

#define WEBASTO_MAX_START_ATTEMPTS			3
#define WEBASTO_MAX_TEMPERATURE			70
#define WEBASTO_TEMPERATURE_THRESHOLD		10

/*

// Define service variables
boolean debug = true;
volatile long lastDebounceTime = 0;

// Define global variables
int onOffSwitch	= 2;
int airFan          	= 3;
int fuelPump        	= 4;
int waterPump       	= 5; 
int glowPlug        	= 6;
int cabinHeater     	= 8;
int remoteControl   	= 9;
int carBlinkers     	= 10;

// Thermocouple 0 = CS, 1 = CLK, 2 = DO
int tempSensor[3]   = { 11, 12, 13 };

// Setting
volatile int startingAttempts	= 0;
double initialTemperature;
double previousTemperature;

// Define system states (ON, OFF, REMOTE_STARTING, REMOTE_STOPPING, REMOTE_SHUTDOWN, SYSTEM_STOPPING, SYSTEM_SHUTDOWN)
volatile int systemState 	= OFF;

// Define modules states
int airFanState			= OFF;
int waterPumpState		= OFF;
int glowPlugState		= OFF;
int cabinHeaterState	= OFF;
int remoteControlState	= ON;

// Initialize temperature sensor
MAX6675 thermocouple(tempSensor[1], tempSensor[0], tempSensor[2]);

// Setup the controller
void setup(){

    // Is debug on
    if(debug == true){

        // Enable serial connection
        Serial.begin(9600);
    }

    // Set modules modes
    pinMode(fuelPump, OUTPUT);
    pinMode(waterPump, OUTPUT);
    pinMode(glowPlug, OUTPUT);
    pinMode(airFan, OUTPUT);
    pinMode(cabinHeater, OUTPUT);
    pinMode(carBlinkers, OUTPUT);
    pinMode(remoteControl, OUTPUT);

    // Set ON/OFF switch to ON (internal pull-up resistor)
    digitalWrite(onOffSwitch, HIGH);

    // Attach interrupt
    attachInterrupt(0, turnOnOffSystem, CHANGE);

    // Reset all modules
    resetAllModules();
}

// Start the controller
void loop(){

	// If system has to be started or is already started
	if(systemState == REMOTE_STARTING || systemState == ON){
		// If starting from remote
		if(systemState == REMOTE_STARTING){
			// Blink the sideblinkers
			if(blink(2, true, false)) return;
		}
		// Set initial temperature
		initialTemperature = measureTemp(3);
		// If temperature is above max temperature level or temperature sensor is off
		if(!initialTemperature || initialTemperature >= WEBASTO_MAX_TEMPERATURE){
			// If heater was running
			if(systemState == ON){
				// Set system state
				systemState = SYSTEM_STOPPING;
			}
			// If heater is starting
			else {
				// Set system state
				systemState = SYSTEM_SHUTDOWN;
			}
			// Stop webasto starting cycle
			return;
		}
		// Is debug on
		if(debug == true){
			// Log
			Serial.print("Initial temperature: ");
			Serial.print(initialTemperature);
			Serial.println();
		}
		// Start the Webasto heater
		startWebasto();
	}

	// If system has to be stopped
	else if(systemState == REMOTE_STOPPING || systemState == SYSTEM_STOPPING){
	// Stop remote control receiver
	stopRemoteControlReceiver();
	// If stopping from remote
	if(systemState == REMOTE_STOPPING){
			// Blink the sideblinkers
			blink(4, false, false);
		}
		// Stop the Webasto heater
		stopWebasto();
	}

	// If system has to be shutdown
	else if(systemState == REMOTE_SHUTDOWN || systemState == SYSTEM_SHUTDOWN){
		// If shutting down from remote
		if(systemState == REMOTE_SHUTDOWN){
			// Blink the sideblinkers
			if(blink(4, true, false)) return;
		}
		// Reset all modules
		resetAllModules();
		// Set system state
		systemState = OFF;
	}
}

// Turn ON/OFF system from remote control
void turnOnOffSystem(){

    // Get current time
    long currentTime = millis();

    // Debounce
    if ((currentTime - lastDebounceTime) > 100){

        // Record last debounce time
        lastDebounceTime = currentTime;

        // If system is still starting
        if(systemState == REMOTE_STARTING){

            // Turn system off immediately
            systemState = REMOTE_SHUTDOWN;
        }

        // If system is on
        else if(systemState == ON){

            // Turn system off
            systemState = REMOTE_STOPPING;
        }

        // If system is off
        else if(systemState == OFF){

            // Turn system on
            systemState = REMOTE_STARTING;

            // Clear all previous starting attempts
            startingAttempts = 0;
        }
    }
}

// Start the webasto heater
void startWebasto(){

    // If this is first, second or third attempt
    if(startingAttempts < WEBASTO_MAX_START_ATTEMPTS){

        // Is debug on
        if(debug == true){

            // Log
            Serial.println("Starting the heater");
        }

        // Increment attempt
        startingAttempts++;

        // Reset all modules
        resetAllModules();

        // Start the air fan to clean the carbon
        startAirFan();

        // Wait and interrupt on system state change and temperature change
        if(wait(DELAY_15_SEC, ON_STATE_CHANGE_INTERRUPT, ON_TEMP_CHANGE_INTERRUPT)) return;

        // Stop air fan
        stopAirFan();

        // Start water pump
        startWaterPump();

        // Wait and interrupt on system state change and temperature change
        if(wait(DELAY_15_SEC, ON_STATE_CHANGE_INTERRUPT, ON_TEMP_CHANGE_INTERRUPT)) return;

        // Set system state
        systemState = ON;

        // Start glow plug
        startGlowPlug();

        // Wait and interrupt on system state change and temperature change
        if(wait(DELAY_5_SEC, ON_STATE_CHANGE_INTERRUPT, ON_TEMP_CHANGE_INTERRUPT)) return;

        // Start the combustion process
        for(int i = 0; i <= 1000; i++){

            // Supply fuel to the burner while checking for state change
            if(pulseFuelPump(true, true)) return;

            // 15th second from start of the burning process
            if(i == 15){

                // Start the air fan
                startAirFan();
            }

            // 20th second from start of the burning process
            else if(i == 20){

                // Stop the glow plug
                stopGlowPlug();
            }

            // 60th second and above from start of the burning process
            else if(i >= 60){

                // Check temperature
                double temperature = measureTemp(3);

                // If temperature sensor is off
                if(!temperature){

                    // Go to shutdown mode
                    systemState = SYSTEM_STOPPING;

                    // Stop the burning process
                    return;
                }

                // 60th second
                if(i == 60){

                    // If temperature threshold is not reached
                    if(initialTemperature + WEBASTO_TEMPERATURE_THRESHOLD > temperature){

                        // Is debug on
                        if(debug == true){

                            // Print error
                            Serial.print("Initial temperature: ");
                            Serial.print(initialTemperature);
                            Serial.print(", Current temperature: ");
                            Serial.print(temperature);
                            Serial.println();
                            Serial.println("Temperature threshold not reached!");
                        }

                        // Restart the burning process
                        return;
                    }

                    // Is debug on
                    if(debug == true){

                        // Log
                        Serial.print("Initial temperature: ");
                        Serial.print(initialTemperature);
                        Serial.print(", Current temperature: ");
                        Serial.print(temperature);
                        Serial.println();
                        Serial.println("Temperature threshold reached, continuing the burning process");
                    }

                    // Log the current temperature
                    previousTemperature = temperature;

                    // Start the cabin heater
                    startCabinHeater();
                }

                // Above 60th second
                else {

                    // If temperature has increased
                    if(temperature >= previousTemperature && temperature <= previousTemperature + 2){

                        // Log the current temperature
                        previousTemperature = temperature;
                    }

                    // If temperature has decreased
                    else if(temperature <= previousTemperature && temperature >= previousTemperature - 2){

                        // Is debug on
                        if(debug == true){

                            // Print error
                            Serial.print("Previous temperature: ");
                            Serial.print(previousTemperature);
                            Serial.print(", Current temperature: ");
                            Serial.print(temperature);
                            Serial.println();
                            Serial.println("Temperature has decreased, possible flame extinguishing!");
                        }

                        // Restart the burning process
                        return;
                    }
                }
            }

            // Wait and interrupt on system state change and temperature change
            if(wait(700, true, true)) return;
        }

        // Go to shutdown mode
        systemState = SYSTEM_STOPPING;
    }

    // If we have tried three times starting the Webasto
    else {

        // Set system state
        systemState = SYSTEM_STOPPING;
    }
}

// Stop the webasto heater
void stopWebasto(){

    // Is debug on
    if(debug == true){

        // Log
        Serial.println("Shutting down");
    }

    // Stop fuel burning and cabin heater
    stopGlowPlug();
    stopCabinHeater();

    // Start the water pump and air fan
    startWaterPump();
    startAirFan();

    // Wait without checking for system state change and temperature change
    wait(DELAY_60_SEC, NO_STATE_CHANGE_INTERRUPT, NO_TEMP_CHANGE_INTERRUPT);

    // Set system state
    systemState = OFF;

    // Reset all modules
    resetAllModules();

    // Is debug on
    if(debug == true){

        // Log
        Serial.println("Done");
    }
}

// Pulse fuel pump
boolean pulseFuelPump(boolean stateChangeInterrupt, boolean tempChangeInterrupt){

    // Start fuel pump
    digitalWrite(fuelPump, ON);

    // Sleep for 100 miliseconds and return true on state or temperature change
    if(wait(100, stateChangeInterrupt, tempChangeInterrupt)) return true;

    // Stop fuel pump
    digitalWrite(fuelPump, OFF);

    // Is debug on
    if(debug == true){

        // Log
        Serial.println("Pulse fuel");
    }

    // Return false
    return false;
}

// Start air fan
void startAirFan(){

    // Start the air fan
    digitalWrite(airFan, ON);

    // Set module status
    airFanState = ON;

    // Is debug on
    if(debug == true){

        // Log
        Serial.println("Air fan on");
    }
}

// Stop air fan
void stopAirFan(){

    // Start the air fan
    digitalWrite(airFan, OFF);

    // Set module status
    airFanState = OFF;

    // Is debug on
    if(debug == true){

        // Log
        Serial.println("Air fan off");
    }
}

// Start water pump
void startWaterPump(){

    // Start the water pump
    digitalWrite(waterPump, ON);

    // Set module status
    waterPumpState = ON;

    // Is debug on
    if(debug == true){

        // Log
        Serial.println("Water pump on");
    }
}

// Stop water pump
void stopWaterPump(){

    // Start the water pump
    digitalWrite(waterPump, OFF);

    // Set module status
    waterPumpState = OFF;

    // Is debug on
    if(debug == true){

        // Log
        Serial.println("Water pump off");
    }
}

// Start glow plug
void startGlowPlug(){

    // Start the glow plug
    digitalWrite(glowPlug, ON);

    // Set module status
    glowPlugState = ON;

    // Is debug on
    if(debug == true){

        // Log
        Serial.println("Glow plug on");
    }
}

// Stop glow plug
void stopGlowPlug(){

    // Start the glow plug
    digitalWrite(glowPlug, OFF);

    // Set module status
    glowPlugState = OFF;

    // Is debug on
    if(debug == true){

        // Log
        Serial.println("Glow plug off");
    }
}

// Start cabin heater
void startCabinHeater(){

    // Start the cabin heater
    digitalWrite(cabinHeater, ON);

    // Set module status
    cabinHeaterState = ON;

    // Is debug on
    if(debug == true){

        // Log
        Serial.println("Cabin heater on");
    }
}

// Stop cabin heater
void stopCabinHeater(){

    // Start the cabin heater
    digitalWrite(cabinHeater, OFF);

    // Set module status
    cabinHeaterState = OFF;

    // Is debug on
    if(debug == true){

        // Log
        Serial.println("Cabin heater off");
    }
}

// Start remote control receiver
void startRemoteControlReceiver(){

    // Start the remote control receiver
    digitalWrite(remoteControl, OFF);

    // Set module status
    remoteControlState = ON;

    // Is debug on
    if(debug == true){

        // Log
        Serial.println("Remote control receiver on");
    }
}

// Stop remote control receiver
void stopRemoteControlReceiver(){

    // Stop the remote control receiver
    digitalWrite(remoteControl, ON);

    // Set module status
    remoteControlState = OFF;

    // Is debug on
    if(debug == true){

        // Log
        Serial.println("Remote control receiver off");
    }
}

// Measure unit temperature
double measureTemp(int maxRetries){

    // Init temperature variable
    double temperature = NAN;

    // Loop until max retries number is reached
    for (int i = 0; i < maxRetries; i++){

        // Measure temperature
        temperature = thermocouple.readCelsius();

        // If temperature reading is ok
        if (!isnan(temperature)){

            // Exit on first good reading
            break;
        }
    }

    // If temperature sensor is off
    if (isnan(temperature)) {

        // Is debug on
        if(debug == true){

            // Print error
            Serial.println("Thermocouple not responding!");
        }

        // Return false
        return false;
    } 

    // If temperature is ok
    else {

        // Return temperature
        return temperature;
    }
}

// Reset all modules
void resetAllModules(){

    // Turn off all modules
    stopWaterPump();
    stopGlowPlug();
    stopAirFan();
    stopCabinHeater();

    // Turn on remote control receiver
    startRemoteControlReceiver();
}

// Blink car blinkers
boolean blink(int times, boolean stateChangeInterrupt, boolean tempChangeInterrupt){

    // Start the loop
    for(int i = 0; i < times; i++){

        // Turn on blinkers
        digitalWrite(carBlinkers, ON);

        // Sleep for 0.2 seconds and listen for system state and temperature change
        if(wait(200, stateChangeInterrupt, tempChangeInterrupt)) return true;

        // Turn off blinkers
        digitalWrite(carBlinkers, OFF);

        // Sleep for 0.2 seconds and listen for system state and temperature change
        if(wait(200, stateChangeInterrupt, tempChangeInterrupt)) return true;

        // Is debug on
        if(debug == true){

            // Log
            Serial.println("Blink");
        }
    }

    // Return false if system state or temperature have not changed
    return false;
}

// Custom delay
boolean wait(unsigned long milliseconds, boolean stateChangeInterrupt, boolean tempChangeInterrupt){

    // Record current time
    unsigned long currentTime = millis();

    // Get current system state
    int currentSystemState = systemState;

    // Init temperature variable
    double temperature;

    // Loop until time limit is reached
    while (millis() - currentTime <= milliseconds) {

        // If temperature change interrupt is on
        if(tempChangeInterrupt == true){

            // Measure temperature
            temperature = measureTemp(3);

            // If temperature is off or above max temperature level
            if(!temperature || temperature >= WEBASTO_MAX_TEMPERATURE){

                // Is debug on
                if(debug == true){

                    // Print error
                    Serial.print("Current temperature: ");
                    Serial.print(temperature);
                    Serial.println();
                    Serial.println("Max temperature reached!");
                }

                // If temperature critical level is reached return true
                return true;
            }
        }

        // If state change interrupt is on
        if(stateChangeInterrupt == true){

            // Check if system state has changed
            if(systemState != currentSystemState){

                // Update the current system state
                currentSystemState = systemState;

                // If system state has changed return true
                return true;
            }
        }
    }

    // Return false if system state has not changed
    return false;
}



*/


