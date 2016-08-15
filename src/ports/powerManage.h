/*
* Battery manager header code
*/

#ifndef _BATTERY_MANAGER_H
#define _BATTERY_MANAGER_H

#define MAX_AV_COUNT           3
#define MAX_AAV_COUNT          5

enum
{
    temperature_normal,
    temperature_abnormal,
    temperature_full,       // this menans the battery is not pluged in, or is pluged out
};

enum
{
	BATT_L00 = 0x00,
    BATT_L01,	// do not to modify
    BATT_L02,
    BATT_L03,
    BATT_L04,
    BATT_L05,
    BATT_L06,
    BATT_L07,
    BATT_L08,
    BATT_L09,
    BATT_L10,
    BATT_L11,
    BATT_L12,
    BATT_L13,
    BATT_L14,
    BATT_L15,
    BATT_L16,
    BATT_L17,
    BATT_L18,
    BATT_L19,
    BATT_L20,

    BATT_EMPTY      = BATT_L04, //3.35v
    BATT_LOW2       = BATT_L07, //3.50v
    BATT_LOW1       = BATT_L09, //3.60v
    BATT_NORMAL1    = BATT_L11, //3.70v
    BATT_NORMAL2    = BATT_L15, // unused
    BATT_FULL       = BATT_L17, //4.00v

	BATT_TOP
};

/*
*
*/
typedef struct
{
    uint8 currentVoltArray[MAX_AV_COUNT];		/* CV,  the current level of Battery voltage*/
    uint8 currentAverVolt;						/* AV,  the current average voltage value of battery, this is for LED PWM */
    uint8 averageVoltArrary[MAX_AAV_COUNT];     /* AV arrary, the average voltage arrary of battery,*/
    uint8 averageAverVolt;						/* AAV, this is for low battery display */
    
    uint8 minVolt;								/*MV, the min adc value of battery voltage*/
	uint16 chargingCount;						// record the charging time
	uint8 battChargingStep;						// record the charging step
	
}battery_type;

void batteryMonitor(void);
void batteryLoop(void);
void batteryLowLoop(void);
uint8 batteryIsNormal(void);
uint8 getTemperatureStatus(uint8 channel);
void batteryDcStatus(void);

#endif  // _BATTERY_MANAGR_H

