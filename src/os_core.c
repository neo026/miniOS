
#include "os_core.h"


static loop_type LoopQueue[LOOP_ID_NUM];

static void osLoopInit(void)
{
	uint8 i;

	for(i = 0; i < LOOP_ID_NUM; i++)
		LoopQueue[i].id = LOOP_ID_NONE;
}

void osLoopCreate(loop_id_type id, loopHandler *handler)
{
	uint8 i;
	
	// check input parameters
	if(id >= LoopMax)
		return;

	if(NULL == handler)
		return;

	// check if this loopId is already in your Queue
	for(i = 0; i < LOOP_ID_NUM; i++)
	{
		if(LOOP_ID_NONE != LoopQueue[i].id)
		{
			if(id == LoopQueue[i].id)
			{
				//already in the loop Queue.
				LoopQueue[i].handler = handler;
				return;
			}
		}
		else
		{
			break;
		}
	}

	// if it does not in the loop Queue
	for(; i < LOOP_ID_NUM; i++)
	{
		if(LOOP_ID_NONE == LoopQueue[i].id)
		{
			LoopQueue[i].id = id;
			LoopQueue[i].handler = handler;
			return;
		}
	}

	// if it runs here, that means LoopQueue is FULL.
	
}

void osLoopDelete(loop_id_type id)
{
	uint8 i, next;
	uint8 found = FALSE;
	
	if(id >= LoopMax)
		return;

	for(i = 0; i < LOOP_ID_NUM; i++)
	{
		if(id == LoopQueue[i].id)
		{
			found = TRUE;
			break;
		}
	}

	if(found)
	{
		for(next = i + 1; next < LOOP_ID_NUM; next++, i++)
		{
			if(LOOP_ID_NONE != LoopQueue[next].id)
				LoopQueue[i] = LoopQueue[next];
			else
				break;
		}

		LoopQueue[i].id = LOOP_ID_NONE;
	}

	// if it runs here, it means that we didn't find the response LOOP ID.
}

static void osLoop(void)
{
	uint8 i;

	for(i = 0; i < LOOP_ID_NUM; i++)
	{
		if(LOOP_ID_NONE != LoopQueue[i].id)
		{
			(LoopQueue[i].handler)();
		}
	}
	
}

//
static timer_type timerQueue[TIMER_ID_NUM];

static void osTimerInit(void)
{
    uint8 i;
    
    for(i = 0; i < TIMER_ID_NUM; i++)
        timerQueue[i].id = TIMER_ID_NONE;
}

/*
    timeout: 10ms unit.
*/
void osTimerCreate(timer_id_type id, uint8 interval, timerHandler handler)
{
    uint8 i;

    // 1, check the input parameters.
    if(id >= TIMER_ID_NUM)
    {
        DBG_STR_DATA("timerCreate: id is fault:", id);
        return ;
    }

    if(NULL == handler)
    {
        DBG_STR_DATA("timerCreate: handler is null:", id);
        return;
    }

    // 2,check if this timer ID is already in the timer Queue
    for(i = 0; i < TIMER_ID_NUM; i++)
    {
    	if(TIMER_ID_NONE == timerQueue[i].id)
    	{
	        if(id == timerQueue[i].id)
	        {
	            DBG_STR_DATA("timerCreate: already is created:", id);
	            timerQueue[i].count = 0;
	            timerQueue[i].interval = interval;
	            timerQueue[i].handler = handler;
	            return ;
	        }
        }
        else
        {
			break;
        }
    }

    // 3, find the empty position and fill in it    
    for(; i < TIMER_ID_NUM; i++)
    {
        if(TIMER_ID_NONE == timerQueue[i].id)
        {
            timerQueue[i].id = id;
            timerQueue[i].count = 0;
            timerQueue[i].interval = interval;
            timerQueue[i].handler = handler;
            return ;
        }
    }
    
    DBG_STR_DATA("timerCreate: Queue is full:", id);
}

void osTimerDelete(const timer_id_type id)
{
    uint8 i, next;
    uint8 found = FALSE;

    // 1, check the input parameters
    if(id >= TimerMax)
    {
        DBG_STR_DATA("timerDelete: id is fault:" id);
        return;
    }

    // 2, find the specific position
    for(i = 0; i < TIMER_ID_NUM; i++)
    {
        if(id == timerQueue[i].id)
        {
            found = TRUE;
            break;
        }
    }

    // 3, move the element
    if(found)   // we found the right position
    {
        for(next = i + 1; next < TIMER_ID_NUM; next++, i++)
        {
            if(TIMER_ID_NONE != timerQueue[next].id)
                timerQueue[i] = timerQueue[next];
            else
                break;
        }

        timerQueue[i].id = TIMER_ID_NONE;
    }
    else
    {
        // not found timer id in the timer Queue.
    }

}

