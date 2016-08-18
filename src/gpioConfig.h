
#ifndef _GPIO_CONFIG_H
#define _GPIO_CONFIG_H

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

#endif // _GPIO_CONFIG_H

