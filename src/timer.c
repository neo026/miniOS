/*

*/

#include "config.h"
#include "timer.h"

static timer_type timerQueue[TIMER_ID_NUM];

//
void timerInit(void)
{
    uint8 i;
    
    for(i = 0; i < TIMER_ID_NUM; i++)
        timerQueue[i].id = TIMER_ID_NONE;
}

/*
    timeout: 10ms unit.
*/
void timerCreate(timer_id_type id, uint8 interval, timerHandler handler)
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

void timerDelete(const timer_id_type id)
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

void timerLoop(void)
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

