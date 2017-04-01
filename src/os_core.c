
#include "os_core.h"


static loop_type LoopQueue[LoopNumMax];

static void osLoopInit(void)
{
	uint8 i;

	for(i = 0; i < LOOP_ID_NUM; i++)
		LoopQueue[i].id = LOOP_ID_NONE;
}

/*
*
*/
uint8 OS_CreateLoop(loop_id_type id, loopHandler *handler)
{
    uint8 i;
    loop_type* pLoop;

    /* check input paramters */
    if ((id >= LoopNumMax) || (NULL == handler))
    {
        return ERR_PARAM;
    }

    /* check if this LoopID is already in your LoopQueue */
    for (i = 0; i < LoopNumMax; i++)
    {
        pLoop = LoopQueue + i;
        if (LOOP_ID_NONE == pLoop->id)
        {
            /* end of the loopQueue, put it*/
            pLoop->id = id;
            pLoop->handler = handler;
            return ERR_OK;
        }
        else
        {
            if (id == pLoop->id)
            {
                /* the loop you want to create is created before*/
                return ERR_ESIXT;
            }
        }
    }

    /* Loop queue is already full */
    return ERR_FULL;
}

/*
*
*/
void OS_DestroyLoop(loop_id_type id)
{
 	uint8 i, next;
	bool is_found = FALSE;
	loop_type *pLoop;

    /* check the input parameters */
    if (id >= LoopNumMax)
    {
        return;
    }

    /* search the loop which you want to destory */
    for (i = 0; i < LoopNumMax; i++)
    {
        pLoop = LoopQueue + i;
        if (id == pLoop->id)
        {
            /* found it, jump out*/
            is_found = TRUE;
            break;
        }
    }

    /* check if we found the loop */
    if (is_found)
    {
        for (next = i + 1; next < LoopNumMax; next++, i++)
        {       
            if (LOOP_ID_NONE == LoopQueue[next].id)
            {
                /*end of the loopQueue, jump out */
                LoopQueue[i].id = LOOP_ID_NONE;
                break;
            }
            else
            {
                /* move the next to previous */
                LoopQueue[i] = LoopQueue[next];
            }
        }
    }

    /* or this loop isn't in the loopQueue*/
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

/*
*
*/
static void OS_Loop(void)
{
    uint8 i;
    loop_type* pLoop;

    /* loop the loopQueue*/
    for (i = 0; i < LoopNumMax; i++)
    {
        pLoop = LoopQueue + i;
        if (LOOP_ID_NONE != pLoop->id)
        {
            /* execute this loop function */
            pLoop->handler();
        }
    }
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
static timer_type timerQueue[TimerNumMax];

static void osTimerInit(void)
{
    uint8 i;
    
    for (i = 0; i < TIMER_ID_NUM; i++)
        timerQueue[i].id = TIMER_ID_NONE;
}

/*
*
*/
uint8 OS_CreateTimer(timer_id_type id, uint8 interval, timerHandler handler)
{
    uint8 i;
    timer_type* pTimer;

    /* check the input parameters*/
    if ((id > TimerNumMax) || (NULL == handler))
    {
        return ERR_PARAM;
    }

    /* check if the timer is already in the timerQueue */
    for ( i = 0; i < TimerNumMax; i++)
    {
        pTimer = timerQueue + i;
        if (TIMER_ID_NONE == pTimer->id)
        {
            pTimer->id = id;
            pTimer->count = interval;   /* the count of timer will decrease */
            pTimer->interval = interval;
            pTimer->handler = handler;
            return ERR_OK;
        }
        else
        {
            if (id == pTimer->id)
            {
                /* this timer is already in the timerQueue,
                   but we don't return ERR_ESIXT, we update the timer
                */
                pTimer->count = interval; /* the count of timer will decrease */
                pTimer->interval = interval;
                pTimer->handler = handler;
            }
        }
    }

    /* timerQueue is full */
    return ERR_FULL;
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

void OS_DestoryTimer(timer_id_type id)
{
    uint8 i, next;
    timer_type *pTimer;
    bool is_found = FALSE;

    /* check the input parameters */
    if (id >= TimerNumMax)
    {
        return;
    }

    /* search the timer that you want to destroy*/
    for (i = 0; i < TimerNumMax; i++)
    {
        pTimer = timerQueue + i;
        if (id == pTimer->id)
        {
            /* found it*/
            is_found = TRUE;
            break;
        }
    }

    /* check if we have found the timer */
    if (is_found)
    {
        for (next = i + 1; next < TimerNumMax; next++, i++)
        {       
            if (TIMER_ID_NONE == timerQueue[next].id)
            {
                /*end of the timerQueue, jump out */
                timerQueue[i].id = TIMER_ID_NONE;
                break;
            }
            else
            {
                /* move the next to previous */
                timerQueue[i] = timerQueue[next];
            }
        }
    }

    /* this timer isn't in the timerQueue */
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

static void OS_LoopTimer(void)
{
    uint8 i;
    timer_type* pTimer;
    
    /* loop the timer functions */
    for (i = 0; i < TimerNumMax; i++)
    {
        pTimer = timerQueue + i;
        if (TIMER_ID_NONE == pTimer->id)
        {
            /* end of the timerQueue, do nothing */
        }
        else
        {
            if(pTimer->count > 0)
            {
                /* decrease the timer count */
                pTimer->count--;
            }
            else
            {
                /* timeout, handle the function, new loop*/
                pTimer->handler();
                pTimer->count = pTimer->interval;
            }
        }
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

// message Queue, this is a header of single link chain

static message_type msgQueue[MSG_NUM];
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

uint8 OS_SendMessage(event_type event, uint8 mdata, uint16 interval)
{
    uint8 i;
    message_type* pMsg;

    /* check the input parameters */
    if (event >= EvtNumMax)
    {
        return ERR_PARAM;
    }

    /* check if this message is already in the messageQueue */
    for (i = 0; i < MSG_NUM; i++)
    {
        pMsg = msgQueue + i;
        if (EVENT_EMPTY == pMsg->event)
        {
            /* end of this queue, put message here*/
            pMsg->event = event;
            pMsg->msg = mdata;
            pMsg->interval = interval;
            return ERR_OK;
        }
        else
        {
            if (event == pMsg->event)
            {
                /* this message is esixt, but we don't send ERR_ESIXT,
                   just overite it
                */
                pMsg->msg = mdata;
                pMsg->interval = interval;
                return ERR_OK;
            }
        }
    }

    /* msgQueue is full */
    return ERR_FULL;
}

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
void OS_CancelMessage(event_type event)
{
    uint8 i, next;
    bool is_found;
    message_type* pMsg;
    
    /* check the input parameters */
    if (event >= EvtNumMax)
    {
        return;
    }

    /* search this message that you want to cancel in message queue */
    for (i = 0; i < MSG_NUM; i++)
    {
        pMsg = msgQueue + i;
        if (event == pMsg->event)
        {
            /* found it*/
            is_found = TRUE;
            break;
        }
    }

    /* check if we have found the message */
    if (is_found)
    {
        for(next = i + 1; next < MSG_NUM; next++, i++)
        {
            if(EVENT_EMPTY == msgQueue[i].event)
            {
                msgQueue[i].event = EVENT_EMPTY;
                break;
            }
            else
            {
                msgQueue[i] = msgQueue[next];
            }
        }
    }

    /* this message isn't in the msgQueue */
}

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

static void OS_LoopMessage(void)
{
    
}

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



