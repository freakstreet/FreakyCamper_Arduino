#ifndef MASTERCONF_H
#define MASTERCONF_H

#include <Arduino.h>
#include <DallasTemperature.h>
#include <Button.h>


//*****************************************************************************************************
//	ELECTRICAL MODULE
//*****************************************************************************************************
#define		NB_ELEC_SWITCHES			6

#define		POUT_RELAY_WATER		 	33	// 	http://www.seeedstudio.com/depot/Grove-SPDT-Relay30A-p-1473.html?cPath=39_42
#define		POUT_RELAY_FRIDGE		34	//	http://www.seeedstudio.com/depot/Grove-Relay-p-769.html?cPath=39_42
#define		POUT_RELAY_HEATING		35	//	http://www.seeedstudio.com/depot/Grove-Relay-p-769.html?cPath=39_42
#define		POUT_RELAY_AUX			40	// 	http://www.seeedstudio.com/depot/Grove-SPDT-Relay30A-p-1473.html?cPath=39_42
#define		POUT_RELAY_SPARE			41	//	http://www.seeedstudio.com/depot/Grove-Relay-p-769.html?cPath=39_42
#define		POUT_RELAY_LIGHT			42

typedef enum eElecRelays {
	EL_RELAY_WATER = 0,
	EL_RELAY_FRIDGE = 1,
	EL_RELAY_HEATING = 2 ,
	EL_RELAY_CAR_PLUG = 3,
	EL_RELAY_SPARE = 4,
	EL_RELAY_LIGHT = 5
};


extern byte electricalLinesPins[];



//*****************************************************************************************************
//	CURRENT MODULE
//*****************************************************************************************************
#define		NB_CURRENT_SENSORS 		7

#define		PIN_CURRENT_COLD_MODULE	A0
#define		PIN_CURRENT_WATER_MODULE	A1
#define		PIN_CURRENT_HEAT_MODULE	A2
#define		PIN_CURRENT_LIGHT_MODULE	A3
#define		PIN_CURRENT_AUX_MODULE	A4
#define		PIN_CURRENT_SPARE_MODULE	A5
#define		PIN_CURRENT_SOLAR_GENE	A6
/*
#define		PIN_CURRENT_BATT_AUX		00
#define		PIN_CURRENT_220V_CHARGE	00
#define		PIN_CURRENT_SOLAR_REGUL	00
*/

extern byte currentPins[];

#define		CURRENT_FACTOR_COLD_MODULE	1.0
#define		CURRENT_FACTOR_WATER_MODULE	1.0
#define		CURRENT_FACTOR_HEAT_MODULE	1.0
#define		CURRENT_FACTOR_LIGHT_MODULE	1.0
#define		CURRENT_FACTOR_AUX_MODULE	1.0
#define		CURRENT_FACTOR_SPARE_MODULE	1.0
#define		CURRENT_FACTOR_SOLAR_GENE	1.0

extern float currentCalFactors[];

typedef enum eCurrentSensorId{
	CURRENT_COLD_MODULE		= 0,
	CURRENT_WATER_MODULE	= 1,
	CURRENT_HEAT_MODULE		= 2,
	CURRENT_LIGHT_MODULE		= 3,
	CURRENT_AUX_MODULE		= 4,
	CURRENT_SPARE_MODULE	= 5,
	CURRENT_SOLAR_GENE		= 6
};

extern short currents[];




//*****************************************************************************************************
//						TENSION MODULE
//*****************************************************************************************************
#define		NB_TENSION_SENSORS 		2

#define		PIN_TENSION_BATT_PRIM		A9
#define		PIN_TENSION_BATT_AUX		A8

/*
#define		PIN_TENSION_SOLAR_PANEL	00
*/

extern byte tensionPins[];

#define		TENSION_FACTOR_BATT_PRIM	1.0
#define		TENSION_FACTOR_BATT_AUX 	1.0

extern float tensionCalFactors[];

extern short tensions[];


//*****************************************************************************************************
//						LIGHTS MODULE
//*****************************************************************************************************
#define		NB_LIGHTS				2

