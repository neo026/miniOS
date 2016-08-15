/*
	AW9523B Driver

	example:

*/

#include "../config.h"
#include "../private.h"

#include "i2c.h"
#include "aw9523.h"


#define LED_NUM_MAX			4		// the number of dim led
#define DIM_FIX_STEP		0x05

static led_type theLED[LED_NUM_MAX];

//uint8 ledRegAll[LED_ALL_NUM] = {};
uint8 ledRegGroup1[] = {LED_FORWARD, LED_BACKWARD, LED_PLAY, LED_VOL_DOWN, LED_VOL_UP, LED_SOURCE};
uint8 ledRegGroup2[] = {LED_BATT1, LED_BATT2, LED_BATT3, LED_BATT4};
//uint8 ledRegGroup3[LED_GROUP3_NUM] = {};

static uint8 aw9523_write(uint8 regAddr, uint8 writeData)
{
	uint8 ack1,ack2,ack3;
	uint8 i = 5;
	
	while(i --)
	{
		i2cStart();
		ack1 = i2cWriteByte(AW9523_WRITE_ADDR);
		ack2 = i2cWriteByte(regAddr);
		ack3 = i2cWriteByte(writeData);		
		i2cStop();
		
		if((ack1 == 0) && (ack2 == 0) && (ack3 == 0))
			return TRUE;
	}
	
	return FALSE;
}

static uint8 aw9523_read(uint8 regAddr, uint8 *readData)
{
	uint8 ack1, ack2, ack3;
	uint8 i = 5;

	while(i --)
	{
		i2cStart();
		ack1 = i2cWriteByte(AW9523_WRITE_ADDR);	
		ack2 = i2cWriteByte(regAddr);	
		i2cStop();	// maybe this is not neccessary

		i2cStart();
		ack3 = i2cWriteByte(AW9523_READ_ADDR);
		*readData = i2cReadByte(1);
		i2cStop();
		
		if((ack1 == 0) && (ack2 == 0) && (ack3 == 0))
			return TRUE;
	}
	
	return FALSE;
}

static void ledSet(led_type *pLed)
{
	uint8 i;

    if(LED_NONE == pLed->led)
		return;

    if(gProject.ledInitialising == FALSE)
        return;

    if(pLed->led < LED_GROUP1)
    {
        aw9523_write(pLed->led, pLed->step);
    }
    else if(pLed->led == LED_GROUP1)
	{
		for(i = 0; i < sizeof(ledRegGroup1); i ++)
		{
			//if(dim->maskLed == ledRegGroup1[i])
			//	continue;
			
			aw9523_write(ledRegGroup1[i], pLed->step);
		}
	}
	else if(pLed->led == LED_GROUP2)
	{
		for(i = 0; i < sizeof(ledRegGroup2); i ++)
		{
			//if(dim->maskLed == ledRegGroup1[i])
			//	continue;
			
			aw9523_write(ledRegGroup2[i], pLed->step);
		}
	}
}

uint8 ledClear(uint8 led)
{
	uint8 i;
	led_type *pLed;

    if(gProject.ledInitialising == FALSE)
        return;

	if(LED_NONE == led)
		return FALSE;
	
	for(i = 0; i < LED_NUM_MAX; i ++)
	{
		pLed = &theLED[i];
		if(pLed->led == led)
		{
		    if(led < LED_GROUP1)
            {
                aw9523_write(led, 0x00);
            }      
			else if(led == LED_GROUP1)
			{
				for(i = 0; i < sizeof(ledRegGroup1); i ++)
					aw9523_write(ledRegGroup1[i], 0x00);
			}
			else if(led == LED_GROUP2)
			{
				for(i = 0; i < sizeof(ledRegGroup2); i ++)
					aw9523_write(ledRegGroup2[i], 0x00);
			}

			pLed->led = LED_NONE;
			return TRUE;
		}
	}

    return FALSE;
}

