
#include "config.h"
#include "private.h"
#include "message.h"

#define MSG_NUM		6
static message_type msgQueue[MSG_NUM];	// message Queue, this is a header of single link chain

//
void messageInit(void)
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
void messageAdd(uint8 event, uint8 mdata, uint16 interval)
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
void messageCancel(const uint8 event)
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

void messageLoop(void)
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

