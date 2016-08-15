/*
*   Battery manager code
*   author: Neo Gan
*/

#include "../config.h"
#include "../private.h"
#include "../message.h"

#include "batteryManager.h"
#include "ad.h"

#define CHARGING_NORMAL_INTERVAL            (1800) // unit: 1sec         
#define CHARGING_HIGH_INTERVAL              (600)

static battery_type theBattery;
static battery_type theBattery2;

//battery manager debug
#define DBG_BAT_ENABLE 1

uint16 adcValueTbl[] =
{
    345,
    351,
    356,
    362,
    367,
    373,
    378,
    384,
    389,
    395,
    401,
    406,
    412,
    417,
    423,
    428,
    434,
    439,
    445,
    450,
    456,


};

void batteryLowLoop(void)
{
    if((TRUE == gProject.battLowFlag) && (gProject.state == StatePowerOn))
    {
        if(gProject.battLowCnt > 0)
            gProject.battLowCnt --;
        else
            messageSend(EventBatteryLow, 0, 0);
    }
}

/*
*   update the voltage level.
*/
static uint8 getAdcLevel(uint16 adcValue)
{
	uint8 level;

	for(level = BATT_L00; level < BATT_FULL; level ++)
	{
		if(adcValue < adcValueTbl[level])
			break;
	}

	return level;
}

uint8 getTemperatureStatus(uint8 channel)
{
   uint8 status;
   uint16 value = getAdcValue(channel);
   
   if(gProject.DcOnOff || gProject.DcOnOff2)
   {
       if(value > 1000)
            status = temperature_full;
       else if((value >= 336) && (value <= 754))
            status = temperature_normal;
       else
            status = temperature_abnormal;
    }
    else
    {
        if(value > 1000)
             status = temperature_full;
        else if((value >= 236) && (value <= 900))
             status = temperature_normal;
        else
             status = temperature_abnormal;
    }

    return status;
}

/*
*   Battery Initial
*/
uint8 powerManageInit(battery_type *battery, uint8 channel)
{
    uint16 tmpValue;
    uint8 i;
	uint8 level;

	#if DBG_BAT_ENABLE
		if(batteryVoltInternal == channel)
            debugStr("internal plug in\n");
        else if(batteryVoltExternal == channel)
            debugStr("external plug in\n");
	#endif
	
    // get battery voltage first
    for(tmpValue = 0, i = 0 ; i < MAX_AV_COUNT; i ++)
        tmpValue += getAdcValue(channel);

	// average
	tmpValue /= MAX_AV_COUNT;

	// get the level of voltage according to the table
	level = getAdcLevel(tmpValue);

    #if DBG_BAT_ENABLE
		debugMsg("initLevel=", level);
	#endif
    
	// save into the currrent voltage array
	for(i = 0; i < MAX_AV_COUNT; i ++)
		battery->currentVoltArray[i] = level;
	
    // set average arrary
    for(i = 0; i < MAX_AAV_COUNT; i ++)
       battery->averageVoltArrary[i] = level;
	
    // save into current voltage
    battery->currentAverVolt = level;

	//save into average average voltage
	battery->averageAverVolt = level;

	// keep the save
	battery->battChargingStep = level;

    // set the minimal value, set a middle value
    battery->minVolt = level;

    battery->chargingCount = 0;


    // set the power supply type, this is determined by hardware limit
    // gProject.powerSupplyType = fromCharging;

	// do some indication when battery low

    return level;
	
}


void batteryPowerOff(void)
{
    static uint8 already_off = FALSE;
    
    // handle the battery temperature status below, such as indications
    if(already_off == FALSE)
    {
        //do some warning indications
        messageSend(EventIndications, ERR_BATT_NTC, 0);
        
        // turn off DC in
        messageSend(EventDcOnOff, OFF, 0);
        messageSend(EventDcOnOff2, OFF, 0);

        // turn off charger out
        messageSend(EventChargerOut1OnOff, OFF, 0);
        messageSend(EventChargerOut2OnOff, OFF, 0);

        // power off
        messageSend(EventPowerOff, 0, T_SEC(1));

        already_off = TRUE;
    }    
}

