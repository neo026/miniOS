
#ifndef _SERIAL_H
#define _SERIAL_H
/*
*******************************************************************************
**	Register bit define
*******************************************************************************
*/
/*
*******************************************************************************
**	UART0
*******************************************************************************
*/
/* Asynchronous serial interface operation mode register (ASIM0) */
#define UART0_ASIM0_INITIALVALUE	0x01

/* Enable/disable UART0 operation of internal operation clock(POWER0) */
#define	UART0_POWER_ON		0x80	/* enable operation of the internal operation clock */
#define	UART0_POWER_OFF		0x00	/* disable operation of the internal operation clock */

/* UART0 transfer parity(PS01,PS00) */
#define	UART0_PARITY_NONE	0x00		/* transfer without parity */
#define	UART0_PARITY_ZERO	0x08		/* judges as 0 parity */
#define	UART0_PARITY_ODD	0x10		/* judges as odd parity */
#define	UART0_PARITY_EVEN	0x18		/* judges as even parity */
										
/* UART0 character length of transfer data(CL0) */
#define	UART0_DATA_LENGTH_8	0x04		/* character length of data = 8 bits */
#define	UART0_DATA_LENGTH_7	0x00		/* character length of data = 7 bits */

/* UART0 stop bits of transmit data(SL0) */
#define	UART0_STOP_BIT_2	0x02		/* stop bits = 2 */
#define	UART0_STOP_BIT_1	0x00		/* stop bits = 1 */

/* Asynchronous	serial interface Receive error status register (ASIS0) */
/* Status flag indicating parity error(PE0) */
#define	UART0_PARITY_ERROR	0x04
/* Status flag indicating framing error(FE0) */
#define	UART0_FRAMING_ERROR	0x02
/* Status flag indicating overrun error(OVE0) */
#define	UART0_OVERRUN_ERROR	0x01

/* Format of Baud Rate Generator Control Register 0 (BRGC0) */
/* Base clock selection (TPS01,TPS00) */
#define	UART0_BASECLK_1		0x00		/* fxclk0 = TM50 output */
#define	UART0_BASECLK_2		0x40		/* fxclk0 = fprs/2 */
#define	UART0_BASECLK_3		0x80		/* fxclk0 = fprs/8 */
#define	UART0_BASECLK_4		0xc0		/* fxclk0 = fprs/32 */

/* Selection of 5-bit counter output clock (MDL04~MDL00) */
#define	UART0_BASECLK_DIVISION		0xd	/* 8 ~ 31 */
/*
*******************************************************************************
**   UART6n
*******************************************************************************
*/

/* Asynchronous serial interface operation mode register (ASIM6n) */
/* Enable/disable UART6 operation of internal operation clock(POWER6n) */
#define UART6_POWER_ON		0x80	/* enable operation of the internal operation clock */
#define UART6_POWER_OFF		0x00	/* disable operation of the internal operation clock */

/*uart6 transfer parity(PS01,PS00)*/
#define UART6_PARITY_NONE	0x00	/* transfer without parity */
#define UART6_PARITY_ZERO	0x08	/* judges as 0 parity */
#define UART6_PARITY_ODD	0x10	/* judges as odd parity */
#define UART6_PARITY_EVEN	0x18	/* judges as even parity */

/* uart6 character length of transfer data(CL0) */
#define UART6_DATA_LENGTH_8	0x04	/* character length of data = 8 bits */
#define UART6_DATA_LENGTH_7	0x00	/* character length of data = 7 bits */

/* uart6 stop bits of transmit data(SL0) */
#define UART6_STOP_BIT_2	0x02	/* stop bits = 2 */
#define UART6_STOP_BIT_1	0x00	/* stop bits = 1 */

/* uart6 occurrence of reception completion interrupt in case of error(ISRM6) */
#define UART6_ERR_INTSRE6	0x00	/* INTSRE6 occurs in case of error */
#define UART6_ERR_INTSR6	0x01	/* INTSR6 occurs in case of error */

/* Asynchronous serial interface reception error status register (ASIS6) */
/* Status flag indicating parity error(PE6) */
#define UART6_PARITY_ERROR	0x04
/* Status flag indicating framing error(FE6) */
#define UART6_FRAMING_ERROR	0x02
/* Status flag indicating overrun error(OVE6) */
#define UART6_OVERRUN_ERROR	0x01 

