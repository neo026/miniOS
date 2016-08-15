/*

*/

#include "../config.h"
#include "bsp.h"


void bspInit(void)
{
    ;
}

void bspInterruptSwitch(uint8 enable)
{
    if(TRUE == enable)
    {
        // enable all interrupts
        
    }
    else
    {
        // disable all interrupts
    }
    
}

void bspEnterSleep(void)
{

}

void bspWatchDogSwitch(uint8 enable)
{
    if(TRUE == enable)
    {
        // enable watch dog
        
    }
    else
    {
        // disable watch dog
    }
}


void bspWatchDogClear(void)
{

}