/*
* get and handle battery temperature status. 
* NOTE, battery is not pluged in, but MCU can runing normal
*/
void batteryTemperature(void)
{
    uint8 internal, external;

    internal = getTemperatureStatus(batteryTempInternal);
    external = getTemperatureStatus(batteryTempExternal);

    #if DBG_BAT_ENABLEx
        debugMsg("int-temp=", gProject.temperatureStatus);
        debugMsg("int-temp1=", internal);
        debugMsg("ext-temp=", gProject.temperatureStatus2);
        debugMsg("ext-temp1=", external);
    #endif
    

    // make list, and list all the situation
    if((temperature_normal == internal) && (temperature_normal == external))
    {
        
        if(temperature_full == gProject.temperatureStatus)  // initial internal battery
        {
            gProject.battLevel  = batteryInit(&theBattery, batteryVoltInternal);

            // restart battery charging
            if(gProject.DcConnected)
                messageSend(EventDcOnOff, ON, 0);
        }
        else if(temperature_full == gProject.temperatureStatus2)    // intial external battery
        {
            gProject.battLevel2 = batteryInit(&theBattery2, batteryVoltExternal);
            // restart battery charging
            if(gProject.DcConnected)
                messageSend(EventDcOnOff2, ON, 0);
        }
        else
        {
            // check the battery level, get the power from the higer battery, and then using both
            if(gProject.DcConnected)
            {
                // do nothing
            }
            else
            {
                if(gProject.battLevel == gProject.battLevel2)
                {
                    if(BatteryBoth != gProject.battUsing)
                        messageSend(EventChangeBattUsing, BatteryBoth, 0);
                }
                else if(gProject.battLevel > gProject.battLevel2)
                {
                    if(BatteryInternal != gProject.battUsing)
                        messageSend(EventChangeBattUsing, BatteryInternal, 0);
                }
                else
                {
                    if(BatteryExternal != gProject.battUsing)
                        messageSend(EventChangeBattUsing, BatteryExternal, 0);
                }
            }
        }
    }
    else if((temperature_normal == internal) && (temperature_abnormal == external))
    {
        //power off
        batteryPowerOff();
    }
    else if((temperature_normal == internal) && (temperature_full == external))
    {
        if(temperature_full == gProject.temperatureStatus)
        {
            //debugStr("internal plug in1\n");
            gProject.battLevel  = batteryInit(&theBattery, batteryVoltInternal);
            // restart battery charging
            if(gProject.DcConnected)
                messageSend(EventDcOnOff, ON, 0);
        }
            
        if(temperature_full != gProject.temperatureStatus2) // external battery plug out
        {
            debugStr("external plug out2\n");
            gProject.battLevel2 = 0;
            gProject.DcStatus2 = DcNotCharging;
            
            // remember that turn off current charger, and then turn on another battery charger if it can
            if(gProject.DcOnOff2)
                messageSend(EventDcOnOff2, OFF, 0);
        }
            
        // when DC is in, don't change, because we can't get power from battery
        if(BatteryInternal != gProject.battUsing)
        {
            if(gProject.DcConnected)
            {
                //Dc is in, we need to open the DC on/off
                if((OFF == gProject.DcOnOff) && (gProject.DcStatus < DcCharging))
                {
                    gProject.DcStatus2 = DcNotCharging;
                    messageSend(EventDcOnOff2, OFF, 0);
                    messageSend(EventDcOnOff, ON, 0);
                }
            }
            else
            {
                // should switch to internal battery in immediately, for holding the power
                GPIO_BAT_INTERNAL_ON(ON);
                messageSend(EventChangeBattUsing, BatteryInternal, 0);
            }
        }
    }
    else if((temperature_abnormal == internal) && (temperature_normal == external))
    {
        //power off
        batteryPowerOff();
    }
    else if((temperature_abnormal == internal) && (temperature_abnormal == external))
    {
        //power off
        batteryPowerOff();
    }
    else if((temperature_abnormal == internal) && (temperature_full == external))
    {
        //power off
        batteryPowerOff();
    }
    else if((temperature_full == internal) && (temperature_normal == external))
    {            
        if(temperature_full != gProject.temperatureStatus) // external battery plug out
        {
            debugStr("internal plug out3\n");
            gProject.battLevel = 0;
            gProject.DcStatus = DcNotCharging;
            
            // remember that turn off current charger, and then turn on another battery charger if it can
            if(gProject.DcOnOff)
                messageSend(EventDcOnOff, OFF, 0);
        }
        
        if(temperature_full == gProject.temperatureStatus2)
        {
            //debugStr("external plug in4\n");
            gProject.battLevel2 = batteryInit(&theBattery2, batteryVoltExternal);

            // restart battery charging
            if(gProject.DcConnected)
                messageSend(EventDcOnOff2, ON, 0);
        }
        
        // when DC is in, don't change, because we can't get power from battery
        if(BatteryExternal != gProject.battUsing)
        {
            if(gProject.DcConnected)
            {
                //Dc is in, we need to open the DC on/off
                if((OFF == gProject.DcOnOff2) && (gProject.DcStatus2 < DcCharging))
                {
                    gProject.DcStatus2 = DcNotCharging;
                    messageSend(EventDcOnOff, OFF, 0);
                    messageSend(EventDcOnOff2, ON, 0);
                }
            }
            else
            {
                // should switch to external battery in immediately, for holding the power
                GPIO_BAT_EXTERNAL_ON(ON);
                messageSend(EventChangeBattUsing, BatteryExternal, 0); 
            }
        }
    }
    else if((temperature_full == internal) && (temperature_abnormal == external))
    {
        //power off
        batteryPowerOff();
    }
    else if((temperature_full == internal) && (temperature_full == external))
    {
        if((temperature_full == gProject.temperatureStatus) && (temperature_full == gProject.temperatureStatus2))
            return;
        
        gProject.battLevel = 0;
        gProject.battLevel2 = 0;
        gProject.DcStatus = DcNotCharging;
        gProject.DcStatus2 = DcNotCharging;

        if(gProject.DcOnOff)
            messageSend(EventDcOnOff, OFF, 0);

        if(gProject.DcOnOff2)
            messageSend(EventDcOnOff2, OFF, 0);
        
        if(BatteryNone != gProject.battUsing)
            messageSend(EventChangeBattUsing, BatteryNone, 0);
    }

    // save status
    gProject.temperatureStatus = internal;
    gProject.temperatureStatus2 = external;
}

