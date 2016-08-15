
#ifndef _KEY_H
#define _KEY_H

enum
{
	noPressFlag,
	shortPressFlag,
	longPressFlag,
	vLongPressFlag,
	vvLongPressFlag
};

typedef struct
{
	uint8 keyValueLast;	/*Save Last Press Key value*/
	uint8 keyStatus;
	uint8 keyHoldTimeCnt;
	uint8 keyRepeatTimeCnt;
}key_type;

// need add 50 adc value
#define KEY1	0
#define KEY2	290
#define KEY3	562
#define KEY4	794

#define KEY1_1	0
#define KEY1_2	290
#define KEY1_3	562

#define KEY0	1023
#define KEYT	50

#define KEY_ERR		0xff		// detect error or sample error, something multi keys press will cause it
#define KEY_NONE	0x00		// key release
#define KEY_POWER	(1 << 0)
#define KEY_VOLSUB	(1 << 1)
#define KEY_VOLADD	(1 << 2)
#define KEY_BT		(1 << 3)
#define KEY_PLAY	(1 << 4)
#define KEY_FORWARD (1 << 5)
#define KEY_BACKWARD (1 << 6)
#define KEY_SOURCE  (1 << 7)

// combine key
#define KEY_UPGRADE         (KEY_BT | KEY_SOURCE)
#define KEY_FACTORY_RESET   (KEY_BT | KEY_VOLADD)
#define KEY_TEST            (KEY_BT | KEY_VOLSUB)
#define KEY_BQB_TEST        (KEY_BT | KEY_FORWARD)
#define KEY_EQ_TEST         (KEY_VOLSUB | KEY_BACKWARD)

#define KEY_LONG_TIME	(20)	//uint: 50ms
#define KEY_VLONG_TIME	(40)
#define KEY_VVLONG_TIME	(50)
#define	KEY_REPEAT_TIME	(12)

void keyInit(void);
void keyLoop(void);
#endif