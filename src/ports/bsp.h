/*


*/

#ifndef BSP_H
#define BSP_H

void bspInit(void);
void bspInterruptSwitch(uint8 enable);
void bspEnterSleep(void);
void bspWatchDogSwitch(uint8 enable);
void bspWatchDogClear(void);

#endif // BSP_H