/*
*
*/
void detectAlgorithm(uint8 channel)
{
    uint8 level;
    uint8 i;
    battery_type *battery = (batteryVoltInternal == channel) ? &theBattery : &theBattery2;
    
    // 0, get the voltage level
    level = getAdcLevel(getAdcValue(channel));

    #if DBG_BAT_ENABLEx
		debugMsg("channel=", channel);
        debugMsg("level=", level);
    #endif

	// 1, remove the head(or first) value, add a new value at tail.
    for(i = 1; i < MAX_AV_COUNT; i ++)
        battery->currentVoltArray[i - 1] = battery->currentVoltArray[i];

    battery->currentVoltArray[i - 1] = level;

    // 2, calculate the current average of battery voltage
    for(level = 0, i = 0; i < MAX_AV_COUNT; i ++)
        level += battery->currentVoltArray[i];

    level /= MAX_AV_COUNT;

    // 3, update the minimal battery voltage in memory
    if(level < battery->minVolt)
        battery->minVolt = level;

	/*debug*/
	#if DBG_BAT_ENABLEx
		debugStr("BAT:\n");
		for(i = 0; i < MAX_AV_COUNT; i++)
			debugNum(battery->currentVoltArray[i]);
		
		debugMsg("MV:", battery->minVolt);
	#endif

    // 4, calulate the average voltage of battery, for Battery voltage display quickly
    battery->currentAverVolt = (battery->minVolt + level) / 2;

	#if DBG_BAT_ENABLEx
		debugMsg("CAV:", battery->currentAverVolt);
	#endif

    // 5, update average voltage arrary
    for(i = 1; i < MAX_AAV_COUNT; i ++)
        battery->averageVoltArrary[i - 1] = battery->averageVoltArrary[i];

    battery->averageVoltArrary[i - 1] = battery->currentAverVolt;

    // 6, update the Average Average voltage value, for Battery low display
    for(level = 0, i = 0; i < MAX_AAV_COUNT; i++)
        level += battery->averageVoltArrary[i];
        
    battery->averageAverVolt = level / MAX_AAV_COUNT;

	#if DBG_BAT_ENABLEx
		for(i = 0; i < MAX_AAV_COUNT; i++)
			debugNum(battery->averageVoltArrary[i]);
		
		debugMsg("AAV:", battery->averageAverVolt);
	#endif
}

