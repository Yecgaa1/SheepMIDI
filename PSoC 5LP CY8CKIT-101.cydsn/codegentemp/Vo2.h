/*******************************************************************************
* File Name: Vo2.h  
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

#if !defined(CY_PINS_Vo2_H) /* Pins Vo2_H */
#define CY_PINS_Vo2_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "Vo2_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 Vo2__PORT == 15 && ((Vo2__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    Vo2_Write(uint8 value);
void    Vo2_SetDriveMode(uint8 mode);
uint8   Vo2_ReadDataReg(void);
uint8   Vo2_Read(void);
void    Vo2_SetInterruptMode(uint16 position, uint16 mode);
uint8   Vo2_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the Vo2_SetDriveMode() function.
     *  @{
     */
        #define Vo2_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define Vo2_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define Vo2_DM_RES_UP          PIN_DM_RES_UP
        #define Vo2_DM_RES_DWN         PIN_DM_RES_DWN
        #define Vo2_DM_OD_LO           PIN_DM_OD_LO
        #define Vo2_DM_OD_HI           PIN_DM_OD_HI
        #define Vo2_DM_STRONG          PIN_DM_STRONG
        #define Vo2_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define Vo2_MASK               Vo2__MASK
#define Vo2_SHIFT              Vo2__SHIFT
#define Vo2_WIDTH              1u

/* Interrupt constants */
#if defined(Vo2__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in Vo2_SetInterruptMode() function.
     *  @{
     */
        #define Vo2_INTR_NONE      (uint16)(0x0000u)
        #define Vo2_INTR_RISING    (uint16)(0x0001u)
        #define Vo2_INTR_FALLING   (uint16)(0x0002u)
        #define Vo2_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define Vo2_INTR_MASK      (0x01u) 
#endif /* (Vo2__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define Vo2_PS                     (* (reg8 *) Vo2__PS)
/* Data Register */
#define Vo2_DR                     (* (reg8 *) Vo2__DR)
/* Port Number */
#define Vo2_PRT_NUM                (* (reg8 *) Vo2__PRT) 
/* Connect to Analog Globals */                                                  
#define Vo2_AG                     (* (reg8 *) Vo2__AG)                       
/* Analog MUX bux enable */
#define Vo2_AMUX                   (* (reg8 *) Vo2__AMUX) 
/* Bidirectional Enable */                                                        
#define Vo2_BIE                    (* (reg8 *) Vo2__BIE)
/* Bit-mask for Aliased Register Access */
#define Vo2_BIT_MASK               (* (reg8 *) Vo2__BIT_MASK)
/* Bypass Enable */
#define Vo2_BYP                    (* (reg8 *) Vo2__BYP)
/* Port wide control signals */                                                   
#define Vo2_CTL                    (* (reg8 *) Vo2__CTL)
/* Drive Modes */
#define Vo2_DM0                    (* (reg8 *) Vo2__DM0) 
#define Vo2_DM1                    (* (reg8 *) Vo2__DM1)
#define Vo2_DM2                    (* (reg8 *) Vo2__DM2) 
/* Input Buffer Disable Override */
#define Vo2_INP_DIS                (* (reg8 *) Vo2__INP_DIS)
/* LCD Common or Segment Drive */
#define Vo2_LCD_COM_SEG            (* (reg8 *) Vo2__LCD_COM_SEG)
/* Enable Segment LCD */
#define Vo2_LCD_EN                 (* (reg8 *) Vo2__LCD_EN)
/* Slew Rate Control */
#define Vo2_SLW                    (* (reg8 *) Vo2__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define Vo2_PRTDSI__CAPS_SEL       (* (reg8 *) Vo2__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define Vo2_PRTDSI__DBL_SYNC_IN    (* (reg8 *) Vo2__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define Vo2_PRTDSI__OE_SEL0        (* (reg8 *) Vo2__PRTDSI__OE_SEL0) 
#define Vo2_PRTDSI__OE_SEL1        (* (reg8 *) Vo2__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define Vo2_PRTDSI__OUT_SEL0       (* (reg8 *) Vo2__PRTDSI__OUT_SEL0) 
#define Vo2_PRTDSI__OUT_SEL1       (* (reg8 *) Vo2__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define Vo2_PRTDSI__SYNC_OUT       (* (reg8 *) Vo2__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(Vo2__SIO_CFG)
    #define Vo2_SIO_HYST_EN        (* (reg8 *) Vo2__SIO_HYST_EN)
    #define Vo2_SIO_REG_HIFREQ     (* (reg8 *) Vo2__SIO_REG_HIFREQ)
    #define Vo2_SIO_CFG            (* (reg8 *) Vo2__SIO_CFG)
    #define Vo2_SIO_DIFF           (* (reg8 *) Vo2__SIO_DIFF)
#endif /* (Vo2__SIO_CFG) */

/* Interrupt Registers */
#if defined(Vo2__INTSTAT)
    #define Vo2_INTSTAT            (* (reg8 *) Vo2__INTSTAT)
    #define Vo2_SNAP               (* (reg8 *) Vo2__SNAP)
    
	#define Vo2_0_INTTYPE_REG 		(* (reg8 *) Vo2__0__INTTYPE)
#endif /* (Vo2__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_Vo2_H */


/* [] END OF FILE */
