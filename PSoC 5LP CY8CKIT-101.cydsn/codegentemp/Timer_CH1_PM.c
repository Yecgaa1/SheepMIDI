/*******************************************************************************
* File Name: Timer_CH1_PM.c
* Version 2.80
*
*  Description:
*     This file provides the power management source code to API for the
*     Timer.
*
*   Note:
*     None
*
*******************************************************************************
* Copyright 2008-2017, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
********************************************************************************/

#include "Timer_CH1.h"

static Timer_CH1_backupStruct Timer_CH1_backup;


/*******************************************************************************
* Function Name: Timer_CH1_SaveConfig
********************************************************************************
*
* Summary:
*     Save the current user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  Timer_CH1_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void Timer_CH1_SaveConfig(void) 
{
    #if (!Timer_CH1_UsingFixedFunction)
        Timer_CH1_backup.TimerUdb = Timer_CH1_ReadCounter();
        Timer_CH1_backup.InterruptMaskValue = Timer_CH1_STATUS_MASK;
        #if (Timer_CH1_UsingHWCaptureCounter)
            Timer_CH1_backup.TimerCaptureCounter = Timer_CH1_ReadCaptureCount();
        #endif /* Back Up capture counter register  */

        #if(!Timer_CH1_UDB_CONTROL_REG_REMOVED)
            Timer_CH1_backup.TimerControlRegister = Timer_CH1_ReadControlRegister();
        #endif /* Backup the enable state of the Timer component */
    #endif /* Backup non retention registers in UDB implementation. All fixed function registers are retention */
}


/*******************************************************************************
* Function Name: Timer_CH1_RestoreConfig
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
* Global variables:
*  Timer_CH1_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void Timer_CH1_RestoreConfig(void) 
{   
    #if (!Timer_CH1_UsingFixedFunction)

        Timer_CH1_WriteCounter(Timer_CH1_backup.TimerUdb);
        Timer_CH1_STATUS_MASK =Timer_CH1_backup.InterruptMaskValue;
        #if (Timer_CH1_UsingHWCaptureCounter)
            Timer_CH1_SetCaptureCount(Timer_CH1_backup.TimerCaptureCounter);
        #endif /* Restore Capture counter register*/

        #if(!Timer_CH1_UDB_CONTROL_REG_REMOVED)
            Timer_CH1_WriteControlRegister(Timer_CH1_backup.TimerControlRegister);
        #endif /* Restore the enable state of the Timer component */
    #endif /* Restore non retention registers in the UDB implementation only */
}


/*******************************************************************************
* Function Name: Timer_CH1_Sleep
********************************************************************************
*
* Summary:
*     Stop and Save the user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  Timer_CH1_backup.TimerEnableState:  Is modified depending on the
*  enable state of the block before entering sleep mode.
*
*******************************************************************************/
void Timer_CH1_Sleep(void) 
{
    #if(!Timer_CH1_UDB_CONTROL_REG_REMOVED)
        /* Save Counter's enable state */
        if(Timer_CH1_CTRL_ENABLE == (Timer_CH1_CONTROL & Timer_CH1_CTRL_ENABLE))
        {
            /* Timer is enabled */
            Timer_CH1_backup.TimerEnableState = 1u;
        }
        else
        {
            /* Timer is disabled */
            Timer_CH1_backup.TimerEnableState = 0u;
        }
    #endif /* Back up enable state from the Timer control register */
    Timer_CH1_Stop();
    Timer_CH1_SaveConfig();
}


/*******************************************************************************
* Function Name: Timer_CH1_Wakeup
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
*  Timer_CH1_backup.enableState:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void Timer_CH1_Wakeup(void) 
{
    Timer_CH1_RestoreConfig();
    #if(!Timer_CH1_UDB_CONTROL_REG_REMOVED)
        if(Timer_CH1_backup.TimerEnableState == 1u)
        {     /* Enable Timer's operation */
                Timer_CH1_Enable();
        } /* Do nothing if Timer was disabled before */
    #endif /* Remove this code section if Control register is removed */
}


/* [] END OF FILE */
