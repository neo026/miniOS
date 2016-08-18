/*

*/

#include "config.h"
#include "timer.h"

static timer_type timerQueque[TIMER_ID_NUM];

//
void timerInit(void)
{
    uint8 i;
    
    for(i = 0; i < TIMER_ID_NUM; i++)
        timerQueque[i].id = TIMER_ID_NONE;
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

    // 2,check if this timer ID is already in the timer queque
    for(i = 0; i < TIMER_ID_NUM; i++)
    {
        if(id == timerQueque[i].id)
        {
            DBG_STR_DATA("timerCreate: already is created:", id);
            return ;
        }
    }

    // 3, find the empty position and fill in it    
    for(i = 0; i < TIMER_ID_NUM; i++)
    {
        if(TIMER_ID_NONE == timerQueque[i].id)
        {
            timerQueque[i].id = id;
            timerQueque[i].count = 0;
            timerQueque[i].interval = interval;
            timerQueque[i].handler = handler;
            return ;
        }
    }
    
    DBG_STR_DATA("timerCreate: queque is full:", id);
}

void timerDelete(const timer_id_type id)
{
    uint8 i, next;
    uint8 found = FALSE;

    // 1, check the input parameters
    if(id >= TimerMax)
    {
        DBG_STR_DATA("timerDelete: id is fault:" id);
    }

    // 2, find the specific position
    for(i = 0; i < TIMER_ID_NUM; i++)
    {
        if(id == timerQueque[i].id)
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
            if(TIMER_ID_NONE != timerQueque[next].id)
                timerQueque[i] = timerQueque[next];
            else
                break;
        }

        timerQueque[i].id = TIMER_ID_NONE;
    }
    else
    {
        // not found timer id in the timer queque.
    }

}

void timerLoop(void)
{
    uint8 i;

    // find the valid timer id and then run its handle function
    for(i = 0; i < TIMER_ID_NUM; i++)
    {
        if(TIMER_ID_NONE != timerQueque[i].id)
        {
            if(timerQueque[i].count > 0)
            {
                timerQueque[i].count--; // doesn't reach yet
            }
            else
            {
                timerQueque[i].count = timerQueque[i].interval;
                (timerQueque[i].handler)();
            }
        }
        else
        {
            // we reached the last valid timer's loop. jump out here
            break;
        }
    }
}

