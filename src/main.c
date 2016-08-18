
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

typedef void (*whileLoopHandler)(void);

typedef struct
{
	loop_id_type id;
	whileLoopHandler handler;
}while_loop_type;

typedef enum
{
	LoopMessage,
	LoopDebugRx,
	/*add new loop ID here*/
	
	LoopMax,
}loop_id_type;

#define LOOP_ID_NONE	(LoopMax)
#define LOOP_ID_NUM		(LoopMax)

while_loop_type LoopQueque[LOOP_ID_NUM];

void loopInit(void)
{
	uint8 i;

	for(i = 0; i < LOOP_ID_NUM; i++)
		LoopQueque[i].id = LOOP_ID_NONE;
}

void loopAdd(loop_id_type id, whileLoopHandler *handler)
{
	uint8 i;
	
	// check input parameters
	if(id >= LoopMax)
		return;

	if(NULL == handler)
		return;

	// check if this loopId is already in your queque
	for(i = 0; i < LOOP_ID_NUM; i++)
	{
		if(LOOP_ID_NONE != LoopQueque[i].id)
		{
			if(id == LoopQueque[i].id)
			{
				//already in the loop queque.
				LoopQueque[i].handler = handler;
				return;
			}
		}
		else
		{
			break;
		}
	}

	// if it does not in the loop queque
	for(; i < LOOP_ID_NUM; i++)
	{
		if(LOOP_ID_NONE == LoopQueque[i].id)
		{
			LoopQueque[i].id = id;
			LoopQueque[i].handler = handler;
			return;
		}
	}

	// if it runs here, that means LoopQueque is FULL.
	
}

void loopDelete(loop_id_type id)
{
	uint8 i, next;
	uint8 found = FALSE;
	
	if(id >= LoopMax)
		return;

	for(i = 0; i < LOOP_ID_NUM; i++)
	{
		if(id == LoopQueque[i].id)
		{
			found = TRUE;
			break;
		}
	}

	if(found)
	{
		for(next = i + 1; next < LOOP_ID_NUM; next++, i++)
		{
			if(LOOP_ID_NONE != LoopQueque[next].id)
				LoopQueque[i] = LoopQueque[next];
			else
				break;
		}

		LoopQueque[i].id = LOOP_ID_NONE;
	}

	// if it runs here, it means that we didn't find the response LOOP ID.
}

void loopLoop(void)
{
	uint8 i;

	for(i = 0; i < LOOP_ID_NUM; i++)
	{
		if(LOOP_ID_NONE != LoopQueque[i].id)
		{
			(LoopQueque[i].handler)();
		}
	}
	
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
