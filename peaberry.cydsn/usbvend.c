/* ========================================
 *
 * Copyright 2012 David Turnbull
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF DAVID TURNBULL.
 *
 * ========================================
*/

#include "device.h"
//#include "USBFS.h"
#include "main.h"
#include "si570.h"

uint8 USBFS_InitControlRead(void);
uint8 USBFS_InitControlWrite(void);
extern volatile T_USBFS_TD USBFS_currentTD;
extern volatile uint32 Si570_LO;

uint8 key_return;

uint8 USBFS_HandleVendorRqst(void) 
{
    uint8 requestHandled = USBFS_FALSE;
    uint8 reqType, reqCmd;
    
    reqType = CY_GET_REG8(USBFS_bmRequestType);
    reqCmd = CY_GET_REG8(USBFS_bRequest);

    if ((reqType & USBFS_RQST_DIR_MASK) == USBFS_RQST_DIR_D2H)
    {
        switch (reqCmd)
        {
            case 0x3A: // CMD_GET_FREQ
                USBFS_currentTD.pData = (void *)&Si570_LO;
                USBFS_currentTD.count = sizeof(Si570_LO);
                requestHandled  = USBFS_InitControlRead();
            case 0x50: // CMD_SET_USRP1
                if (CY_GET_REG8(USBFS_wValueLo) & 0x01) {
                    Control_Write(Control_Read() | CONTROL_TX_ENABLE);
                } else {
                    Control_Write(Control_Read() & ~CONTROL_TX_ENABLE);
                }
                //nobreak, returns key value
            case 0x51: // CMD_GET_CW_KEY
                key_return = 0;//TODO convert key bits
                USBFS_currentTD.pData = (void *)&key_return;
                USBFS_currentTD.count = sizeof(key_return);
                requestHandled  = USBFS_InitControlRead();
            default:
                break;
        }
    }
    if ((reqType & USBFS_RQST_DIR_MASK) == USBFS_RQST_DIR_H2D)
    {
        switch (reqCmd)
        {
            case 0x32: // CMD_SET_FREQ
                USBFS_currentTD.pData = (void *)&Si570_LO;
                USBFS_currentTD.count = sizeof(Si570_LO);
                requestHandled  = USBFS_InitControlWrite();
                break;
            default:
                break;
        }
    }

    return(requestHandled);
}

