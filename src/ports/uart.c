/*
*******************************************************************************
**
**  This device driver was created by Applilet for the 78K0S/KB1+, 78K0S/KA1+
**  8-Bit Single-Chip Microcontrollers.
**
**  Copyright(C) NEC Electronics Corporation 2002-2006
**  All rights reserved by NEC Electronics Corporation.
**
**  This program should be used on your own responsibility.
**  NEC Electronics Corporation assumes no responsibility for any losses
**  incurred by customers or third parties arising from the use of this file.
**
**  Filename :	serial.c
**  Abstract :	This file implements device driver for serial module.
**  APIlib :	NEC78K0SKX1+.lib V1.70 [7 Dec. 2006]
**
**  Device :	uPD78F9221
**
**  Compiler :	NEC/CC78K0
**
*******************************************************************************
*/

/*
*******************************************************************************
**  Include files
*******************************************************************************
*/

#ifdef ENABLE_SERIAL

#include "../config.h"
#include "../private.h"
#include "../message.h"

#include "serial.h"

/* For finding packet byte*/
#define	FLAG_HEAD0		0
#define	FLAG_HEAD1		1
#define	FLAG_LENGTH		2
#define FLAG_PDU		3

#define MIN_PKT_LENGTH	3
static uint8 packet_header[] = {'A','T'};

serial_type theSerial;

static uint8 uart0_sending_flag =  FALSE;
/* -------------------------------------------------------------------*/
/*convert decimal number to hexadecimal*/
uint8 dec2Hex(uint8 input)
{
	if((input >= 0x0a) && (input <= 0x0f))
		return input - 0x0a + 'a';
	else if((input >= 0x00) && (input <= 0x09))
		return input + '0';
	else
		return 'z';		// input wrong number		
}

/* put the data into the Tx Buffer and update the write index */
void addTxBuffer(uint8 *tx_addr, uint8 tx_len)
{
	uint8 i;
    
    if((NULL != tx_addr) && (tx_len > 0))
    {
    	for(i = 0; i < tx_len; i ++)
    		theSerial.txBuffer[theSerial.txPutIndex ++] = tx_addr[i];

        //if((ASIF6 & 0x02) == 0)
        if(uart0_sending_flag == FALSE)
        {
            TXS0 = theSerial.txBuffer[theSerial.txGetIndex ++];
            uart0_sending_flag = TRUE;
        }
    }
}

#if 0
void printIndex(void)
{
	uint8 tmp;
	
	while((ASIF6 & 0x02) != 0);
	TXB6 = dec2Hex(theSerial.txPutIndex/16);
	while((ASIF6 & 0x02) != 0);
	TXB6 = dec2Hex(theSerial.txPutIndex%16);
	while((ASIF6 & 0x02) != 0);
	TXB6 = dec2Hex(theSerial.txGetIndex/16);
	while((ASIF6 & 0x02) != 0);
	TXB6 = dec2Hex(theSerial.txGetIndex%16);
	while((ASIF6 & 0x02) != 0);
	TXB6 = ':';
	for(tmp = theSerial.txGetIndex; tmp != theSerial.txPutIndex; tmp ++)
	{
		if(tmp > SERIAL_TX_BUFFER_LENGTH)tmp = 0;
		while((ASIF6 & 0x02) != 0);
		TXB6 = theSerial.txBuffer[tmp];
	}
}

void printRaw(uint8 byte)
{
	while((ASIF6 & 0x02) != 0);
	TXB6 = byte;
}

void printChar(uint8 byte)
{
	while((ASIF6 & 0x02) != 0);
	TXB6 = dec2Hex(byte/16);
	while((ASIF6 & 0x02) != 0);
	TXB6 = dec2Hex(byte%16);
}

void printString(const char *str)
{
	uint8 i;
	uint8 size = 0;
	char *p = str;
	
	while(*p++ != '\0')
		size ++;

	for(i = 0; i < size; i++)
	{
		while((ASIF6 & 0x02) != 0);
		TXB6 = str[i];
	}	
}
void gernateFrame(void)
{
#define G_SIZE 10

	uint8 frame[G_SIZE] = {'*','@','>',G_SIZE-7};
	uint8 i;
	static uint8 a = 12;
	uint8 checksum;
	
	frame[0] = packet_header[0];
	frame[1] = packet_header[1];
	frame[2] = G_SIZE - 2;
	frame[3] = '>';
	
	checksum = frame[2]+frame[3];
	//format: HEADER + LENGTH + CMD + DATA+ CHECKSUM
	for(i = 4; i < G_SIZE-3; i++)
	{
		frame[i] = a++;
		checksum += frame[i];
	}
	
	frame[i++] = ~checksum + 1;
	addTxBuffer(frame,G_SIZE);
}
#endif

