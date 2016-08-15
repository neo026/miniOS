/*

*/

#include "config.h"
#include "private.h"
#include "message.h"

#include "./powerManage.h"
#include "./ports/key.h"
#include "./ports/aw9523.h"
#include "./ports/ad.h"

#define MAX_VOLUME 16

#define LIMBO_TIMEOUT   (5000)  // 5 seconds

void osStop(uint8 errno)
{
	debugMsg("osStop Error=", errno);
	//here you do some indications

	//while(1);
}



void osMemset(const void *src, uint8 value, uint8 len)
{
	uint8 *p = (uint8*)src;
	
	if((NULL == src) || (0 == len))	
		osStop(ERR_MEMSET);  
	
	while(len)
		p[len --] = value;
}

void auxLoop(void)
{
    static uint8 auxIn1Debounce = 0;
    
    // Aux audio plug in/out
    if(GPIO_AUXIN1)
    {
        auxIn1Debounce = 0;
        if(TRUE == gProject.auxIn1Connected)
            messageSend(EventAuxIn1Disconnected, 0, 0);
    }
    else
    {
        if(FALSE == gProject.auxIn1Connected)
        {
            if(auxIn1Debounce++ > 2)    // 2*100 = 200ms
                messageSend(EventAuxIn1Connected, 0, 0);
        }
    }
}

void userInit(void)
{
    // intial power manage.
    powerManageInit();
    timerCreate(TIMER_ID_POWER_MANAGER, 100, powerManageLoop);
    
    // initial key
    keyInit();
    timerCreate(TIMER_ID_KEY, 5, keyLoop);
    
    // initial led.
    ledFlashInit();
    timerCreate(TIMER_ID_BLINK_LED, 10, ledBlinkLoop);

    //
    timerCreate(TIMER_ID_AUX, 10, auxLoop);
    
    // if there is no action, the machine will enter limbo after 2 seconds
    messageSend(EventEnterLimbo, 0, LIMBO_TIMEOUT);

    // change state
    gProject.state = StateLimbo;
}

void eventEnterLimbo(void)
{
    gProject.state = StateLimbo;

    messageCancel(EventEnterLimbo);
    
    if(gProject.DcConnected || gProject.chargerOut1Connnected)    // DC is still in, charger out connected
    {
        debugMsg("out1=",gProject.chargerOut1Connnected);
    }
    else
    {
        debugStr("cut\n");
        //disable all the interrupt
        interruptDisable();
        
        disableWatchDog();
        //turn on the Battery voltage ADC switch, before get the value
        GPIO_BAT_EXTERNAL_VOLT_ON(OFF);
        GPIO_BAT_INTERNAL_VOLT_ON(OFF);

        // cut off peripheral power
        GPIO_SYS_POWER(OFF);
        
        //cut off the back out charger
        messageSend(EventChargerOut2OnOff, OFF, 0);

        // cut off battery power, these should be put at the last, because it is the system power
        GPIO_BAT_INTERNAL_ON(OFF);
        GPIO_BAT_EXTERNAL_ON(OFF);

        //enter sleep, even through the power is cut off by up-step, but somethings, the hardware will have something wrong,
        // the power will float, and then UI is wrong
        // but if add this, debug information will stop here
        enterSleep();
        debugStr("off\n");
    }

    
	/*Clear original data, Reset system data*/
	//initData();	
}

void eventPowerKey()
{
    if(StatePowerOn == gProject.state)
    {
        if((FALSE == gProject.DcOnOff) && (FALSE == gProject.DcOnOff2))
            messageSend(EventBatteryCheckStart, 0, 0);
    }
    else
    {
        //turn on indication
        messageSend(EventPowerOn, 0, 0);
    }	
}

void eventReady(uint8 on_off)
{
	//if(StateReadyOn == gProject.state)
	//	messageSend(EventPowerOn, 0, 0);
	//else
	//	messageSend(EventPowerOff, 0, 0);
}

