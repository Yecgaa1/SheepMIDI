/*******************************************************************************
* File Name: MatrixKbLED_isr_MKbLED.h
* Version 1.70
*
*  Description:
*   Provides the function definitions for the Interrupt Controller.
*
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/
#if !defined(CY_ISR_MatrixKbLED_isr_MKbLED_H)
#define CY_ISR_MatrixKbLED_isr_MKbLED_H


#include <cytypes.h>
#include <cyfitter.h>

/* Interrupt Controller API. */
void MatrixKbLED_isr_MKbLED_Start(void);
void MatrixKbLED_isr_MKbLED_StartEx(cyisraddress address);
void MatrixKbLED_isr_MKbLED_Stop(void);

CY_ISR_PROTO(MatrixKbLED_isr_MKbLED_Interrupt);

void MatrixKbLED_isr_MKbLED_SetVector(cyisraddress address);
cyisraddress MatrixKbLED_isr_MKbLED_GetVector(void);

void MatrixKbLED_isr_MKbLED_SetPriority(uint8 priority);
uint8 MatrixKbLED_isr_MKbLED_GetPriority(void);

void MatrixKbLED_isr_MKbLED_Enable(void);
uint8 MatrixKbLED_isr_MKbLED_GetState(void);
void MatrixKbLED_isr_MKbLED_Disable(void);

void MatrixKbLED_isr_MKbLED_SetPending(void);
void MatrixKbLED_isr_MKbLED_ClearPending(void);


/* Interrupt Controller Constants */

/* Address of the INTC.VECT[x] register that contains the Address of the MatrixKbLED_isr_MKbLED ISR. */
#define MatrixKbLED_isr_MKbLED_INTC_VECTOR            ((reg32 *) MatrixKbLED_isr_MKbLED__INTC_VECT)

/* Address of the MatrixKbLED_isr_MKbLED ISR priority. */
#define MatrixKbLED_isr_MKbLED_INTC_PRIOR             ((reg8 *) MatrixKbLED_isr_MKbLED__INTC_PRIOR_REG)

/* Priority of the MatrixKbLED_isr_MKbLED interrupt. */
#define MatrixKbLED_isr_MKbLED_INTC_PRIOR_NUMBER      MatrixKbLED_isr_MKbLED__INTC_PRIOR_NUM

/* Address of the INTC.SET_EN[x] byte to bit enable MatrixKbLED_isr_MKbLED interrupt. */
#define MatrixKbLED_isr_MKbLED_INTC_SET_EN            ((reg32 *) MatrixKbLED_isr_MKbLED__INTC_SET_EN_REG)

/* Address of the INTC.CLR_EN[x] register to bit clear the MatrixKbLED_isr_MKbLED interrupt. */
#define MatrixKbLED_isr_MKbLED_INTC_CLR_EN            ((reg32 *) MatrixKbLED_isr_MKbLED__INTC_CLR_EN_REG)

/* Address of the INTC.SET_PD[x] register to set the MatrixKbLED_isr_MKbLED interrupt state to pending. */
#define MatrixKbLED_isr_MKbLED_INTC_SET_PD            ((reg32 *) MatrixKbLED_isr_MKbLED__INTC_SET_PD_REG)

/* Address of the INTC.CLR_PD[x] register to clear the MatrixKbLED_isr_MKbLED interrupt. */
#define MatrixKbLED_isr_MKbLED_INTC_CLR_PD            ((reg32 *) MatrixKbLED_isr_MKbLED__INTC_CLR_PD_REG)


#endif /* CY_ISR_MatrixKbLED_isr_MKbLED_H */


/* [] END OF FILE */
