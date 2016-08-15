/*
*******************************************************************************
**
**  This device driver was created by Applilet for the 78K0S/KB1+, 78K0S/KA1+,
**  78K0S/KY1+,78K0S/KU1+ 8-Bit Single-Chip Microcontrollers.
**
**  Copyright(C) NEC Electronics Corporation 2002-2006
**  All rights reserved by NEC Electronics Corporation.
**
**  This program should be used on your own responsibility.
**  NEC Electronics Corporation assumes no responsibility for any losses
**  incurred by customers or third parties arising from the use of this file.
**
**  Filename :	ad.h
**  Abstract :	This file implements device driver for AD module.
**  APIlib :	NEC78K0SKX1+.lib V1.70 [7 Dec. 2006]
**
**  Device :	uPD78F9221
**
**  Compiler :	NEC/CC78K0
**
*******************************************************************************
*/
#ifndef	_MDAD_
#define	_MDAD_
/*
*******************************************************************************
**  MacroDefine
*******************************************************************************
*/
typedef enum
{
	ADChannel2 = 0x02,
	ADChannel3 = 0x03,
	ADChannel4 = 0x04,
	ADChannel5 = 0x05,
	ADChannel6 = 0x06, 
	ADChannel7 = 0x07,
	
	batteryVoltInternal = ADChannel6,
	batteryVoltExternal = ADChannel5,
	batteryTempInternal = ADChannel7,
	batteryTempExternal = ADChannel4,
	
	ADC_KEY = ADChannel3,
	ADC_KEY1 = ADChannel2,
}AdcChannel;

void AD_Init( void );
uint16 getAdcValue(AdcChannel channel);

#endif
