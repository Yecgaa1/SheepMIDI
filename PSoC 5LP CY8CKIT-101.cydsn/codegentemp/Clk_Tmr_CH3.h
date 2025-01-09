/*******************************************************************************
* File Name: Clk_Tmr_CH3.h
* Version 2.20
*
*  Description:
*   Provides the function and constant definitions for the clock component.
*
*  Note:
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_CLOCK_Clk_Tmr_CH3_H)
#define CY_CLOCK_Clk_Tmr_CH3_H

#include <cytypes.h>
#include <cyfitter.h>


/***************************************
* Conditional Compilation Parameters
***************************************/

/* Check to see if required defines such as CY_PSOC5LP are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5LP)
    #error Component cy_clock_v2_20 requires cy_boot v3.0 or later
#endif /* (CY_PSOC5LP) */


/***************************************
*        Function Prototypes
***************************************/

void Clk_Tmr_CH3_Start(void) ;
void Clk_Tmr_CH3_Stop(void) ;

#if(CY_PSOC3 || CY_PSOC5LP)
void Clk_Tmr_CH3_StopBlock(void) ;
#endif /* (CY_PSOC3 || CY_PSOC5LP) */

void Clk_Tmr_CH3_StandbyPower(uint8 state) ;
void Clk_Tmr_CH3_SetDividerRegister(uint16 clkDivider, uint8 restart) 
                                ;
uint16 Clk_Tmr_CH3_GetDividerRegister(void) ;
void Clk_Tmr_CH3_SetModeRegister(uint8 modeBitMask) ;
void Clk_Tmr_CH3_ClearModeRegister(uint8 modeBitMask) ;
uint8 Clk_Tmr_CH3_GetModeRegister(void) ;
void Clk_Tmr_CH3_SetSourceRegister(uint8 clkSource) ;
uint8 Clk_Tmr_CH3_GetSourceRegister(void) ;
#if defined(Clk_Tmr_CH3__CFG3)
void Clk_Tmr_CH3_SetPhaseRegister(uint8 clkPhase) ;
uint8 Clk_Tmr_CH3_GetPhaseRegister(void) ;
#endif /* defined(Clk_Tmr_CH3__CFG3) */

#define Clk_Tmr_CH3_Enable()                       Clk_Tmr_CH3_Start()
#define Clk_Tmr_CH3_Disable()                      Clk_Tmr_CH3_Stop()
#define Clk_Tmr_CH3_SetDivider(clkDivider)         Clk_Tmr_CH3_SetDividerRegister(clkDivider, 1u)
#define Clk_Tmr_CH3_SetDividerValue(clkDivider)    Clk_Tmr_CH3_SetDividerRegister((clkDivider) - 1u, 1u)
#define Clk_Tmr_CH3_SetMode(clkMode)               Clk_Tmr_CH3_SetModeRegister(clkMode)
#define Clk_Tmr_CH3_SetSource(clkSource)           Clk_Tmr_CH3_SetSourceRegister(clkSource)
#if defined(Clk_Tmr_CH3__CFG3)
#define Clk_Tmr_CH3_SetPhase(clkPhase)             Clk_Tmr_CH3_SetPhaseRegister(clkPhase)
#define Clk_Tmr_CH3_SetPhaseValue(clkPhase)        Clk_Tmr_CH3_SetPhaseRegister((clkPhase) + 1u)
#endif /* defined(Clk_Tmr_CH3__CFG3) */


/***************************************
*             Registers
***************************************/

/* Register to enable or disable the clock */
#define Clk_Tmr_CH3_CLKEN              (* (reg8 *) Clk_Tmr_CH3__PM_ACT_CFG)
#define Clk_Tmr_CH3_CLKEN_PTR          ((reg8 *) Clk_Tmr_CH3__PM_ACT_CFG)

/* Register to enable or disable the clock */
#define Clk_Tmr_CH3_CLKSTBY            (* (reg8 *) Clk_Tmr_CH3__PM_STBY_CFG)
#define Clk_Tmr_CH3_CLKSTBY_PTR        ((reg8 *) Clk_Tmr_CH3__PM_STBY_CFG)

/* Clock LSB divider configuration register. */
#define Clk_Tmr_CH3_DIV_LSB            (* (reg8 *) Clk_Tmr_CH3__CFG0)
#define Clk_Tmr_CH3_DIV_LSB_PTR        ((reg8 *) Clk_Tmr_CH3__CFG0)
#define Clk_Tmr_CH3_DIV_PTR            ((reg16 *) Clk_Tmr_CH3__CFG0)

/* Clock MSB divider configuration register. */
#define Clk_Tmr_CH3_DIV_MSB            (* (reg8 *) Clk_Tmr_CH3__CFG1)
#define Clk_Tmr_CH3_DIV_MSB_PTR        ((reg8 *) Clk_Tmr_CH3__CFG1)

/* Mode and source configuration register */
#define Clk_Tmr_CH3_MOD_SRC            (* (reg8 *) Clk_Tmr_CH3__CFG2)
#define Clk_Tmr_CH3_MOD_SRC_PTR        ((reg8 *) Clk_Tmr_CH3__CFG2)

#if defined(Clk_Tmr_CH3__CFG3)
/* Analog clock phase configuration register */
#define Clk_Tmr_CH3_PHASE              (* (reg8 *) Clk_Tmr_CH3__CFG3)
#define Clk_Tmr_CH3_PHASE_PTR          ((reg8 *) Clk_Tmr_CH3__CFG3)
#endif /* defined(Clk_Tmr_CH3__CFG3) */


/**************************************
*       Register Constants
**************************************/

/* Power manager register masks */
#define Clk_Tmr_CH3_CLKEN_MASK         Clk_Tmr_CH3__PM_ACT_MSK
#define Clk_Tmr_CH3_CLKSTBY_MASK       Clk_Tmr_CH3__PM_STBY_MSK

/* CFG2 field masks */
#define Clk_Tmr_CH3_SRC_SEL_MSK        Clk_Tmr_CH3__CFG2_SRC_SEL_MASK
#define Clk_Tmr_CH3_MODE_MASK          (~(Clk_Tmr_CH3_SRC_SEL_MSK))

#if defined(Clk_Tmr_CH3__CFG3)
/* CFG3 phase mask */
#define Clk_Tmr_CH3_PHASE_MASK         Clk_Tmr_CH3__CFG3_PHASE_DLY_MASK
#endif /* defined(Clk_Tmr_CH3__CFG3) */

#endif /* CY_CLOCK_Clk_Tmr_CH3_H */


/* [] END OF FILE */