void eventPowerOn(void)
{
	if(StatePowerOn == gProject.state)
        return ;

    // this is the peripheral power, e.g AW9523, it should be turn ON before the others operations.
    GPIO_SYS_POWER(ON);
    
    // inital led
	ledInit();

    // cancel message
    messageCancel(EventEnterLimbo);
    messageCancel(EventPowerOn);
    messageCancel(EventPowerOff);

    if(FALSE == gProject.battInitialising)
        messageSend(EventBatteryMonitor, 0, 0);
    
    //check the lock button    
    if(ON == gProject.lock)
    {
        //on the ON switch, the machine can not be open really

        // LED_LOCK blink 3 times as a kind of warning indication
        ledBlinkConfigure(LED_LOCK, 1, 1, 3);

        // enter limbo after timeout
        messageSend(EventEnterLimbo, 0, LIMBO_TIMEOUT);
    }
    else
    {
        // on the OFF switch, that is the really power on

        // turn on LED_LOCK
        messageSend(EventLock, 0, 0);

        // turn on LED_POWER.
        ledOnOff(LED_POWER, ON);

        // dim up all the LED
        ledDimConfigure(LED_GROUP1, DIM_MS(1500), 1, FALSE);

        //Open the back out charger
        messageSend(EventChargerOut2OnOff, ON, 0);
        messageSend(EventChargerOut1OnOff, ON, 0);
        
        
        //then send the power-on command to bluetooth, delay a moment
        messageSend(EventBtPowerOnOff, ON, 2000);

        // change state
        gProject.state = StatePowerOn;
    }

}

void eventPowerOff(void)
{
	if(StateLimbo == gProject.state)
        return;

    messageCancel(EventBatteryCheckStop);
    messageCancel(EventBtPowerOnOff);
    
    // turn off all LEDs
    ledAllOnOff(OFF);

    //charger is connected.
    if(gProject.DcConnected)
        messageSend(EventBatteryCheckStart, 0, 0);

    // enter limbo after timeout
    //if(batteryIsNormal())
        messageSend(EventEnterLimbo, 0, LIMBO_TIMEOUT);
    //else
    //    messageSend(EventEnterLimbo, 0, 0);

    //Close the Chrome Case power
    messageSend(EventChargerOut2OnOff, OFF, 0);
    messageSend(EventChargerOut1OnOff, OFF, 0);

    //close bluetooth.
    if(gProject.btState >= StateBtPowerOn)
    {
        sendKeyCmd(U_KEY_POWEROFF, KEY_TYPE_SHORT_UP);
    }
    
    gProject.btState = StateBtNone;
    
    // change state
    gProject.state = StateLimbo;
}

void eventBatteryCheckStart(void)
{
    uint8 level = 0;

    if(FALSE == gProject.battInitialising)
        return;

    if(gProject.DcOnOff || gProject.DcOnOff2)
    {
       if(gProject.DcOnOff)
            level = gProject.battLevel;
       else
            level = gProject.battLevel2;

       debugMsg("chg level=", level);
       
       // do nothing
       if(level <= BATT_LOW1)
       {
            ledBlinkConfigure(LED_BATT1, 10, 10, 0);
       }
       else if(level < BATT_NORMAL1)
       {
            ledOnOff(LED_BATT1, ON);    // in this function, it contains the turn off function,
            ledBlinkConfigure(LED_BATT2, 10, 10, 0);
            
       }
       else if(level < BATT_FULL)
       {
            ledOnOff(LED_BATT1, ON);
            ledOnOff(LED_BATT2, ON);
            ledBlinkConfigure(LED_BATT3, 10, 10, 0);
       }
       else
       {
            ledOnOff(LED_BATT1, ON);
            ledOnOff(LED_BATT2, ON);
            ledOnOff(LED_BATT3, ON);
            ledBlinkConfigure(LED_BATT4, 10, 10, 0);
       }
       
    }
    else
    {
         //in a case, power off state, long press power key, the battery will is intialized complete, it will enter here and display.
       if(StateLimbo == gProject.state)
            return ;
       
        if(BatteryInternal == gProject.battUsing)
            level = gProject.battLevel;
        else if(BatteryExternal == gProject.battUsing)
            level = gProject.battLevel2;
        else if(BatteryBoth == gProject.battUsing)
            level = gProject.battLevel;

        debugMsg("batt=", level);
    
        if(level <= BATT_LOW1)
        {
            ledOnOff(LED_BATT1, ON);
        }
        else if(level < BATT_NORMAL1)
        {
            ledOnOff(LED_BATT1, ON);
            ledOnOff(LED_BATT2, ON);
        }
        else if(level < BATT_FULL)
        {
            ledOnOff(LED_BATT1, ON);
            ledOnOff(LED_BATT2, ON);
            ledOnOff(LED_BATT3, ON);
        }
        else
        {
            ledOnOff(LED_BATT1, ON);
            ledOnOff(LED_BATT2, ON);
            ledOnOff(LED_BATT3, ON);
            ledOnOff(LED_BATT4, ON);
        }

        messageSend(EventBatteryCheckStop, 0, T_SEC(3));
    }
}

