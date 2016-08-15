/*

*/

#include "config.h"
#include "private.h"
#include "message.h"


void eventLoopBack(message_type *msg)
{
    messageSend(EvtLoopBack, 0, 100);
}

//
static const eventList_type eventList[] = 
{
    {EvtLoopBack,     StateLimbo,   eventLoopBack},
};

#define EVENT_LIST_NUM      (sizeof(eventList)/sizeof(eventList[0]))
static uint8 eventIndex[EVENT_LIST_NUM];

//
void osEventIndexInit(void)
{
    uint8 i;
    
    for(i = 0; i < EVENT_LIST_NUM; i++)
    {
        eventIndex[i] = eventList[i].event;
    }
}


void osMessageHandler(message_type *msg)
{
    uint8 i;
    uint8 state;
    uint8 state_mask;
    eventList_type temp;
    uint8 event = msg->event;
    
    for(i = 0; i < EVENT_LIST_NUM; i++)
    {
        // find the index of this event.
        if(eventIndex[i] == event)
        {
            // we found the event position in the eventList.
            temp = eventList[i];

            // start to handle
            state = osGetState();     // get current state
            state_mask = temp.state_mask; // expect state

            #if 0// Debug
            DBG_STR_DATA("event=", temp.event);
            DBG_STR_DATA("arg  =", temp);
            DBG_STR_DATA("state=", state);
            DBG_STR_DATA("state_mask=", state_mask);
            #endif
            
            if(state & state_mask)
            {
                if(NULL != temp.handler)
                {
                    (temp.handler)(msg);
                }
                else
                {
                    //warning
                }
            }
            else
            {
                // state error
            }

            break;
        }
    }

}

void osMemset(const void *src, uint8 value, uint8 len)
{
	uint8 *p = (uint8*)src;
	
	if((NULL == src) || (0 == len))	
		osStop(ERR_MEMSET);
	
	while(len)
		p[len --] = value;
}

void osStop(uint8 errno)
{
	DBG_STR_DATA("osStop Error=", errno);
	//here you do some indications

	//while(1);
}

void osWarning(uint8 errno)
{
    debugMsg("osWarning:", errno);
    //
}


// timer tick is 1ms, so this input "count" range is 0 ~ 255ms
void osDelay1ms(uint8 count)
{
    gProject.timerDelay =  count;
    
    osWatchDogClear();
    
    while(gProject.timerDelay);
}

void osSetState(osState_type state)
{

}

osState_type osGetState(void)
{

    return StatePowerOn;
}





