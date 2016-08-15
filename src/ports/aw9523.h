/*
	AW9523B Driver
*/

#ifndef _AW9523_H
#define _AW9523_H

// I2C address
#define AW9523_WRITE_ADDR			0xB6
#define AW9523_READ_ADDR			0xB7

// AW9523 Registers, see detail in datasheet
#define REG_INPUT_PORT0				0x00
#define REG_INPUT_PORT1				0x01
#define REG_OUTPUT_PORT0			0x02
#define REG_OUTPUT_PORT1			0x03
#define REG_CONFIG_PORT0			0x04
#define REG_CONFIG_PORT1			0x05
#define REG_INT_PORT0				0x06
#define REG_INT_PORT1				0x07
#define REG_ID						0x10
#define REG_CTL						0x11
#define REG_LED0_MODE				0x12
#define REG_LED1_MODE				0x13
#define REG_DIM_BASE				0x20
#define REG_SW_RSTN					0x7F


#define AW9523_ID					0x23

//
#define LED_POWER			0x20		// P1.0
#define LED_BATT1			0x25		// P0.1
#define LED_BATT2			0x24		// P0.0
#define LED_BATT3			0x23		// P1.3
#define LED_BATT4			0x22		// P1.2
#define LED_LOCK			0x21		// P1.1
#define LED_UNLOCK			0x26		// P0.2
#define LED_BT			    0x29		// P0.5
#define LED_SOURCE			0x2F		// P1.7
#define LED_PLAY			0x2C		// P1.4
#define LED_VOL_DOWN	    0x2D		// P1.5
#define LED_VOL_UP		    0x2B		// P0.7
#define LED_BACKWARD		0x2A		// P0.6
#define LED_FORWARD			0x2E		// P1.6

#define LED_GROUP1			0xF0        // why it can not use the "0x7F" and "0xFF", because I write it to ledOnOff directly
#define LED_GROUP2			0xF1
#define LED_GROUP3			0xF2

#define LED_NONE			0x00
#define LIMITLESS			0x00

typedef struct
{
    uint8 time;
    uint8 onTime;
    uint8 offTime;
    uint8 count;
}blink_type;

typedef struct
{
    uint8 stepKeepTime;
    uint8 time;
    uint8 count;
    uint8 keyAction;
    uint8 keyRelease;
}dim_type;

typedef struct
{
    uint8 led;
    uint8 type;
    uint8 direction;
    uint8 step;
    
    union
    {
        dim_type dim;
        blink_type blink; 
    }content;
    
}led_type;

#define TYPE_BLINK			0
#define TYPE_DIM			1

#define DIM_STEP_MAX		20
#define DIM_MS(x)			(uint8)(x/DIM_STEP_MAX)

// functions
void ledInit(void);
void ledAllOnOff(uint8 onOff);
void ledOnOff(uint8 led, uint8 onOff);
uint8 ledClear(uint8 led);
uint8 ledGetDirection(uint8 led);

void ledBlinkConfigure(uint8 led, uint8 onTime, uint8 offTime, uint8 count);
void ledBlinkLoop(void);


void ledDimConfigure(uint8 led, uint8 step_time, uint8 count, uint8 action);
void ledDimDown(uint8 led);
void ledDimLoop(void);

void ledSetMask(uint8 led);
void ledClearMask(uint8 led);

#endif // _AW9523_H