void eventBatteryCheckStop(void)
{
    if(OFF == gProject.DcConnected)
    {
        ledClear(LED_GROUP2);
        ledOnOff(LED_BATT1, OFF);
        ledOnOff(LED_BATT2, OFF);
        ledOnOff(LED_BATT3, OFF);
        ledOnOff(LED_BATT4, OFF);
    }
}


void eventBatteryLow(void)
{
	if(StatePowerOn ==  gProject.state)
    {
        if(OFF == gProject.DcConnected)
        {
            gProject.battLowCnt = 30;   //unit: 1s
            gProject.battLowFlag = TRUE;

            sendPacket(CMD_LOW_BATTERY, NULL, 0);
            
            ledBlinkConfigure(LED_BATT1, 2, 2, 12);
        }
    }   
}

void eventDcOnOff(uint8 on)
{
    if(on == gProject.DcOnOff)
        return ;

	if(on)
	{
		// external battery is normal and internal battery isn't charging, then we will charge the external battery
        if(temperature_normal == gProject.temperatureStatus)
		{
		    // stop the DC2 charging
            if(ON == gProject.DcOnOff2)
                messageSend(EventDcOnOff2, OFF, 0);
            
            // open charger, low active charging
			GPIO_DC_ON(OFF);

            // turn on indicator
			
			//
			messageSend(EventDcDetectStart, 0, T_SEC(1));
            gProject.DcOnOff = on;
		}
	}
	else
	{
		// close the charger, low active charging 
		GPIO_DC_ON(ON);

		// turn off indicator
		

		//
		messageCancel(EventDcDetectStart);
		gProject.DcStartDetect = FALSE;
        gProject.DcLedCnt = 0;
	    gProject.DcLedLowCnt = 0;
        gProject.DcOnOff = on;
	}
}

void eventDcOnOff2(uint8 on)
{
    if(on == gProject.DcOnOff2)
        return ;

    if(on)
    {
        // external battery is normal and internal battery isn't charging, then we will charge the external battery
        if((temperature_normal == gProject.temperatureStatus2) && (OFF == gProject.DcOnOff))
        {
            // open charger, low active charging
			GPIO_DC_ON2(OFF);
            
			// turn on indicator
			
			//
			messageSend(EventDcDetectStart, 0, T_SEC(1));

            //
            gProject.DcOnOff2 = on;
        }
    }
    else
    {
        // close the charger, low active charging 
        GPIO_DC_ON2(ON);

        // turn off indicator
        

        //
        messageCancel(EventDcDetectStart);
        gProject.DcStartDetect = FALSE;
        gProject.DcLedCnt = 0;
		gProject.DcLedLowCnt = 0;
        gProject.DcOnOff2 = on;

    }
}

void eventDcConnected(void)
{
	gProject.DcConnected = TRUE;

    // this is the peripheral power, e.g AW9523, it should be turn ON before the others operations.
    GPIO_SYS_POWER(ON);
    
    // inital led
	ledInit();

    if(FALSE == gProject.battInitialising)
        messageSend(EventBatteryMonitor, 0, 0);

    // charging internal battery firstly, and then charging external battery if it is exist,  
    // "gProject.temperatureStatus" is got from initData(), when the machine doesn't power on
    //if(temperature_normal == gProject.temperatureStatus)
	    messageSend(EventDcOnOff, ON, 60);
    //else if(temperature_normal == gProject.temperatureStatus2)
    //    messageSend(EventDcOnOff2, ON, 0);

    // open amplifier power
    GPIO_AMP_POWER(ON);
    
    // cancel
    messageCancel(EventEnterLimbo);

    messageCancel(EventBatteryLow);
    gProject.battLowFlag = FALSE;
    
    // cut off all the system power, supply power from DC, but not from the battery
    messageSend(EventChangeBattUsing, BatteryNone, 0);
}

