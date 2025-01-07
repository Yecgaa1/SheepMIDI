/*******************************************************************************
* File Name: Vo1.h  
* Version 2.20
*
* Description:
*  This file contains Pin function prototypes and register defines
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_Vo1_H) /* Pins Vo1_H */
#define CY_PINS_Vo1_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "Vo1_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 Vo1__PORT == 15 && ((Vo1__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    Vo1_Write(uint8 value);
void    Vo1_SetDriveMode(uint8 mode);
uint8   Vo1_ReadDataReg(void);
uint8   Vo1_Read(void);
void    Vo1_SetInterruptMode(uint16 position, uint16 mode);
uint8   Vo1_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the Vo1_SetDriveMode() function.
     *  @{
     */
        #define Vo1_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define Vo1_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define Vo1_DM_RES_UP          PIN_DM_RES_UP
        #define Vo1_DM_RES_DWN         PIN_DM_RES_DWN
        #define Vo1_DM_OD_LO           PIN_DM_OD_LO
        #define Vo1_DM_OD_HI           PIN_DM_OD_HI
        #define Vo1_DM_STRONG          PIN_DM_STRONG
        #define Vo1_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define Vo1_MASK               Vo1__MASK
#define Vo1_SHIFT              Vo1__SHIFT
#define Vo1_WIDTH              1u

/* Interrupt constants */
#if defined(Vo1__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in Vo1_SetInterruptMode() function.
     *  @{
     */
        #define Vo1_INTR_NONE      (uint16)(0x0000u)
        #define Vo1_INTR_RISING    (uint16)(0x0001u)
        #define Vo1_INTR_FALLING   (uint16)(0x0002u)
        #define Vo1_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define Vo1_INTR_MASK      (0x01u) 
#endif /* (Vo1__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define Vo1_PS                     (* (reg8 *) Vo1__PS)
/* Data Register */
#define Vo1_DR                     (* (reg8 *) Vo1__DR)
/* Port Number */
#define Vo1_PRT_NUM                (* (reg8 *) Vo1__PRT) 
/* Connect to Analog Globals */                                                  
#define Vo1_AG                     (* (reg8 *) Vo1__AG)                       
/* Analog MUX bux enable */
#define Vo1_AMUX                   (* (reg8 *) Vo1__AMUX) 
/* Bidirectional Enable */                                                        
#define Vo1_BIE                    (* (reg8 *) Vo1__BIE)
/* Bit-mask for Aliased Register Access */
#define Vo1_BIT_MASK               (* (reg8 *) Vo1__BIT_MASK)
/* Bypass Enable */
#define Vo1_BYP                    (* (reg8 *) Vo1__BYP)
/* Port wide control signals */                                                   
#define Vo1_CTL                    (* (reg8 *) Vo1__CTL)
/* Drive Modes */
#define Vo1_DM0                    (* (reg8 *) Vo1__DM0) 
#define Vo1_DM1                    (* (reg8 *) Vo1__DM1)
#define Vo1_DM2                    (* (reg8 *) Vo1__DM2) 
/* Input Buffer Disable Override */
#define Vo1_INP_DIS                (* (reg8 *) Vo1__INP_DIS)
/* LCD Common or Segment Drive */
#define Vo1_LCD_COM_SEG            (* (reg8 *) Vo1__LCD_COM_SEG)
/* Enable Segment LCD */
#define Vo1_LCD_EN                 (* (reg8 *) Vo1__LCD_EN)
/* Slew Rate Control */
#define Vo1_SLW                    (* (reg8 *) Vo1__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define Vo1_PRTDSI__CAPS_SEL       (* (reg8 *) Vo1__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define Vo1_PRTDSI__DBL_SYNC_IN    (* (reg8 *) Vo1__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define Vo1_PRTDSI__OE_SEL0        (* (reg8 *) Vo1__PRTDSI__OE_SEL0) 
#define Vo1_PRTDSI__OE_SEL1        (* (reg8 *) Vo1__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define Vo1_PRTDSI__OUT_SEL0       (* (reg8 *) Vo1__PRTDSI__OUT_SEL0) 
#define Vo1_PRTDSI__OUT_SEL1       (* (reg8 *) Vo1__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define Vo1_PRTDSI__SYNC_OUT       (* (reg8 *) Vo1__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(Vo1__SIO_CFG)
    #define Vo1_SIO_HYST_EN        (* (reg8 *) Vo1__SIO_HYST_EN)
    #define Vo1_SIO_REG_HIFREQ     (* (reg8 *) Vo1__SIO_REG_HIFREQ)
    #define Vo1_SIO_CFG            (* (reg8 *) Vo1__SIO_CFG)
    #define Vo1_SIO_DIFF           (* (reg8 *) Vo1__SIO_DIFF)
#endif /* (Vo1__SIO_CFG) */

/* Interrupt Registers */
#if defined(Vo1__INTSTAT)
    #define Vo1_INTSTAT            (* (reg8 *) Vo1__INTSTAT)
    #define Vo1_SNAP               (* (reg8 *) Vo1__SNAP)
    
	#define Vo1_0_INTTYPE_REG 		(* (reg8 *) Vo1__0__INTTYPE)
#endif /* (Vo1__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_Vo1_H */


/* [] END OF FILE */