uint8 batteryIsNormal(void)
{
	return (temperature_normal == gProject.temperatureStatus) || (temperature_normal == gProject.temperatureStatus2);
}

void batteryDcStatus(void)
{
	uint8 status;
	uint8 last_status;
    
	// charge status detection
	if(gProject.DcStartDetect && (gProject.DcOnOff || gProject.DcOnOff2))
	{
		gProject.DcLedCnt ++;

        if(ON == gProject.DcOnOff)
        {
            if(OFF == GPIO_DC_LED)
                gProject.DcLedLowCnt ++;

            last_status = gProject.DcStatus;
        }
        else if(ON == gProject.DcOnOff2)
        {
            if(OFF == GPIO_DC_LED2)
                gProject.DcLedLowCnt ++;

            last_status = gProject.DcStatus2;
        }
        
        //debugMsg("A", GPIO_DC_LED);
        //debugMsg("B", GPIO_DC_LED2);
        
		if(gProject.DcLedCnt >= 10)
		{
			if(gProject.DcLedLowCnt > 5)
				status = DcCharging;
			else if(gProject.DcLedLowCnt < 2)
				status = DcChargingComplete;
			else
				status = DcError;   // doesn't plug in battery

			if(status != last_status)
				messageSend(EventDcStatus, status, 0);

            //debugMsg("Low", gProject.DcLedLowCnt);
			gProject.DcLedCnt = 0;
			gProject.DcLedLowCnt = 0;
		}
		
	}

}


void checkChargingTime(battery_type *battery)
{
	// when charging, we need to increase the charging time(uint:1sec)
	if((DcCharging == gProject.DcStatus) || (DcCharging == gProject.DcStatus2))
	{
		battery->chargingCount ++;
	}
	else
	{
		// when charging stop, we need to decrease the charging time, do not clear immediately, for such a situation:
		// if charger plug out when do not reach the setting charging time,
		// and then charger plug in again, battery level will not  increase, but actually the battery voltage was up slowly
		if(battery->chargingCount > 0)
			battery->chargingCount --;
	}

}

void batteryProcess(uint8 channel)
{    
    uint8 setFlag = 0;
    uint8 level;
    
    battery_type *battery = (batteryVoltInternal == channel) ? &theBattery : &theBattery2;
    
    // calculate the step by the time, and update the battery step
    if(gProject.DcOnOff || gProject.DcOnOff2)
    {
    	// some machines have two ways to  charge battery.
       if(normal == gProject.DcSpeed)
        {
            if(battery->chargingCount > CHARGING_NORMAL_INTERVAL)
                setFlag = 1;
        }
        else
        {
            if(battery->chargingCount > CHARGING_HIGH_INTERVAL)
                setFlag = 1;
        }

		// charging interval timeout, change the battery step for indication
        if(setFlag)
        {
            battery->chargingCount = 0;
            if(battery->battChargingStep >= BATT_FULL)
                battery->battChargingStep = BATT_FULL;
			else
				battery->battChargingStep ++;
        }

		// change the battery level if the averageAverVolt is less, it would happen when playing music while charging.
		if(battery->averageAverVolt < battery->battChargingStep)
			level = battery->averageAverVolt;
		else
			level = battery->battChargingStep;

        // update LED display
        if(batteryVoltInternal == channel)
        {
            if(gProject.battLevel != level)
            {
                if(temperature_normal == gProject.temperatureStatus)
                    messageSend(EventBatteryCheckStart, 0, 0);

                gProject.battLevel = level;
            }
        }
        else
        {
            if(gProject.battLevel2 != level)
            {
                if(temperature_normal == gProject.temperatureStatus2)
                    messageSend(EventBatteryCheckStart, 0, 0);
                
                gProject.battLevel2 = level;
            }
        }
    }
    else
    {
        level = (batteryVoltInternal == channel) ? gProject.battLevel : gProject.battLevel2;
        
		// for battery low
		if(battery->averageAverVolt < level)
		{
			level = battery->averageAverVolt;
            
            if(batteryVoltInternal == channel)
                gProject.battLevel = level;
            else
                gProject.battLevel2 = level;
                
			battery->battChargingStep = battery->averageAverVolt;
			battery->chargingCount = 0;
		}
		else
		{
			//battery level have no change
			/* 	1, gProject.battLevel is the exactly voltage of the battery, or called battery level.

				2, when charging, battery level will be set , it choice the lower voltage between 
			   	theBattery.battChargingStep and theBattery.averageAverVolt.
			   	
				3, when charging stop, it will come here, you have to change to exactly battery level.
					that is averageAverVolt poperbly greater than battChargingStep, you need to keep the battChargingStep
			*/
		}        
    }

	#if DBG_BAT_ENABLE	
		//debugStr("chargingCount: ");
		//debugNum((uint8)(battery->chargingCount >> 8));
		//debugNum((uint8)battery->chargingCount);
	#endif
}