void eventDcDisconnected(void)
{
	gProject.DcConnected = FALSE;

    if(ON == gProject.DcOnOff)
	    messageSend(EventDcOnOff, OFF, 0);
    else if(ON == gProject.DcOnOff2)
        messageSend(EventDcOnOff2, OFF, 0);

    // close amplifier power
    GPIO_AMP_POWER(OFF);

    if(StateLimbo == gProject.state)
    {
        // turn off all LEDs
        ledAllOnOff(OFF);

        // enter limbo after timeout
        messageSend(EventEnterLimbo, 0, LIMBO_TIMEOUT);
        
    }
    else
    {

    }
    
    ledOnOff(LED_BATT1, OFF);
    ledOnOff(LED_BATT2, OFF);
    ledOnOff(LED_BATT3, OFF);
    ledOnOff(LED_BATT4, OFF);
    gProject.DcStatus  = DcNotCharging;
    gProject.DcStatus2 = DcNotCharging;
}

void eventDcStatus(uint8 status)
{	
	uint8 lastStatus;

    if(ON == gProject.DcOnOff)
        lastStatus = gProject.DcStatus;
    else if(ON == gProject.DcOnOff2)
        lastStatus = gProject.DcStatus2;
    else
        return;

	debugMsg("lastDcStatus", lastStatus);
    
	if(lastStatus != status)
	{
	    ledOnOff(LED_BATT1, OFF);
        ledOnOff(LED_BATT2, OFF);
        ledOnOff(LED_BATT3, OFF);
        ledOnOff(LED_BATT4, OFF);
        
		if(DcChargingComplete == status)
		{
			if(ON == gProject.DcOnOff)
            {
                gProject.battLevel = BATT_FULL;
                
			    //turn off the charger for internal battery, and open the external DC charging switch 
			    messageSend(EventDcOnOff, OFF, 0);
                
                if((temperature_normal == gProject.temperatureStatus2) && (DcChargingComplete != gProject.DcStatus2))
                    messageSend(EventDcOnOff2, ON, 0);
            }
            else if(ON == gProject.DcOnOff2)
            {
                gProject.battLevel2 = BATT_FULL;
                messageSend(EventDcOnOff2, OFF, 0);
            }

            // it will clear the LED_BATT4 blink, and then turn on LED_BATT4
            ledOnOff(LED_GROUP2, ON);
		}
		else if(DcError == status)
		{
            if(ON == gProject.DcOnOff)
            {
			    //turn off the charger for internal battery, and open the external DC charging switch 
			    messageSend(EventDcOnOff, OFF, 0);

                if(temperature_normal == gProject.temperatureStatus2)
                    messageSend(EventDcOnOff2, ON, 0);
            }
            else if(ON == gProject.DcOnOff2)
            {
                messageSend(EventDcOnOff2, OFF, 0);
            }
        
			//start to blink error indicator
            ledBlinkConfigure(LED_GROUP2, 2, 2, 0);
		}
        else if(DcCharging == status)
        {
            messageSend(EventBatteryCheckStart, 0, 0);
        }
            
        if(ON == gProject.DcOnOff)
            gProject.DcStatus = status;
        else if(ON == gProject.DcOnOff2)
            gProject.DcStatus2 = status;
	}
}

void eventChangeBattUsing(uint8 batt)
{
    //if(batt == gProject.battUsing)
    //    return;

    if(BatteryInternal == batt)
    {
        GPIO_BAT_INTERNAL_ON(ON);   // ON must be first
        GPIO_BAT_EXTERNAL_ON(OFF);  // OFF must be second, otherwise we will not hold the system power
    }
    else if(BatteryExternal == batt)
    {
		GPIO_BAT_EXTERNAL_ON(ON);   // ON must be first
        GPIO_BAT_INTERNAL_ON(OFF);  // OFF must be second, otherwise we will not hold the system power
    }
    else if(BatteryBoth == batt)
    {
        GPIO_BAT_INTERNAL_ON(ON);
        GPIO_BAT_EXTERNAL_ON(ON);
    }
    else if(BatteryNone == batt)
    {
        GPIO_BAT_INTERNAL_ON(OFF);
        GPIO_BAT_EXTERNAL_ON(OFF);
    }

    gProject.battUsing = batt;
}

