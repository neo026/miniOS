

#ifndef _OS_CORE_H
#define _OS_CORE_H

//
typedef void (*loopHandler)(void);

typedef struct
{
	loop_id_type id;
	loopHandler handler;
}loop_type;

typedef enum
{
	LoopMessage,
	LoopDebugRx,
	/*add new loop ID here*/
	
	LoopNumMax,
}loop_id_type;

#define LOOP_ID_NONE	(LoopNumMax)
#define LOOP_ID_NUM		(LoopMax)

void osLoopInit(void);
void osLoopCreate(loop_id_type id, loopHandler *handler);
void osLoopDelete(loop_id_type id);


/* Timer ID */
typedef enum
{
    TimerBlinkLed,
    TimerKey,
    TimerPowerManage,
    TimerAux,
 
    TimerNumMax,   // do not modify it, it should be at the last, it means the number of TIMER_ID, and the empty TIMER_ID.
}timer_id_type;

#define TIMER_ID_NONE       (TimerMax)
#define TIMER_ID_NUM        (TimerMax)

//
typedef void (*timerHandler)(void);

//
typedef struct
{
    timer_id_type id; // timer id
    uint8 count;    // a counter, unit: TIMER_10MS
    uint8 interval; // the interval between current and next, unit: TIMER_10MS
    timerHandler handler;
}timer_type;

void osTimerCreate(uint8 timer_id, uint8 timeout, timerHandler handler);
void osTimerDelete(uint8 timer_id);


#define MSG_NUM		6

//message struct, it is very simple.
typedef struct _message_type
{
	event_id_type event;					// message id or event
	uint8	msg;					// the message data
	uint16	interval;				// 1ms unit, 500us ~ 32.767s, unit 500us
}message_type;

typedef void (*eventHandler)(message_type *);

//event->state->handler
typedef struct
{
    event_id_type event;
    uint8 state_mask;
    eventHandler handler;
}eventList_type;

//this defines is for the interval time of messageSend().
#define T_SEC(x)	(uint16)(x*1000)	// this input time range is 0~65535 ms

/* */
#define ERR_OK      0x00
#define ERR_PARAM   0x01
#define ERR_FULL    0x02
#define ERR_ESIXT   0x03

void osMessageSend(event_id_type event, uint8 mdata, uint16 interval);
void osMessageCancel(const event_id_type event);

//
void osInit(void);
void osRunning(void);

#endif // _OS_CORE_H

