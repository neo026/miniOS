/*

*/

#ifndef TIMER_H
#define TIMER_H

/* Timer ID */
typedef enum
{
    TimerBlinkLed,
    TimerKey,
    TimerPowerManage,
    TimerAux,
 
    TimerMax,   // do not modify it, it should be at the last, it means the number of TIMER_ID, and the empty TIMER_ID.
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

void timerInit(void);
void timerCreate(uint8 timer_id, uint8 timeout, timerHandler handler);
void timerDelete(uint8 timer_id);
void timerLoop(void);


#endif  // TIMER_H