// Initial LED driver and set the configuration
void ledInit(void)
{
	uint8 i;

    if(gProject.ledInitialising == TRUE)
        return;
    
	GPIO_AW9523_RST(ON);
    
	// I2C initial
	i2cInit();
    
	// Clear
	for(i = 0; i < LED_NUM_MAX; i ++)
		theLED[i].led = LED_NONE;
	
	// Hardware Reset
	GPIO_AW9523_RST(OFF);
	osDelay1ms(1);			// delay 20us at least
	GPIO_AW9523_RST(ON);
	osDelay1ms(10);			// delay
	
	// Get Information
	if((aw9523_read(REG_ID, &i) == FALSE) || (i != AW9523_ID))
		osStop(EER_I2C);
	
	// set configuration
	aw9523_write(REG_LED0_MODE, 0x00);		// P0 MODE, 1 -> GPIO mode, 0 -> LED mode
	aw9523_write(REG_LED1_MODE, 0x00);		// P1 MODE

	aw9523_write(REG_INT_PORT0, 0xFF);		// disable interrupt, 0 -> enable interrupt, 1 -> disable interrupt
	aw9523_write(REG_INT_PORT1, 0xFF);
	
	aw9523_write(REG_CTL, 0x02);			// 0 ~ 2/4 Imax

    gProject.ledInitialising = TRUE;
}

// turn on/off all the  leds
void ledAllOnOff(uint8 onOff)
{
	uint8 i;
	uint8 step = (onOff == ON) ? DIM_FIX_STEP : 0x00;

    if(gProject.ledInitialising == FALSE)
        return;
    
	for(i = 0; i < 16; i ++)
		aw9523_write(REG_DIM_BASE + i, step);

	for(i = 0; i < LED_NUM_MAX; i ++)
		theLED[i].led = LED_NONE;
}

// turn on/off one of leds
void ledOnOff(uint8 led, uint8 onOff)
{
    uint8 i;
    
	if(LED_NONE == led)
		return ;

    if(gProject.ledInitialising == FALSE)
        return;

	if(ledClear(led) == FALSE)
    {   
        if(led < LED_GROUP1)
        {
            aw9523_write(led, (ON == onOff) ? DIM_FIX_STEP : 0x00);
        }
        else if(led == LED_GROUP1)
    	{
    		for(i = 0; i < sizeof(ledRegGroup1); i ++)
    		{   			
    			aw9523_write(ledRegGroup1[i], (ON == onOff) ? DIM_FIX_STEP : 0x00);
    		}
    	}
    	else if(led == LED_GROUP2)
    	{
    		for(i = 0; i < sizeof(ledRegGroup2); i ++)
    		{   			
    			aw9523_write(ledRegGroup2[i], (ON == onOff) ? DIM_FIX_STEP : 0x00);
    		}
    	}
    }
}

uint8 ledGetDirection(uint8 led)
{
    uint8 i;
    led_type *pLed;

    if(LED_NONE == led)
		return OFF;

    for(i = 0; i < LED_NUM_MAX; i ++)
	{
		pLed = &theLED[i];
		if(pLed->led == led)
		    return pLed->direction;
    }

    return OFF;
}


// it can only mask one button
void ledSetMask(uint8 led)
{
	uint8 i;
    led_type *pLed;
    dim_type *pDim;

	
	for(i = 0; i < LED_NONE; i ++)
	{
		pLed = &theLED[i];
		//pDim->maskLed = led;
	}
}

void ledClearMask(uint8 led)
{
	uint8 i;
    
	led_type *pLed;
    dim_type *pDim;
	
	for(i = 0; i < LED_NONE; i ++)
	{
		pLed = &theLED[i];
		//pDim->maskLed |= ~led;
	}

}

// on_time and off_time unit is 100ms
void ledBlinkConfigure(uint8 led, uint8 on_time, uint8 off_time, uint8 count)
{
	uint8 i;
	led_type *pLed;
    blink_type *pBlink;

	// check if these arguments are valid
	if(LED_NONE == led)
		return ;

    if(gProject.ledInitialising == FALSE)
        return;

	if((0 == on_time) ||(0 == off_time))
		return;


	// close led first
	ledClear(led);
    
	// check if the led was already in the LED List
	for(i = 0; i < LED_NUM_MAX; i ++)
	{
		pLed = &theLED[i];
		if(pLed->led == led)
			goto newBlinkLed;
	}

	// this led is not in the LED List, we need to find a empty position
	for(i = 0; i < LED_NUM_MAX; i ++)
	{
		pLed = &theLED[i];
		if(pLed->led == LED_NONE)
			goto newBlinkLed;
	}
	
	// the LED List was already full
	osStop(EER_LED_FULL);
	
	// Yes, update the List
	newBlinkLed:
		pLed->led  = led;
		pLed->type = TYPE_BLINK;
        pLed->direction = ON;
        pLed->step = DIM_FIX_STEP;
		
        pBlink = &(pLed->content.blink);
		pBlink->onTime = on_time;
        pBlink->offTime = off_time;
        pBlink->count = count;
        pBlink->time = on_time;

        ledSet(pLed);
}

