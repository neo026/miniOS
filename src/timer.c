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
void timerCreate(uint8 timer_id, uint8 timeout, timerHandler handler)
{
    uint8 i;

    if(timer_id >= TIMER_ID_NUM)
    {
        DBG_STR_DATA("timer id is fault:", timer_id);
        return ;
    }

    if(NULL == handler)
    {
        DBG_STR_DATA("timer handler is null:", timer_id);
        return;
    }

    for(i = 0; i < TIMER_ID_NUM; i++)
    {
        if(timer_id == timerQueque[i].id)
        {
            DBG_STR_DATA("timer already is created:", timer_id);
            return ;
        }
    }
    
    for(i = 0; i < TIMER_ID_NUM; i++)
    {
        if(TIMER_ID_NUM == timerQueque[i].id)
        {
            timerQueque[i].id = timer_id;
            timerQueque[i].time = 0;
            timerQueque[i].timeout = timeout;
            timerQueque[i].handler = handler;
            return ;
        }
    }
    
    DBG_STR_DATA("timer queque is full:", timer_id);
}

void timerDelete(uint8 timer_id)
{
    uint8 i, next;

    for(i = 0; i < TIMER_ID_NUM; i++)
    {
        if(timerQueque[i].id == timer_id)
        {
            for(next = i + 1; next < TIMER_ID_NUM; next++, i++)
            {
                timerQueque[i] = timerQueque[next];
            }

            timerQueque[next].id = TIMER_ID_NONE;
        }
    }
}

void timerLoop(void)
{
    uint8 i;

    for(i = 0; i < TIMER_ID_NUM; i++)
    {
        if(TIMER_ID_NONE != timerQueque[i].id)
        {
            if(timerQueque[i].time < timerQueque[i].timeout)
            {
                timerQueque[i].time++;
            }
            else
            {
                timerQueque[i].time = 0;
                (timerQueque[i].handler)();
            }
        }
    }
}