void serialSendNum(uint8 num)
{
	uint8 tmp;
	
	tmp = dec2Hex(num/16);
	addTxBuffer(&tmp, 1);
	tmp = dec2Hex(num%16);
	addTxBuffer(&tmp, 1);
	tmp = ' ';
	addTxBuffer(&tmp, 1);
}

void serialSendString(const char *str)
{
	char *p = str;
	uint8 str_size = 0;
	// Don't forget this
	if(NULL == p)
		return;
	
	// send string
	while(*p++ != '\0') str_size ++;
	addTxBuffer((uint8 *)str, str_size);
}

void serialSendChar(uint8 c)
{
    addTxBuffer(&c, 1);
}

/* for printing the debug information */
void serialSendStringWithData(const char *str, uint8 argv)
{
	char *p = str;
	uint8 str_size = 0;
	uint8 tmp;
	
	// send string
	while(*p++ != '\0') str_size ++;
	addTxBuffer((uint8 *)str, str_size);

	
	// send argv
	tmp = ' ';
	addTxBuffer(&tmp, 1);

	tmp = dec2Hex(argv/16);
	addTxBuffer(&tmp, 1);
	tmp = dec2Hex(argv%16);
	addTxBuffer(&tmp, 1);
	// send tail
	tmp = '\n';
	addTxBuffer(&tmp, 1);
}

/* format: HEADER + LENGTH + CMD + DATA+ CHECKSUM
    data_size, is the size of send data.
*/
void sendPacket(uint8 cmd, uint8 *send_data, uint8 data_size)
{
	uint8 i;
	uint8 checksum;
	uint8 length;
	
	addTxBuffer(packet_header, sizeof(packet_header));
	length = data_size + MIN_PKT_LENGTH;
	addTxBuffer(&length, 1);
	addTxBuffer(&cmd, 1);
    
    if(data_size > 0)
	    addTxBuffer(send_data,data_size);

	checksum = cmd + length;
	for(i = 0; i < data_size; i++)
		checksum += send_data[i];

	checksum = ~checksum + 1;
	addTxBuffer(&checksum, 1);

    addTxBuffer((uint8 *)'\n', 1);
}

void sendKeyCmd(uint8 key, uint8 type)
{
    uint8 send_data[2];
    
    send_data[0] = key;
    send_data[1] = type;
    sendPacket(CMD_KEY, send_data, sizeof(send_data));
}

void sendSourceCmd(uint8 source)
{
    sendPacket(CMD_SOURCE, &source, 1);
}

void getBtStatus(void)
{
    static char timeout = 0;
    uint8 send_data[4];

    if(StateLimbo == gProject.state)
        return ;

    if(timeout++ > 3)
        timeout = 0;
    else
        return;
    
    //battery level
    send_data[0] = gProject.battLevel;
    send_data[1] = gProject.battLevel2;
    //DC status
    send_data[2] = gProject.DcStatus;
    send_data[3] = gProject.DcStatus2;
    //
    sendPacket(CMD_GET_STATUS, send_data, sizeof(send_data));
}