void eventChargerOut1Connected(void)
{
    return;

    // this is the peripheral power, e.g AW9523, it should be turn ON before the others operations.
    GPIO_SYS_POWER(ON);
    
    // inital led
	ledInit();

    if(FALSE == gProject.battInitialising)
        messageSend(EventBatteryMonitor, 0, 0);
    
    if(GPIO_CHARGER_SW_OUT1)
    {
        gProject.chargerOut1Connnected = TRUE;
        
        // for testing, enable charger out
        messageSend(EventChargerOut1OnOff, ON, 0);
    }
}

void eventChargerOut1Disconnected(void)
{
    gProject.chargerOut1Connnected = FALSE;
    
    // for testing, disable charger out
    messageSend(EventChargerOut1OnOff, OFF, 0);

    // enter limbo after timeout
    messageSend(EventEnterLimbo, 0, LIMBO_TIMEOUT);
}

void eventChargerOut1OnOff(uint8 on_off)
{
    if(ON == on_off)
    {
        GPIO_CHARGER_OUT1_ON(ON);
    }
    else
    {
        GPIO_CHARGER_OUT1_ON(OFF);
    }

    gProject.chargerOut1OnOff = on_off;
}

void eventChargerOut2Connected(void)
{
    gProject.chargerOut2Connnected = TRUE;
    
    messageSend(EventChargerOut2OnOff, ON, 0);
}

void eventChargerOut2Disconnected(void)
{
    gProject.chargerOut2Connnected = FALSE;

    messageSend(EventChargerOut2OnOff, OFF, 0);
}

void eventChargerOut2OnOff(uint8 on_off)
{
    if(ON == on_off)
    {
        GPIO_CHARGER_OUT2_ON(ON);
    }
    else
    {
        GPIO_CHARGER_OUT2_ON(OFF);
    }

    gProject.chargerOut2OnOff = on_off;
}

void eventAuxIn1Connected(void)
{
    gProject.auxIn1Connected = TRUE;
}

void eventAuxIn1Disconnected(void)
{
    gProject.auxIn1Connected = FALSE;
}

void eventAuxIn2Connected(void)
{
    gProject.auxIn2Connected = TRUE;
}

void eventAuxIn2Disconnected(void)
{
    gProject.auxIn2Connected = FALSE;
}

void eventCapsense(uint8 OnOff)
{
   
    if(OnOff != gProject.capsense)
    {
        if(OnOff == ON)
        {            
            //
            messageCancel(EventEnterLimbo);

            //
            messageSend(EventDim, ON, 0);
        }
        else
        {
            if(StateLimbo == gProject.state)
                messageSend(EventEnterLimbo, 0, LIMBO_TIMEOUT);

            //
            messageSend(EventDim, OFF, 0);
        }

        gProject.capsense = OnOff;
    }
}

void eventLock(void)
{
    uint8 lock;
    
    if((gProject.state != StatePowerOn) && (gProject.DcConnected == FALSE))
		return ;
    
    lock = GPIO_LOCK ? OFF : ON;

    debugMsg("->", lock);
    //if(lock != gProject.lock)
    {
        if(ON == lock)
        {
            ledOnOff(LED_UNLOCK, OFF);
            ledOnOff(LED_LOCK, ON);
            ledOnOff(LED_GROUP1, OFF);
            ledOnOff(LED_BT, OFF);
        }
        else
        {
            ledOnOff(LED_UNLOCK, ON);
            ledOnOff(LED_LOCK, OFF);
            ledDimConfigure(LED_GROUP1, DIM_MS(1500), 1, FALSE);
            messageSend(EventBtState, 0, 0);
        }
        
        gProject.lock = lock;
    }
}

void eventIndications(uint8 error)
{
    if(ERR_BATT_NTC == error)
    {
        // start indicating
    }
}

void eventDim(uint8 up_down)
{
    //if(StatePowerOn != gProject.state)
    //    return ;
        
    if(gProject.lock)
        return;
        
    if(up_down)
    {
        ledDimConfigure(LED_GROUP1, DIM_MS(1500), 0, TRUE);
    }
    else
    {
        ledDimDown(LED_GROUP1);
    }
}

