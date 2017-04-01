
#ifndef _CONFIG_H
#define _CONFIG_H

/* data class/type defines */
typedef unsigned char       uint8;
typedef unsigned int        uint16;

#ifndef NULL
#define NULL                ((void *)0)
#endif

#define TRUE        (1)
#define FALSE       (0)
#define ON          (1)
#define OFF         (0)
#define SUCCESS     (0)     // Note this please
#define FAIL        (1)
#define UP          (1)
#define DOWN        (0)


/* NEED to modify acorrding to projects  before runing*/
#define TIMER_10MS          (1)     /* it should be TIMER_10MS = Timer * counter  */
#define MSG_TICK_MS         (1)  // 10ms

/* reserved loops, you can use them to UART, SPI, RF Rx */
#define osReservedLoop1()      
#define osReservedLoop2()

/* debug system */
#define UART_ENABLE
#define DBG_STR(str)  
#define DBG_DATA(data)
#define DBG_STR_DATA(str,data)

#define osDebugInit()
#define osDebugLoop()

/* watch dog configuration */
#define osWatchDogEnable()
#define osWatchDogDisable()
#define osWatchDogClear()

/* */
typedef enum
{
    //index 0x00 ~ 0x05, for sepecial events, don't modify
	EvtLoopBack,

	// general events
	EvtEnterLimbo = 0x06,
	EvtPowerKey,
	EvtReady,
	EvtPowerOn,
	EvtPowerOff,
	
	EvtBatteryMonitor,	
    EvtBatteryCheckStart,
    EvtBatteryCheckStop,
	EvtBatteryLow,

    // charger in
	EvtDcConnected,
	EvtDcDisconnected,
	
	EvtDcOnOff,   // for internal charging on/off
	EvtDcOnOff2,  // for external charging on/off
	EvtDcStatus,
	EvtDcDetectStart,
	
    EvtChangeBattUsing,		//??
    // charger out
    EvtChargerOut1Connected,		//Out1 for internal
    EvtChargerOut1Disconnected,
    EvtChargerOut1OnOff,
    
    EvtChargerOut2Connected,		// Out2 for external
    EvtChargerOut2Disconnected,
    EvtChargerOut2OnOff,
    
	//Aux
	EvtAuxIn1Connected,			//Aux1 for Internal
	EvtAuxIn1Disconnected,
    EvtAuxIn2Connected,			//Aux2 for external
    EvtAuxIn2Disconnected,

    // capsense
    EvtCapsense,
    // Lock
    EvtLock,
    //LED
    EvtIndications,
    EvtDim,
    
	EvtVolSub,
	EvtVolAdd,
	EvtForward,
	EvtBackward,
	EvtBtKey,
	EvtBtPowerOnOff,
	EvtBtState,
	EvtBtBqbTest,
	EvtBtEqTest,
	EvtPairing,
	EvtPlayKey,
	EvtPlayLongPress,
	EvtAmp,
	EvtVolFadeIn,
	EvtSource,

    EvtUpgradeRequest,
	EvtUpgradeFinish,
    EvtFactoryReset,
    EvtTest,

    EvtNumMax,
}event_id_type;

#define EVENT_EMPTY         (EvtMax)
#define EVENT_NUM           (EvtMax)

#endif // _CONFIG_H