/* Format of baud rate generator control register 0 (CKSR6) */
#define UART6_BASECLK_1		0x00	/* fxclk0 = fprs */
#define UART6_BASECLK_2		0x01	/* fxclk0 = fprs/2  */
#define UART6_BASECLK_3		0x02	/* fxclk0 = fprs/4  */
#define UART6_BASECLK_4		0x03	/* fxclk0 = fprs/8  */
#define UART6_BASECLK_5		0x04	/* fxclk0 = fprs/16 */
#define UART6_BASECLK_6		0x05	/* fxclk0 = fprs/32 */
#define UART6_BASECLK_7		0x06	/* fxclk0 = fprs/64 */
#define UART6_BASECLK_8		0x07	/* fxclk0 = fprs/128 */
#define UART6_BASECLK_9		0x08	/* fxclk0 = fprs/256 */
#define UART6_BASECLK_10	0x09	/* fxclk0 = fprs/512 */
#define UART6_BASECLK_11	0x0a	/* fxclk0 = fprs/1024*/
#define UART6_BASECLK_12	0x0b	/* fxclk0 = TM50 output */

/* Format of asynchronous serial interface control register 6 (ASICL6) */
/* First-bit direction(DIR6) */
#define UART6_FIRST_BIT_LSB	0x02	/* LSB */
#define UART6_FIRST_BIT_MSB	0x00	/* MSB */

/* Enable/disable inverting TxD6 output(TXDLV6) */
#define UART6_TXD6_OUTPUT_INVERTED	0x01	/* TxD6 output inverted */
#define UART6_TXD6_OUTPUT_NORMAL	0x00	/* TxD6 output normal */

/* Format of asynchronous serial interface transmission status register 6 (ASIF6) */
#define UART6_TRANSMIT_BUFFER_FLAG	0x02	/* if data exists in TXB6,the data is written to transmit buffer register 6 (TXB6) */
#define UART6_TRANSMIT_SHIFT_FLAG	0x01	/* if data transmission is in progress,the data is transferred from transmit buffer register 6 (TXB6) */
/*
	Serial operation mode register (CSIM1n)
*/

/* Receive/transfer mode control (TRMD1n) */  
#define	CSI1_MODE_RECEIVE	0x00	/* receive mode */
#define	CSI1_MODE_BOTH		0x40	/* transmit/receive mode */

/* First bit specification (DIR1n) */
#define	CSI1_FIRST_BIT_MSB	0x00	/* MSB */ 
#define	CSI1_FIRST_BIT_LSB	0x10	/* LSB */

/* SSI11 pin use selection (SSE11) */    
#define	CSI1_SSI11_UNUSED	0x00	/* SSI11 pin is not used */
#define	CSI1_SSI11_USED		0x20	/* SSI11 pin is used */

/*
	Serial clock selection register (CSIC1n)
*/

/* Receive/transmit timing (CKP1n,DAP1n) */	
#define	CSI1_DATA_TIMING_1	0x00	/* type 1 */
#define	CSI1_DATA_TIMING_2	0x08	/* type 2 */
#define	CSI1_DATA_TIMING_3	0x10	/* type 3 */
#define	CSI1_DATA_TIMING_4	0x18	/* type 4 */

/* Clock selection (CKS1n2,CKS1n1,CKS1n0) */
#define	CSI1_CLOCK_1		0x00	/* fprs/2 */
#define	CSI1_CLOCK_2		0x01	/* fprs/4 */
#define	CSI1_CLOCK_3		0x02	/* fprs/8 */	
#define	CSI1_CLOCK_4		0x03	/* fprs/16 */
#define	CSI1_CLOCK_5		0x04	/* fprs/32 */
#define	CSI1_CLOCK_6		0x05	/* fprs/64 */
#define	CSI1_CLOCK_7		0x06	/* fprs/128 */
#define	CSI1_CLOCK_EXT		0x07	/* external clock */

/*
*******************************************************************************
**  Global variables
*******************************************************************************
*/
#define	UART_BAUDRATE_M6	0x0
#define	UART_BAUDRATE_K6	0x68