void eventVolSub(void)
{
	if(gProject.state != StatePowerOn)
		return ;
	
    #if 0
	if(gProject.volume > 0)
	{
		gProject.volume --;
	}
	else
	{
		// min volume, close amp
		if(ON == gProject.ampOnOff)
			messageSend(EventAmp, OFF, 0);
	}
    #else
	
    
    sendKeyCmd(U_KEY_VOLUME_DOWN, KEY_TYPE_DOWN);
    
    #endif

}

void eventVolAdd(void)
{
	if(gProject.state != StatePowerOn)
		return ;
	
    #if 0
	if(0 == gProject.volume)
	{
		// open amp
		messageSend(EventAmp, ON, 0);
	}

	if(gProject.volume < MAX_VOLUME)
	{
		gProject.volume ++;
	}
	else
	{
		// max volume
	}
    #else
		
    sendKeyCmd(U_KEY_VOLUME_UP, KEY_TYPE_DOWN);

    #endif
}

void eventForward(void)
{
	if(gProject.state != StatePowerOn)
		return ;

    if((gProject.source == SOURCE_AUX1) || (gProject.source == SOURCE_AUX2))
        return;
    
    sendKeyCmd(U_KEY_FORWARD, KEY_TYPE_SHORT_UP);
}

void eventBackward(void)
{
	if(gProject.state != StatePowerOn)
		return ;

    if((gProject.source == SOURCE_AUX1) || (gProject.source == SOURCE_AUX2))
        return;
	
    sendKeyCmd(U_KEY_BACKWARD, KEY_TYPE_SHORT_UP);
}

void eventBtKey(void)
{
	if(gProject.state != StatePowerOn)
		return ;
	
	sendKeyCmd(U_KEY_MODE, KEY_TYPE_LONG);
}

void eventBtPowerOnOff(uint8 onOff)
{
    if(onOff)
    {
        sendKeyCmd(U_KEY_POWERON, KEY_TYPE_SHORT_UP);
        gProject.source = SOURCE_BT;
        gProject.btState = StateBtPowerOn;
        sendSourceCmd(gProject.source);
    }
    else
    {
        sendKeyCmd(U_KEY_POWEROFF, KEY_TYPE_SHORT_UP);
    }
}

void eventBtState(void)
{
    uint8 state =  gProject.btState;

    debugMsg("btstate=", state);
    if(SOURCE_BT == gProject.source)
    {
        if(StateBtNone == state)
        {
            ledOnOff(LED_BT, OFF);
        }
        else if(StateBtPairing == state)
        {
            ledBlinkConfigure(LED_BT, 2, 2, LIMITLESS);
        }
        else if((StateBtConnected == state) || (StateBtPlay == state) || (StateBtOutgoingCall == state) || (StateBtAnswer == state))
        {
            ledOnOff(LED_BT, ON);
        }
        else if(StateBtIncomingCall == state)
        {
            ledBlinkConfigure(LED_BT, 5, 5, LIMITLESS);
        }
    }
    else
    {
        ledClear(LED_BT);
    }
        
}

void eventBtBqbTest(void)
{
    if(gProject.state == StateLimbo)
        return;

    sendKeyCmd(U_KEY_BQB, KEY_TYPE_SHORT_UP);
}

void eventBtEqTest(void)
{ 
    if(gProject.state == StateLimbo)
       return;
    
    sendPacket(CMD_EQ_TEST, NULL, 0);
}

void eventPlayKey(void)
{
	if(gProject.state != StatePowerOn)
		return ;
	
    sendKeyCmd(U_KEY_PLAY, KEY_TYPE_SHORT_UP);
}

void eventPlayLongPress(void)
{
    if(gProject.state == StateLimbo)
        return ;

    if(gProject.source == SOURCE_BT)
    {
        sendKeyCmd(U_KEY_SIRI_REJECT, KEY_TYPE_SHORT_UP);
    }
}