void ledBlinkLoop(void)
{
	uint8 i;
	led_type *pLed;
    blink_type *pBlink;

	for(i = 0; i < LED_NUM_MAX; i ++)
	{
		pLed = &theLED[i];
		if((LED_NONE != pLed->led) && (TYPE_BLINK == pLed->type))
		{
		    pBlink = &(pLed->content.blink);
			if(pBlink->time)
			{
				pBlink->time --;
			}
			else
			{
				pLed->direction ^= 1;
				pBlink->time = pLed->direction ? pBlink->onTime : pBlink->offTime;
                pLed->step   = pLed->direction ? DIM_FIX_STEP : 0x00;

                if((pBlink->count != LIMITLESS) && (pLed->direction == OFF))
				{
					if((-- pBlink->count) == 0)
					{
						// stop
						ledClear(pLed->led);
					}
				}
                
                ledSet(pLed);
			}
		}		
	}
}

void ledDimConfigure(uint8 led, uint8 step_time, uint8 count, uint8 action)
{
    uint8 i;
    led_type *pLed;
    dim_type *pDim;
    
    if((LED_NONE == led) || (step_time < DIM_STEP_MAX))
        return;

    // close led first
	ledClear(led);
    
	// check if the led was already in the LED List
	for(i = 0; i < LED_NUM_MAX; i ++)
	{
		pLed = &theLED[i];
		if(pLed->led == led)
			goto newDimLed;
	}

	// this led is not in the LED List, we need to find a empty position
	for(i = 0; i < LED_NUM_MAX; i ++)
	{
		pLed = &theLED[i];
		if(pLed->led == LED_NONE)
			goto newDimLed;
	}
	
	// the LED List was already full
	osStop(EER_LED_FULL);

    newDimLed:
        pLed->led = led;
        pLed->type = TYPE_DIM;
        pLed->step = 0;
        pLed->direction = ON;
        
        pDim = &(pLed->content.dim);
        pDim->stepKeepTime = step_time;
        pDim->time = 0;
        pDim->count = count;
        pDim->keyAction = action ? TRUE : FALSE;
        pDim->keyRelease = FALSE;
}

void ledDimLoop(void)
{
    uint8 i;
    led_type *pLed;
    dim_type *pDim;

    for(i = 0; i < LED_NUM_MAX; i ++)
    {
        pLed = &theLED[i];;
        if((LED_NONE != pLed->led) && (TYPE_DIM == pLed->type))
        {
            pDim = &(pLed->content.dim);
            
            if(pDim->time < pDim->stepKeepTime)
            {
                pDim->time ++;      // the keeping time of very step
            }
            else    // timeout
            {
                pDim->time = 0;
                if(ON == pLed->direction)
                {
                    if(pLed->step < DIM_STEP_MAX)
                    {               
                        pLed->step ++;
                    }
                    else
                    {               
                        if(pDim->keyAction)
                        {
                            // this part is for key press or release action,
                            if(pDim->keyRelease)
                                pLed->direction = OFF;
                            else
                                continue;
                        }
                        else
                        {
                            pLed->direction = OFF;
                        }
                    }
                }
                else
                {
                    if(pLed->step > 0)
                        pLed->step --;
                    else
                    {
                        if((pDim->count != LIMITLESS) && (pLed->direction == OFF))
        				{
        					if((-- pDim->count) == 0)
        					{
        						// stop
        						ledClear(pLed->led);
        					}
        				}
                    
                        if(pDim->keyAction)
                        {
                            // this part is for key press or release action
                            ledClear(pLed->led);
                        }
                    }
                }
                
                ledSet(pLed);                    
            }
        }       
    }

}

void ledDimDown(uint8 led)
{
    uint8 i;
    led_type *pLed;
    dim_type *pDim;

    for(i = 0; i < LED_NUM_MAX; i ++)
	{
		pLed = &theLED[i];
		if((pLed->type == TYPE_DIM) && (pLed->led == led))
        {
            pDim = &(pLed->content.dim);
            pDim->keyRelease = TRUE;
        }      
	}
}

