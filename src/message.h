
#ifndef _MESSAGE_H
#define _MESSAGE_H

//message struct, it is very simple.
typedef struct _message_type
{
	uint8	event;					// message id or event
	uint8	msg;					// the message data
	uint16	interval;				// 1ms unit, 500us ~ 32.767s, unit 500us
}message_type;

typedef void (*eventHandler)(message_type *);

//event->state->handler
typedef struct
{
    uint8 event;
    uint8 state_mask;
    eventHandler handler;
}eventList_type;


//this defines is for the interval time of messageSend().
#define T_SEC(x)	(uint16)(x*1000)	// this input time range is 0~65535 ms

void messageInit(void);
void messageAdd(uint8 event, uint8 mdata, uint16 interval);
void messageCancel(uint8 event);
void messageLoop(void);

#define messageSend(event, mdata, interval)					messageAdd(event, mdata, interval)
#define messageSendConditionally(event, mdata, condition)

#endif	// _MESSAGE_H

