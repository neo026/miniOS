
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
    uint8 event_tmp;

	// check input event is valid or not
	if(event >= EvtMax)
	{
		DBG_STR_DATA("add invalid event:", event);
		return;
	}

    // check if this message already in the message queque.
    for(i = 0; i < MSG_NUM; i++)
    {
        event_tmp = msgQueque[i].event;
        if(EVENT_EMPTY != event_tmp)
        {
            if(event_tmp == event)
            {
                msgQueque[i].msg = mdata;
                msgQueque[i].interval = interval;
                return;
            }
            else
            {
               //do nothing
            }
        }
        else
        {
            // already reach the end of the this message.
            // put this event into message queque.
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
void messageCancel(uint8 event)
{
	uint8 i, index_end;	
    uint8 event_tmp;
    
	// check the message is valid or not
	if(event >= EvtMax)
	{
		DBG_STR_DATA("cancel invalid event:", event);
		return;
	}

    for(index_end = 0, i = 0; i < MSG_NUM; i++)
    {
        if(EVENT_EMPTY == msgQueque[i].event)
            break;
        else
            index_end++;
    }
    
    for(i = 0; i < MSG_NUM; i++)
    {
        event_tmp = msgQueque[i].event;
        if(EVENT_EMPTY != event_tmp)
        {
            if(event_tmp == event)  // find the message which we want to cancel.
            {
                for(; i < index_end; i++)
                {
                    msgQueque[i] = msgQueque[i + 1];
                    break;
                }

                msgQueque[index_end].event = EVENT_EMPTY;
                return;
            }
        }
        else
        {
            // already reach the end of message queque.
            break;
        }        
    }

	//Run Here, this message you want to cancel is not the message queque. do nothing
}

/*
	Find the timeout message or the conndition = TRUE message, this function is put into the main() while(1).
	if find out, apply its handler, otherwise find next timeout or condition message.
*/

void messageLoop(void)
{
	uint8 i, index_end;
    uint8 event_tmp;
	uint8 needApply = FALSE;

    if(gProject.timerMsgTick < MSG_TICK_MS)
        return;

    gProject.timerMsgTick = 0;

    for(i = 0; i < MSG_NUM; i++)
    {
        if(msgQueque[i].interval > 0)
            msgQueque[i].interval--;

        if(0 == msgQueque[i].interval)
           needApply = TRUE;
    }

    if(FALSE == needApply)
        return;

    for(index_end = 0, i = 0; i < MSG_NUM; i++)
    {
        if(EVENT_EMPTY == msgQueque[i].event)
            break;
        else
            index_end++;
    }
    
    for(i = 0; i < MSG_NUM; i++)
    {
        event_tmp = msgQueque[i].event;
        
        if(EVENT_EMPTY != event_tmp)
        {
            if(0 == msgQueque[i].interval)
            {
               message_type msg;
               msg = msgQueque[i];
               //
               for(; i < index_end; i++)
               {
                   msgQueque[i] = msgQueque[i + 1];
                   break;
               }

               msgQueque[index_end].event = EVENT_EMPTY;

               osMessageHandler(&msg);
               return;
            }
        }
        else
        {
            // already reach the end of message queque.
            return;
        }
    }
}

