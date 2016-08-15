
#ifndef _PRIVATE_H
#define _PRIVATE_H


// Error Number
#define ERR_NONE		0
#define ERR_MEMSET		1
#define ERR_MSG_FULL	2
#define ERR_TX_FULL		3
#define EER_I2C			4
#define EER_LED_FULL	5
#define ERR_BATT_NTC    6


typedef enum
{
	StateLimbo,
	//StateReadyOn,		// power button pressed by someone, but not power on, the unit need to display led first.
	StatePowerOn	// really power on state
}osState_type;

enum
{
    StateBtNone,
    StateBtPowerOn,
    StateBtPairing,
    StateBtConnected,
    StateBtPlay,
    StateBtIncomingCall,
    StateBtOutgoingCall,
    StateBtAnswer,
};

enum
{
	DcNotCharging,
	DcCharging,
	DcChargingComplete,
	DcError,

	normal,	// this is for chargerSpeed
};

enum
{
    SceneNoAnyChargers,    /*no charger in or charger out*/
    SceneDcInOnly,          /*DC in, but there is no charger out*/
    SceneChargerOutOnly,   /*charging out, but there is no charger in*/
    SceneBothOfChargers,   /*charging in and charging out*/
};

enum
{
    BatteryNone,
    BatteryInternal,
    BatteryExternal,
    BatteryBoth,
};





typedef struct
{
	// timers
	uint8 tick;
	uint8 timerMsgTick;		// timer for message loop
	uint8 timerTick;		// timer tick, uint 1ms
	uint8 timerDelay;		// timer for delay function

	// machine status
	uint8 state;

	// DC in
	uint8 DcConnected;
	uint8 DcOnOff;
    uint8 DcOnOff2;
	uint8 DcStartDetect;
	uint8 DcStatus;
    uint8 DcStatus2;
    uint8 DcLedCnt;
    uint8 DcLedLowCnt;
	uint8 DcSpeed;
    uint8 DcScenes;

    // charge out
    uint8 chargerOut1Connnected;
    uint8 chargerOut2Connnected;
    uint8 chargerOut1OnOff;
    uint8 chargerOut2OnOff;
    
    // aux in
    uint8 auxIn1Connected;
    uint8 auxIn2Connected;
    //capsense and lock
    uint8 capsense;
    uint8 lock;
    uint8 ledInitialising;

    // Bluetooth
    uint8 btState;
    
	// battery
	uint8 battInitialising;							// check if this is the firt time to handle battery monitor
    uint8 battLowFlag;
    uint8 battLowCnt;
    uint8 temperatureStatus;					// record the temperature status of internal battery;
    uint8 temperatureStatus2;                   // external battery
    uint8 battUsing;
    
	uint8 battLevel;        //the level of internal battery
    uint8 battLevel2;       // the level of external battery

    //audio
    uint8 source;

	//amplifier
	uint8 ampOnOff;

	uint8 volume;
}application_type;

extern application_type gProject;

/*----------------OS Functions-------------------------*/
void osInit(void);
void osRunning(void);
void osEventIndexInit(void);
void osMessageHandler(message_type *msg);

void osMemset(const void *src, uint8 value, uint8 len);
void osStop(uint8 errno);
void osDelay1ms(uint8 count);
void osSetState(osState_type state);
osState_type osGetState(void);



/*------------------Misc. Functions---------------------*/
void userInit(void);
void initData(void);



#endif // _PRIVATE_H