void eventSource(uint8 mode)
{   
    #if 0
    uint8 low, high;
    uint16 value;
    
    value = getAdcValue(batteryVoltInternal);
    high = (uint8)(value >> 8);
    low = (uint8)value;

    debugMsg("I-h=",high);
    debugMsg("I-l=",low);

    value = getAdcValue(batteryVoltExternal);
    high = (uint8)(value >> 8);
    low = (uint8)value;

    debugMsg("E-h=",high);
    debugMsg("E-l=",low);
    #endif
    
	if(gProject.state != StatePowerOn)
		return ;
    
    gProject.source ++;
    
    if(gProject.source == SOURCE_TOP)
        gProject.source = SOURCE_BT;
       
    sendSourceCmd(gProject.source);
}

void eventAmp(uint8 OnOff)
{
	if((ON == OnOff) || (OFF == OnOff))
	{
		if(OnOff != gProject.ampOnOff)
		{
			gProject.ampOnOff = OnOff;
			
			messageCancel(EventVolFadeIn);
			
			if(OnOff)
			{
				// open channel

				//start volume fade in function
				messageSend(EventVolFadeIn, 0, 0);
			}
			else
			{
				// close channel
			}
		}
	}
}

void eventVolFadeIn(uint8 vol)
{
	if(ON == gProject.ampOnOff)
	{
		if(vol < gProject.volume)
		{
			// set volume value

			// fade in next volume step
			messageSend(EventVolFadeIn, vol++, 100);
		}
		else
		{
			// reach the volume

			// set volume value
		}

		// set volume
	}
}

void eventUpgradeRequest(void)
{
    //send request commmand to bluetooth,
    GPIO_UPGRADE(ON);
    GPIO_WAKEUP(ON);
    sendPacket(CMD_UPGRADE, NULL, 0);
    messageSend(EventUpgradeFinish, 0, T_SEC(2));
}

void eventUpgradeFinish(void)
{
    //send reset restart cmd
    GPIO_UPGRADE(OFF);
    GPIO_WAKEUP(OFF);
}

void eventFactoryReset(void)
{

}

void eventTest(void)
{

}

