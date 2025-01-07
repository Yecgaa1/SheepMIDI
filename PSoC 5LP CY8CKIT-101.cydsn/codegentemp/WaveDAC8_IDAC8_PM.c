/*******************************************************************************
* File Name: WaveDAC8_IDAC8.c
* Version 2.0
*
* Description:
*  This file provides the power management source code to API for the
*  IDAC8.
*
* Note:
*  None
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/


#include "WaveDAC8_IDAC8.h"

static WaveDAC8_IDAC8_backupStruct WaveDAC8_IDAC8_backup;


/*******************************************************************************
* Function Name: WaveDAC8_IDAC8_SaveConfig
********************************************************************************
* Summary:
*  Save the current user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void WaveDAC8_IDAC8_SaveConfig(void) 
{
    if (!((WaveDAC8_IDAC8_CR1 & WaveDAC8_IDAC8_SRC_MASK) == WaveDAC8_IDAC8_SRC_UDB))
    {
        WaveDAC8_IDAC8_backup.data_value = WaveDAC8_IDAC8_Data;
    }
}


/*******************************************************************************
* Function Name: WaveDAC8_IDAC8_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the current user configuration.
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void WaveDAC8_IDAC8_RestoreConfig(void) 
{
    if (!((WaveDAC8_IDAC8_CR1 & WaveDAC8_IDAC8_SRC_MASK) == WaveDAC8_IDAC8_SRC_UDB))
    {
        if((WaveDAC8_IDAC8_Strobe & WaveDAC8_IDAC8_STRB_MASK) == WaveDAC8_IDAC8_STRB_EN)
        {
            WaveDAC8_IDAC8_Strobe &= (uint8)(~WaveDAC8_IDAC8_STRB_MASK);
            WaveDAC8_IDAC8_Data = WaveDAC8_IDAC8_backup.data_value;
            WaveDAC8_IDAC8_Strobe |= WaveDAC8_IDAC8_STRB_EN;
        }
        else
        {
            WaveDAC8_IDAC8_Data = WaveDAC8_IDAC8_backup.data_value;
        }
    }
}


/*******************************************************************************
* Function Name: WaveDAC8_IDAC8_Sleep
********************************************************************************
* Summary:
*  Stop and Save the user configuration
*
* Parameters:
*  void:
*
* Return:
*  void
*
* Global variables:
*  WaveDAC8_IDAC8_backup.enableState: Is modified depending on the enable 
*  state of the block before entering sleep mode.
*
*******************************************************************************/
void WaveDAC8_IDAC8_Sleep(void) 
{
    if(WaveDAC8_IDAC8_ACT_PWR_EN == (WaveDAC8_IDAC8_PWRMGR & WaveDAC8_IDAC8_ACT_PWR_EN))
    {
        /* IDAC8 is enabled */
        WaveDAC8_IDAC8_backup.enableState = 1u;
    }
    else
    {
        /* IDAC8 is disabled */
        WaveDAC8_IDAC8_backup.enableState = 0u;
    }

    WaveDAC8_IDAC8_Stop();
    WaveDAC8_IDAC8_SaveConfig();
}


/*******************************************************************************
* Function Name: WaveDAC8_IDAC8_Wakeup
********************************************************************************
*
* Summary:
*  Restores and enables the user configuration
*  
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  WaveDAC8_IDAC8_backup.enableState: Is used to restore the enable state of 
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void WaveDAC8_IDAC8_Wakeup(void) 
{
    WaveDAC8_IDAC8_RestoreConfig();
    
    if(WaveDAC8_IDAC8_backup.enableState == 1u)
    {
        /* Enable IDAC8's operation */
        WaveDAC8_IDAC8_Enable();
        
        /* Set the data register */
        WaveDAC8_IDAC8_SetValue(WaveDAC8_IDAC8_Data);
    } /* Do nothing if IDAC8 was disabled before */    
}


/* [] END OF FILE */