/* Received Commands*/
enum
{
    CMD_NONE,
	CMD_POWERON,
	CMD_POWEROFF,
	CMD_GET_STATUS,
	CMD_KEY,
	CMD_AMP,
	CMD_RESTORE,
	CMD_SOURCE,
	CMD_UPGRADE,
	CMD_AUXIN_STATUS,
	CMD_EQ_TEST,
	CMD_LOW_BATTERY,
};

enum
{
   SOURCE_BT,
   SOURCE_AUX1,
   SOURCE_AUX2,
   SOURCE_TOP,
};

enum
{
    KEY_TYPE_NULL = 0x00,
    KEY_TYPE_DBL_CLICK = 0x20,
    KEY_TYPE_DOWN = 0x10,
    KEY_TYPE_LONG = 0x08,
    KEY_TYPE_HOLD = 0x04,
    KEY_TYPE_SHORT_UP = 0x02,
    KEY_TYPE_LONG_UP = 0x01,
    KEY_TYPE_ALL = 0x3f
};


enum
{
	U_KEY_NULL,         // 
	U_KEY_PLAY  = 0x01, // play and pause
	U_KEY_BACKWARD = 0x0F,  //backward
	U_KEY_FORWARD = 0x0E,   // forward
	
	
    U_KEY_VOLUME_UP = 0x05,
	U_KEY_VOLUME_DOWN = 0x06,
	U_KEY_MODE = 0x07,   // bt pairing
	U_KEY_EQ = 0x08,
	U_KEY_MUTE = 0x0A,
	
	
	U_KEY_PHONE = 0x1F, // phone call
	U_KEY_POWERON = 0x20,
	U_KEY_POWEROFF = 0x21,    
	U_KEY_BQB = 0x24,
	U_KEY_SIRI_REJECT = 0x25,

    KEY_TOP,
};

/* Response acknowledge value */
#define ACK_OK				0x00
#define ACK_WRONG_LENGTH	0x01
#define ACK_WRONG_STATUS	0x02

/*Following program is for Serial RX and TX work, this set of code is NOT for debuging information.*/
#define SERIAL_RX_BUFFER_LENGTH	255	//
#define SERIAL_TX_BUFFER_LENGTH	127	//

typedef struct 
{
	uint8 rxBuffer[SERIAL_RX_BUFFER_LENGTH + 1];	// ring buffer
	uint8 rxPutIndex;	// put RX register data into the rxBuffer, this will done at RX interrupt service handle rountine
	uint8 rxGetIndex;	// get received data from the rxBuffer, that data will be handled by main while(1).

	uint8 txBuffer[SERIAL_TX_BUFFER_LENGTH + 1];
	uint8 txPutIndex:7;
	uint8 txGetIndex:7;
}serial_type;


typedef enum
{
    CONN_STATUS_NONE,
    CONN_STATUS_WAIT_PAIR,
    CONN_STATUS_PAIRED,
    CONN_STATUS_LINKED,
    CONN_STATUS_ERROR,
} conn_status_e;

typedef enum
{
    A2DP_STATUS_NONE,
    A2DP_STATUS_LINKED,
    A2DP_STATUS_PLAY,
    A2DP_STATUS_PAUSE,
} a2dp_status_e;

typedef enum
{
    HFP_STATUS_NONE,
    HFP_STATUS_LINKED,
    HFP_STATUS_INCOMING,
    HFP_STATUS_OUTGOING,
    HFP_STATUS_CALLING,
    HFP_STATUS_PHONE,
	HFP_STATUS_SIRI,
} hfp_status_e;


extern serial_type theSerial;
/*------------------------------------------------*/
void serialInit(void);
void serialEnable(uint8 enable);
void serialLoop(void);
void serialSendNum(uint8 num);
void serialSendString(const char *str);
void serialSendStringWithData(const char *str, uint8 argv);

void sendPacket(uint8 cmd, uint8 *send_data, uint8 data_size);
void sendKeyCmd(uint8 key, uint8 type);
void sendSourceCmd(uint8 source);
void getBtStatus(void);

void cmdHandler(uint8 cmd, uint8 startIndex, uint8 data_size);
//void gernateFrame(void);

#endif // _SERIAL_H
