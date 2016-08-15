
#include "config.h"
#include "private.h"
#include "message.h"

#include "./ports/bsp.h"
#include "timer.h"

//
application_type gProject;

void osInit(void)
{
    //Initial global variable
    osMemset(&gProject, 0, sizeof(application_type));

    // debug system initial.
    osDebugInit();

    // initial event Index
    osEventIndexInit();

    //initial message loop
    messageInit();

    // initial timers
    timerInit();
}

void osRunning(void)
{
    while(1)
    {
        // events loop
        messageLoop();

        // debug loop
        osDebugLoop();
        
        // reserved loop
        osReservedLoop1();
        osReservedLoop2();

        // timers loop
        if(gProject.tick >= TIMER_10MS)
        {
            gProject.tick = 0;
            timerLoop();

            //clear watch dog
            osWatchDogClear();
        }
    }
}

//edit on the 'development' branch
void main(void)
{
    /* hardware initialization. */
    bspInit();

    /* disable watch dog first. */
    osWatchDogDisable();
    
	/* os initialization */
    osInit();
    
    /* start initial the user's data */
    //userInit();

    /* enable watch dog */
    osWatchDogEnable();
    
    /* start to run all the loops*/
    osRunning();

    /* never come here */
}