void messageHandler(const message_type *msg)
{
	if(NULL == msg)
		return;
	
	//debugMsg("Handle message:", msg->event);
	
	switch(msg->event)
	{
		case EventLoopBack:
			{
			//static uint8 loop_count = 0;
			//P3.3 ^= 1;
			//debugMsg("EventLoopBack", loop_count++);
			//messageSend(EventLoopBack,0, T_SEC(5));
			}
			break;
			
		case EventEnterLimbo:
            // this debug information may be not printed when DC is not in, because it will be cut off power directly, but the debug need a loop;
			debugStr("eventEnterLimbo\n");
			eventEnterLimbo();
			break;
			
		case EventPowerKey:
            debugStr("EventPowerKey\n");
			eventPowerKey();
			break;

		case EventReady:
            debugMsg("EventReady=", msg->msg);
			eventReady(msg->msg);
			break;
			
		case EventPowerOn:
			debugStr("EventPowerOn\n");
            //GPIO_WAKEUP(ON);
            //GPIO_UPGRADE(ON);
			eventPowerOn();
			break;
			
		case EventPowerOff:
            debugStr("EventPowerOff\n");
			eventPowerOff();
			break;

		case EventBatteryMonitor:
			debugStr("EventBatteryMonitor\n");
			batteryMonitor();
			break;

        case EventBatteryCheckStart:
            debugStr("EventBatteryCheckStart\n");
            eventBatteryCheckStart();
            break;

        case EventBatteryCheckStop:
            debugStr("EventBatteryCheckStop\n");
            eventBatteryCheckStop();
            break;

		case EventBatteryLow:
			debugStr("EventBatteryLow\n");
			eventBatteryLow();
			break;

		case EventDcConnected:
			debugStr("EventDcConnected\n");
			eventDcConnected();
			break;

		case EventDcDisconnected:
			debugStr("EventDcDisconnected\n");
			eventDcDisconnected();
			break;

		case EventDcOnOff:
			debugMsg("EventDcOnOff=", msg->msg);
			eventDcOnOff(msg->msg);
			break;

        case EventDcOnOff2:
            debugMsg("EventDcOnOff2=", msg->msg);
			eventDcOnOff2(msg->msg);
            break;

		case EventDcStatus:
			debugMsg("EventDcStatus", msg->msg);
			eventDcStatus(msg->msg);
			break;

		case EventDcDetectStart:
			debugStr("EventDcDetectStart\n");
			gProject.DcStartDetect = TRUE;
			break;
			
        case EventChangeBattUsing:
            debugMsg("EventChangeBattUsing",msg->msg);
            eventChangeBattUsing(msg->msg);
            break;

        case EventChargerOut1Connected:
            debugStr("EventChargerOut1Connected\n");
            eventChargerOut1Connected();
            break;

        case EventChargerOut1Disconnected:
            debugStr("EventChargerOut1Disconnected\n");
            eventChargerOut1Disconnected();
            break;

        case EventChargerOut1OnOff:
            debugMsg("EventChargerOut1OnOff",msg->msg);
            eventChargerOut1OnOff(msg->msg);
            break;

        case EventChargerOut2Connected:
            debugStr("EventChargerOut2Connected\n");
            eventChargerOut2Connected();
            break;

        case EventChargerOut2Disconnected:
            debugStr("EventChargerOut2Disconnected\n");
            eventChargerOut2Disconnected();
            break;

        case EventChargerOut2OnOff:
            debugMsg("EventChargerOut2OnOff",msg->msg);
            eventChargerOut2OnOff(msg->msg);
            break;

        case EventAuxIn1Connected:
            debugStr("EventAuxIn1Connected\n");
            eventAuxIn1Connected();
            break;

        case EventAuxIn1Disconnected:
            debugStr("EventAuxIn1Disconnected\n");
            eventAuxIn1Disconnected();
            break;

        case EventAuxIn2Connected:
            debugStr("EventAuxIn2Connected\n");
            eventAuxIn2Connected();
            break;

        case EventAuxIn2Disconnected:
            debugStr("EventAuxIn2Disconnected\n");
            eventAuxIn2Disconnected();
            break;

        case EventCapsense:
            debugMsg("EventCapsense", msg->msg);
            eventCapsense(msg->msg);
            break;

        case EventLock:
			debugStr("EventLock\n");
            eventLock();
            break;

        case EventIndications:
            debugMsg("EventErrorIndications",msg->msg);
            eventIndications(msg->msg);
            break;

        case EventDim:
            //debugMsg("EventDim",msg->msg);
            eventDim(msg->msg);
            break;

		case EventVolSub:
			debugStr("EventVolSub\n");
			eventVolSub();
			break;

		case EventVolAdd:
			debugStr("EventVolAdd\n");
			eventVolAdd();
			//ledFadeStop();
			break;

        case EventForward:
			debugStr("EventForward\n");
            eventForward();
            break;

        case EventBackward:
			debugStr("EventBackward\n");
            eventBackward();
            break;
            
		case EventBtKey:
			debugStr("EventBtKey\n");
			eventBtKey();
			break;

        case EventBtPowerOnOff:
            debugMsg("EventBtPowerOnOff", msg->msg);
			eventBtPowerOnOff(msg->msg);
            break;

        case EventBtState:
            debugStr("EventBtState\n");
            eventBtState();
            break;

        case EventBtBqbTest:
            debugStr("EventBtBqbTest\n");
            eventBtBqbTest();
            break;

        case EventBtEqTest:
            debugStr("EventBtEqTest\n");
            eventBtEqTest();
            break;

        case EventPairing:
            //
            break;

		case EventPlayKey:
			debugStr("EventPlayKey\n");
			eventPlayKey();
			break;

        case EventPlayLongPress:
            debugStr("EventPlayLongPress\n");
            eventPlayLongPress();
            break;

		case EventAmp:
			//debugMsg("EventAmp:", msg->msg);
			eventAmp(msg->msg);
			break;

		case EventVolFadeIn:
			//debugMsg("EventVolFadeIn:", msg->msg);
			//eventVolFadeIn(msg->msg);
			break;

        case EventSource:
			debugStr("EventSource\n");
            eventSource(msg->msg);
            break;

        case EventUpgradeRequest:
            debugStr("EventUpgradeRequest\n");
            eventUpgradeRequest();
            break;

        case EventUpgradeFinish:
            debugStr("EventUpgradeFinish\n");
            eventUpgradeFinish();
            break;

        case EventFactoryReset:
            debugStr("EventFactoryReset\n");
            eventFactoryReset();
            break;

        case EventTest:
            debugStr("EventTest\n");
            eventTest();
            break;


		default:
			debugMsg("Unknown event:", msg->event);
			break;
	}
}
