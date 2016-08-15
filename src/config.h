
#ifndef _CONFIG_H
#define _CONFIG_H

/* data class/type defines */
typedef unsigned char       uint8;
typedef unsigned int        uint16;
#define NULL                ((void *)0)


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
enum
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

    EvtMax,
};

#define EVENT_EMPTY         (EvtMax)
#define EVENT_NUM           (EvtMax)

/* Timer ID */
enum
{
    TimerBlinkLed,
    TimerKey,
    TimerPowerManage,
    TimerAux,
 
    TimerMax,   // do not modify it, it should be at the last, it means the number of TIMER_ID, and the empty TIMER_ID.
};

#define TIMER_ID_NONE       (TimerMax)
#define TIMER_ID_NUM        (TimerMax)


/*----------------GPIOs Configuration------------------*/
//i2c interface
#define GPIO_SCK				(0)  //LED-SCL
#define GPIO_SDA				(0)  //LED-SDA
#define GPIO_SCK_MODE(x)		(0)
#define GPIO_SDA_MODE(x)		(0)
#define GPIO_AW9523_RST(x)		(0)  // LED-RSTN, Reset pin, Low level to reset, High to normal operation

//Keys
#define GPIO_BN_POWER			(0) // POWER-ON/OFF-KEY, interrupt pin

// System 3.3v power
#define GPIO_SYS_POWER(x)       (0) // POWER-ON/OFF
#define GPIO_DC_BATT(x)         (0) // DC/BATTERY-POWER-ON/OFF, unused
#define GPIO_AMP_POWER(x)		(0) //POWER-AMP-POWER-ON/OFF, 1 -> open amplifier power, 0 ->close amplifier power

//charger in
#define GPIO_DC			        (0) //DC-DETECTION, pull high when plugin DC, pull low when plug out DC

#define GPIO_DC_ON(x)	        (0) // INTERNAL-BATTERY-CHARGE-ON/OFF pin, Low active charging
#define GPIO_DC_LED		        (0)  //INTERNAL-BATTERY-CHARGING-STATUS-DETECTION
#define GPIO_DC_FULL            (0)  //INTERNAL-BATTERY-CHARGED-STATUS-DETECTION

#define GPIO_DC_ON2(x)          (0) // EXTERNAL-BATTERY-CHARGE-ON/OFF pin, Low active charging
#define GPIO_DC_LED2            (0)  //ETERNAL-BATTERY-CHARGING-STATUS-DETECTION
#define GPIO_DC_FULL2           (0)  //EXTERNAL-BATTERY-CHARGED-STATUS-DETECTION

//Battery Switch
#define GPIO_BAT_INTERNAL_ON(x)			(0)  //INTERNAL-BATTERY-POWER-ON/OFF
#define GPIO_BAT_INTERNAL_VOLT_ON(x)	(0)  //INTERNAL-BATTERY-VOLTAGE-DETECTION

#define GPIO_BAT_EXTERNAL_ON(x)			(0) //ETERNAL-BATTERY-POWER-ON/OFF
#define GPIO_BAT_EXTERNAL_VOLT_ON(x)	(0)  //EXTERNAL-BATTERY-VOLTAGE-DETECTION

// charge out
#define GPIO_CHARGER_SW_OUT1       (0/*P7.0*/)  //USB1-INSERT-DETECION
#define GPIO_CHARGER_SW_OUT2       (0)  //undefined
#define GPIO_CHARGER_OUT1_ON(x)    (0)   //USB1-CHARGE-ON/OFF
#define GPIO_CHARGER_OUT2_ON(x)    (0)   //USB2-CHARGE-ON/OFF
#define GPIO_USB_OVP               (0)   //USB_OVP

// audio in
#define GPIO_AUXIN1             (0)  // AUX1-PLUG-INSERT-DETECTION
#define GPIO_AUXIN2             (0)  //AUX2-PLUG-INSERT-DETECTION

//cypress and lock
#define GPIO_CAPSENSE           (0)  // CAPSENSE-CONTROL-SIGNAL
#define GPIO_LOCK               (0)  //LOCK-KEY

//upgrade and wakeup BT
#define GPIO_WAKEUP(x)          (0)  //RESERVED-1
#define GPIO_UPGRADE(x)         (0)  //RESERVED-2

#endif // _CONFIG_H

