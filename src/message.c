
#include "config.h"
#include "private.h"
#include "message.h"

#define MSG_NUM		6
static message_type msgQueque[MSG_NUM];	// message queque, this is a header of single link chain

//
void messageInit(void)
{
    uint8 i;

    for(i = 0; i < MSG_NUM; i++)
        msgQueque[i].event = EVENT_EMPTY;
}

/*
	brief: add a new message to the msgQueque arrary,
		if your input message is already put into the msgQueque, instead of it
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

    // 2, check if this message is already in the message queque.
    for(i = 0; i < MSG_NUM; i++)
    {
        if(event == msgQueque[i].event)
        {
            msgQueque[i].msg = mdata;
            msgQueque[i].interval = interval;
            return;
        }
    }

    // 3, add this new event into the message queque.
    for(; i < MSG_NUM; i++)
    {
        if(EVENT_EMPTY == msgQueque[i].event)
        {
            msgQueque[i].event = event;
            msgQueque[i].msg = mdata;
            msgQueque[i].interval = interval;
            return;
        }
    }
    
    //message queque is full
	osWarning(ERR_MSG_FULL);
}
/*
*	This will Clear all the same event in the message queque.
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
        if(event == msgQueque[i].event)
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
            if(EVENT_EMPTY != msgQueque[i].event)
            {
                msgQueque[i] = msgQueque[next];
            }
            else
            {
                break;
            }
        }

        msgQueque[i].event = EVENT_EMPTY;
    }
    else
    {
	    //Run Here, this message you want to cancel is not the message queque. do nothing
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
            if(EVENT_EMPTY == msgQueque[i].event)
                break;

            if(msgQueque[i].interval > 0)
                msgQueque[i].interval --;
        }
    }

    // 2, find which message is timeout
    for(i = 0; i < MSG_NUM; i++)
    {
        if(EVENT_EMPTY != msgQueque[i].event)
            break;

        if(0 == msgQueque[i].interval)
        {
            found = TRUE;
            break;
        }
    }

    // 3, we found one message is ready,
    if(found)
    {
        message_type msg = msgQueque[i];

        for(next = i + 1; next < MSG_NUM; next++, i++)
        {
            if(EVENT_EMPTY != msgQueque[i].event)
                msgQueque[i] = msgQueque[next];
            else
                break;
        }

        msgQueque[i].event = EVENT_EMPTY;

        osMessageHandler(&msg);
    }
}