#define		POUT_LIGHT_SSD_1			36	//	http://www.seeedstudio.com/depot/Flexible-LED-Strip-White-p-1122.html?cPath=81_79
#define		POUT_LIGHT_SSD_2			37
//#define		POUT_LIGHT_SSD_3			38
//#define		POUT_LIGHT_SSD_4			39
#define		POUT_LIGHT_RGB_1_R		5
#define		POUT_LIGHT_RGB_1_G		6
#define		POUT_LIGHT_RGB_1_B		7

#define	LIGHT_PIN_SW		0
#define	LIGHT_TYPE		1
#define	LIGHT_POUT_R		2
#define	LIGHT_POUT_G		3
#define	LIGHT_POUT_B		4
#define	LIGHT_STAT_DIMM	5
#define 	LIGHT_STAT_R		6
#define 	LIGHT_STAT_G		7
#define 	LIGHT_STAT_B		8

typedef enum eLightTypes {
	LIGHT_TYPE_NORMAL,
	LIGHT_TYPE_DIMMABLE,
	LIGHT_TYPE_RGB_DIMMABLE
};

typedef enum eLights {
	LIGHT_SSD_1,
	LIGHT_SSD_2,
	LIGHT_SSD_3,
	LIGHT_SSD_4	
};

#define		NB_INPUT_SWITCHS			3

#define		PINT_LIGHT_SWITCH_1		10	// 	INT	Input Analogique
#define		PINT_LIGHT_SWITCH_2		11	// 	INT	Input Analogique
#define		PINT_LIGHT_SWITCH_3		12	// 	INT	Input Analogique
//#define		PINT_LIGHT_SWITCH_4		13	// 	INT	Input Analogique

typedef enum eSwitchId{
	SWITCH_BUTTON_LIGHT_1 = 0,
	SWITCH_BUTTON_LIGHT_2 = 1,
	SWITCH_BUTTON_LIGHT_3 = 2,
	SWITCH_BUTTON_LIGHT_4 = 3
};

extern byte lightsConf[][9];
extern Button* inputSwitchs[];


//*****************************************************************************************************
//						WATER MODULE
//*****************************************************************************************************
#define	PINT_WATER_FLOW_OUT			18	//	INT	http://www.seeedstudio.com/depot/G1-Water-Flow-Sensor-p-1671.html?cPath=25

#define	TANK_WATER_LEVEL_INPUT		A10
#define 	TANK_WATER_LEVEL_FACTOR		0.229
#define 	TANK_WATER_LEVEL_OFFSET		50	

// L'entrée de trop plein du réservoir eaux grises est en pull up --> à 1 quand pas plein
#define		PIN_TANK_DARK_FULL			30	//	http://www.seeedstudio.com/depot/water-level-switch-p-565.html?cPath=25

// constants
#define		CONST_PUMP_WATER_FACTOR		0.0	// factor for result in mA
#define		CONST_FLOW_SENSOR_FACTOR 	5.5

#define		PIN_WATER_SWITCH_EVIER	31
//#define		PIN_WATER_SWITCH_SHOWER	32		-> no switch output




//*****************************************************************************************************
//						TEMPERATURE MODULE
//*****************************************************************************************************
#define		PIN_TEMPERATURE_DS8020	15	//	One wire I/O

#define 		NB_TEMP_SENSORS			7

typedef enum eTempSensorId{
	TEMP_FRIDGE = 0,
	TEMP_INSIDE_1 = 1,
	TEMP_INSIDE_2 = 2,
	TEMP_WATER_PRIMARY = 3,
	TEMP_WATER_SECONDARY = 4,
	TEMP_HEAT_BLOWER = 5,
	TEMP_OUTSIDE = 6
};

extern DeviceAddress TempProbes[NB_TEMP_SENSORS];



//*****************************************************************************************************
//						COLD MODULE
//*****************************************************************************************************
#define		MIN_FRIDGE_TEMP			-5
#define		MAX_FRIDGE_TEMP			10



//*****************************************************************************************************
//						HEAT MODULE
//*****************************************************************************************************
#define		POUT_WEBASTO_FAN_1		02
#define		POUT_WEBASTO_FAN_2		03



//*****************************************************************************************************
//						OTHERS
//*****************************************************************************************************
#define		PINT_CAR_SWITCHED_ON		11	// 	INT




#endif





