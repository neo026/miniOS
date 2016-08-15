/*


*/
#if ENABLE_SERIAL
#include "../macrodriver.h"
#include "../message.h"
#include "serial.h"
#include "../private.h"

/* */
void cmdPhoneNumber(uint8 startIndex, uint8 data_size)
{
	uint8 i;
	uint8 buffer[11];
	if(data_size > 11)

	for(i = 0; i < data_size; i++)
	{
		buffer[i] = theSerial.rxBuffer[startIndex ++];
	}
}

void cmdWriteEeprom(uint8 data_size)
{
	uint8 ackBuff[] = "OK";
	//sendPacket(CMD_SETLED, ackBuff, sizeof(ackBuff));
}

void cmdAudioMode(uint8 startIndex, uint8 data_size)
{
    uint8 mode = theSerial.rxBuffer[startIndex];

    switch(mode)
    {
        case SOURCE_AUX1:

            break;

        case SOURCE_AUX2:

            break;

        case SOURCE_BT:

            break;
    }
}

void cmdGetStatus(uint8 startIndex, uint8 data_size)
{
    uint8 source, conn_status, btplay_status, btcall_status;
    uint8 state = StateBtNone;
    
    //debugMsg("\nBtStatus=",data_size);    

    if(data_size < 4)
        return ;

    source          = theSerial.rxBuffer[startIndex ++];
    conn_status     = theSerial.rxBuffer[startIndex ++];
    btplay_status   = theSerial.rxBuffer[startIndex ++];
    btcall_status   = theSerial.rxBuffer[startIndex];

    #if 0
    debugMsg("src=", source);
    debugMsg("conn=", conn_status);
    debugMsg("a2dp=", btplay_status);
    debugMsg("hfp=", btcall_status);
    #endif

    if(SOURCE_BT == source)
    {
        switch(conn_status)
        {
            case CONN_STATUS_NONE:
            case CONN_STATUS_ERROR:
                state = StateBtNone;
                break;

            case CONN_STATUS_WAIT_PAIR:
            case CONN_STATUS_PAIRED:
                state = StateBtPairing;
                break;

            case CONN_STATUS_LINKED:
                state = StateBtConnected;
                break;
        }

        if(StateBtConnected == state)
        {
            switch(btcall_status)
            {
                case HFP_STATUS_INCOMING:
                    state = StateBtIncomingCall;
                    break;

                case HFP_STATUS_OUTGOING:
                case HFP_STATUS_SIRI:
                    state = StateBtOutgoingCall;
                    break;

                case HFP_STATUS_PHONE:
                case HFP_STATUS_CALLING:
                    state = StateBtAnswer;
                    break;

                default:
                    {
                       if(A2DP_STATUS_PLAY == btplay_status)
                       {
                          state = StateBtPlay;
                       }
                    }
                    break;
            }
        }
                    
    }
    else
    {
        
    }
        
        
    if(state != gProject.btState)
    {
        messageSend(EventBtState, 0, 0);
        gProject.btState = state;
    }
    
    //gProject.source = source;
}

void cmdUgrade(uint8 startIndex, uint8 data_size)
{
    uint8 isReady = theSerial.rxBuffer[startIndex];

    if(isReady == TRUE)
    {
        //now, bt module is ready to upgrade firmware.
        GPIO_UPGRADE(ON);
        GPIO_WAKEUP(ON);
        
    }
    else
    {

    }
}

/* command handle*/
void cmdHandler(uint8 cmd, uint8 startIndex, uint8 data_size)
{
	switch(cmd)
	{
	    case CMD_GET_STATUS:
            cmdGetStatus(startIndex, data_size);
            break;
            
		case CMD_POWERON:
			messageSend(EventPowerOn, 1, 0);
			break;

        case CMD_SOURCE:
            cmdAudioMode(startIndex,data_size);
            break;

        case CMD_UPGRADE:
            cmdUgrade(startIndex, data_size);
            break;
          
		default:
			break;
	}
}

#else
	static char dumy = 0;
#endif