/* this is a valid packet, it means that this packet have the header and the tail,
	we need to process it again here, such as judge packet length, data_size and checksum
*/
static uint8 checkSum(uint8 startIndex, uint8 endIndex)
{
	uint8 index;
	uint8 checksum;
	uint8 *buff = theSerial.rxBuffer;

	// HEADER + LENGTH + CMD + DATA+ CHECKSUM
	#if 0	//debug
	printString("\npkt:");
	for(index = startIndex; index != endIndex; index ++)
		printChar(buff[index]);
	
	printChar(buff[index]);	// checksum
	#endif

	for(checksum = 0, index = startIndex; index != endIndex; index ++)
		checksum += buff[index];
	
	if((checksum + buff[endIndex]) == 0)  //
		return TRUE;

	#if 0	//debug
	printString(" cmd:");
	printChar(buff[startIndex + 1]);
	printString("\n");
	#endif

	return FALSE;
}
// handle the data that received by RX interrupt service routine.
void uartRxLoop(void)
{
	uint8 byte;								// get data from the Rx Buffer 
	uint8 *buff = theSerial.rxBuffer;
	uint8 getIndex = theSerial.rxGetIndex;	// current index of the Rx buffer
	static uint8 rxFindFlag = FLAG_HEAD0;	// this is a flag indicate that which part of packet we need to find
	static uint8 startIndex;
	static uint8 packet_length;
	
	// There is nothing need to handle for this loop
	if(getIndex == theSerial.rxPutIndex)
		return;

	// get the specific byte
	byte = buff[getIndex];

    //serialSendChar(byte);
	
    
	// HEADER + LENGTH + CMD + DATA+ CHECKSUM
	if(rxFindFlag == FLAG_HEAD0)	//Find fist byte of header
	{
		if(byte == packet_header[0]) // if find it, set flag, otherwise, keep to checking next byte
			rxFindFlag = FLAG_HEAD1;

	}
	else if(rxFindFlag == FLAG_HEAD1)
	{
		if(byte == packet_header[1])
			rxFindFlag = FLAG_LENGTH;
		else if(byte = packet_header[0])// do not update the read index, because this situation will be ignore: **@xxx
			rxFindFlag = FLAG_HEAD1;
        else
            rxFindFlag = FLAG_HEAD0;
	}
	else if(rxFindFlag == FLAG_LENGTH)
	{
		packet_length = byte - 1;	// because packet_length contains itself.
		if((byte >= MIN_PKT_LENGTH) && (byte < (256 - MIN_PKT_LENGTH)))
		{
			rxFindFlag = FLAG_PDU;
			startIndex = getIndex;
		}
		else
			rxFindFlag = FLAG_HEAD0;
	}
	else if(rxFindFlag == FLAG_PDU)
	{
		packet_length --;
		if(packet_length == 0)
		{
			if(checkSum(startIndex, getIndex))
				cmdHandler(buff[startIndex + 1], startIndex + 2, buff[startIndex] - MIN_PKT_LENGTH);
			
			rxFindFlag = FLAG_HEAD0;
		}
	}

	theSerial.rxGetIndex ++;
}

// transmit one byte each cycle.
#if 0
void txLoop(void)
{
	if((theSerial.txPutIndex != theSerial.txGetIndex) && ((ASIF6 & 0x02) == 0))
		TXB6 = theSerial.txBuffer[theSerial.txGetIndex ++];
}
#endif

void serialLoop(void)
{
	//txLoop();
	rxLoop();
}

void serialEnable(uint8 enable)
{
	if(enable)
	{
        STMK0 = 0;  /* INTST0 enable */
        SRMK0 = 0;  /* INTSR0 enable */
        POWER0 = 1; /* uart0 enable */
        TXE0 = 1;   /* uart0 transmit enable */
        RXE0 = 1;   /* uart0 receive enable */
	}
	else
	{
        TXE0 = 0;   /* uart0 transmit disable */
        RXE0 = 0;   /* uart0 receive disable */
        POWER0 = 0; /* uart0 disable */
        STMK0 = 1;  /* INTST0 disable */
        SRMK0 = 1;  /* INTSR0 disable */
        STIF0 = 0;  /* INTST0 IF clear */
        SRIF0 = 0;  /* INTSR0 IF clear */
	}

    // RX
	theSerial.rxPutIndex = 0;
	theSerial.rxGetIndex = 0;
	// TX
	theSerial.txPutIndex = 0;
	theSerial.txGetIndex = 0;
}

void serialInit(void)
{
	TXE0 = 0;	/* uart0 transmit disable */
	RXE0 = 0;	/* uart0 receive disable */
	STMK0 = 1;	/* INTST0 disable */
	SRMK0 = 1;	/* INTSR0 disable */
	STIF0 = 0;	/* INTST0 IF clear */
	SRIF0 = 0;	/* INTSR0 IF clear */

	BRGC0 = UART0_BASECLK_3 | UART0_BASECLK_DIVISION;
	ASIM0 = UART0_ASIM0_INITIALVALUE | UART0_DATA_LENGTH_8|UART0_STOP_BIT_1|UART0_PARITY_NONE;
	/* INTST0 priority low */
	STPR0 = 1;
	/* INTSR0 priority low */
	SRPR0 = 1;
	/* TXD0 pin setting */
	P1 |= 0x01;
	PM1 &= ~0x01;
	/* RXD0 pin setting */
	PM1 |= 0x02;
}

__interrupt void MD_INTST0( void )
{
    uart0_sending_flag = FALSE;
    
	if(theSerial.txGetIndex != theSerial.txPutIndex)
    {
		//if(((ASIF0 & 0x02) == 0) && ((ASIM0 & 0x80) == 0x80))
			TXS0 = theSerial.txBuffer[theSerial.txGetIndex ++];
            uart0_sending_flag = TRUE;
	}    
}

__interrupt void MD_INTSR0(void)
{
    uint8 error = ASIS0;
    uint8 byte = RXB0;
    
	if(error & 0x07)
		return;
	
	theSerial.rxBuffer[theSerial.rxPutIndex ++] = byte;
}
#else
	static char dumy = 0;
#endif

