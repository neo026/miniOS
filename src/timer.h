/*

*/

#ifndef TIMER_H
#define TIMER_H

//
typedef void (*timerHandler)(void);

//
typedef struct
{
    uint8 id;
    uint8 time;
    uint8 timeout;
    timerHandler handler;
}timer_type;

void timerInit(void);
void timerCreate(uint8 timer_id, uint8 timeout, timerHandler handler);
void timerDelete(uint8 timer_id);
void timerLoop(void);


#endif  // TIMER_H