void batteryScenes(void)
{
    uint8 scenes;
    uint8 internal, external;
    
    /* Get scenes*/
    if(ON == gProject.DcOnOff)
    {
        if((ON == gProject.chargerOut1OnOff) || (ON == gProject.chargerOut2OnOff))
            scenes = SceneBothOfChargers;
        else
            scenes = SceneDcInOnly;
    }
    else
    {
        if((ON == gProject.chargerOut1OnOff) || (ON == gProject.chargerOut2OnOff))
            scenes = SceneChargerOutOnly;
        else
            scenes = SceneNoAnyChargers;
    }

    gProject.DcScenes = scenes;

    // get the NTC status
    internal = gProject.temperatureStatus;
    external = gProject.temperatureStatus2;    
}

/*
* Battery process
*/
void powerManageLoop(void)
{
    static uint8 battery1sec = 0;
    
    if(FALSE == gProject.battInitialising)
	    return;
    
    // always detect the battery voltage ADC value.
    if(temperature_full != gProject.temperatureStatus)
    {
        // update the  battery level
	    #if DBG_BAT_ENABLEx
            debugMsg("Proc-Int=", gProject.battLevel);
        #endif
        
        detectAlgorithm(batteryVoltInternal);
	     batteryProcess(batteryVoltInternal);
    }           

    // external battery
    if(temperature_full != gProject.temperatureStatus2)
    {    
        #if DBG_BAT_ENABLEx
            debugMsg("Proc-Ext=", gProject.battLevel2);
        #endif
        
        detectAlgorithm(batteryVoltExternal);
	     batteryProcess(batteryVoltExternal);
    }
    
    // detect the temperature of battery
    batteryTemperature();

    // check the battery low
    if((BatteryBoth == gProject.battUsing) || (BatteryInternal == gProject.battUsing))
    {  
       if(gProject.battLevel < BATT_LOW2)
       {
            if(gProject.state == StatePowerOn)
                messageSend(EventPowerOff, 0, 0);
       }
       else if(gProject.battLevel <= BATT_LOW1)
       {
            if((FALSE == gProject.battLowFlag) && (gProject.state == StatePowerOn))
                messageSend(EventBatteryLow, 0, 0);
       }
    }
    else if(BatteryExternal == gProject.battUsing)
    {
         if(gProject.battLevel < BATT_LOW2)
         {
             if(gProject.state == StatePowerOn)
                messageSend(EventPowerOff, 0, 0);
         }
         else if(gProject.battLevel2 <= BATT_LOW1)
         {
            if((FALSE == gProject.battLowFlag) && (gProject.state == StatePowerOn))
                messageSend(EventBatteryLow, 0, 0);
         }
    }

    if(battery1sec ++ >= 10) // unit 100ms
    {
        battery1sec = 0;
        // counting the battery charging time
        checkChargingTime(&theBattery);
        checkChargingTime(&theBattery2);
    }
}

void batteryMonitor(void)
{
    uint8 internal, external;
    
    if(FALSE == gProject.battInitialising)
    {
        gProject.temperatureStatus  = getTemperatureStatus(batteryTempInternal);
        gProject.temperatureStatus2 = getTemperatureStatus(batteryTempExternal);
        
        //initial these two battery
        if(temperature_full != gProject.temperatureStatus)
        {
            gProject.battLevel  = batteryInit(&theBattery, batteryVoltInternal);
        }
        
        if(temperature_full != gProject.temperatureStatus2)
        {
            gProject.battLevel2 = batteryInit(&theBattery2, batteryVoltExternal);
        }
        
        // do some indications
        messageSend(EventBatteryCheckStart, 0, 0);
	    gProject.DcSpeed = normal;
        gProject.battInitialising = TRUE;
    }
}