static void osTimerLoop(void)
{
    uint8 i;

    // find the valid timer id and then run its handle function
    for(i = 0; i < TIMER_ID_NUM; i++)
    {
        if(TIMER_ID_NONE != timerQueue[i].id)
        {
            if(timerQueue[i].count > 0)
            {
                timerQueue[i].count--; // doesn't reach yet
            }
            else
            {
                timerQueue[i].count = timerQueue[i].interval;
                (timerQueue[i].handler)();
            }
        }
        else
        {
            // we reached the last valid timer's loop. jump out here
            break;
        }
    }
}

static message_type msgQueue[MSG_NUM];	// message Queue, this is a header of single link chain
//
static void osMessageInit(void)
{
    uint8 i;

    for(i = 0; i < MSG_NUM; i++)
        msgQueue[i].event = EVENT_EMPTY;
}

/*
	brief: add a new message to the msgQueue arrary,
		if your input message is already put into the msgQueue, instead of it
		if you find an empty array position, that is the right position, fill it
			
	delay time = delay * 1ms
*/
void osMessageSend(event_id_type event, uint8 mdata, uint16 interval)
{
	uint8 i;

	// 1, check input event is valid or not
	if(event >= EvtMax)
	{
		DBG_STR_DATA("add invalid event:", event);
		return;
	}

    // 2, check if this message is already in the message Queue.
    for(i = 0; i < MSG_NUM; i++)
    {
    	if(EVENT_EMPTY != msgQueue[i].event)
    	{
			if(event == msgQueue[i].event)
			{
				msgQueue[i].msg = mdata;
				msgQueue[i].interval = interval;
				return;
			}

    	}
    	else
    	{
        	break;
        }
    }

    // 3, add this new event into the message Queue.
    for(; i < MSG_NUM; i++)
    {
        if(EVENT_EMPTY == msgQueue[i].event)
        {
            msgQueue[i].event = event;
            msgQueue[i].msg = mdata;
            msgQueue[i].interval = interval;
            return;
        }
    }
    
    //message Queue is full
	osWarning(ERR_MSG_FULL);
}
/*
*	This will Clear all the same event in the message Queue.
*/
void osMessageCancel(const event_id_type event)
{
	uint8 i, next;	
    uint8 found = FALSE;
  
	// 1, check the inpput parameters
	if(event >= EvtMax)
	{
		DBG_STR_DATA("cancel invalid event:", event);
		return;
	}

    // 2, find the specific position 
    for(i = 0; i < MSG_NUM; i++)
    {
        if(event == msgQueue[i].event)
        {
            found = TRUE;
            break;
        }
    }

    // 3, realign the message queue
    if(found)
    {
        for(next = i + 1; next < MSG_NUM; next++, i++)
        {
            if(EVENT_EMPTY != msgQueue[i].event)
            {
                msgQueue[i] = msgQueue[next];
            }
            else
            {
                break;
            }
        }

        msgQueue[i].event = EVENT_EMPTY;
    }
    else
    {
	    //Run Here, this message you want to cancel is not the message Queue. do nothing
    }
}

/*
	Find the timeout message or the conndition = TRUE message, this function is put into the main() while(1).
	if find out, apply its handler, otherwise find next timeout or condition message.
*/

static void osMessageLoop(void)
{
	uint8 i, next;
    uint8 found = FALSE;
	uint8 needApply = FALSE;

    // 1, if the message tick get ready, update the all the interval
    if(gProject.timerMsgTick >= MSG_TICK_MS)
    {
        gProject.timerMsgTick = 0;

        for(i = 0; i < MSG_NUM; i++)
        {
            if(EVENT_EMPTY == msgQueue[i].event)
                break;

            if(msgQueue[i].interval > 0)
                msgQueue[i].interval --;
        }
    }

    // 2, find which message is timeout
    for(i = 0; i < MSG_NUM; i++)
    {
        if(EVENT_EMPTY != msgQueue[i].event)
            break;

        if(0 == msgQueue[i].interval)
        {
            found = TRUE;
            break;
        }
    }

    // 3, we found one message is ready,
    if(found)
    {
        message_type msg = msgQueue[i];

        for(next = i + 1; next < MSG_NUM; next++, i++)
        {
            if(EVENT_EMPTY != msgQueue[i].event)
                msgQueue[i] = msgQueue[next];
            else
                break;
        }

        msgQueue[i].event = EVENT_EMPTY;

        osMessageHandler(&msg);
    }
}

void osInit(void)
{
    //
    osLoopInit();

    //
    osTimerInit();

	//
	osMessageInit();

    // initial event Index
    osEventIndexInit();
}

void osRunning(void)
{
    while(1)
    {
        osLoop();

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



