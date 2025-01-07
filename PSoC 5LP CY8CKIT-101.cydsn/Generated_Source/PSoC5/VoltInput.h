/*******************************************************************************
* File Name: VoltInput.h  
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

#if !defined(CY_PINS_VoltInput_H) /* Pins VoltInput_H */
#define CY_PINS_VoltInput_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "VoltInput_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 VoltInput__PORT == 15 && ((VoltInput__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    VoltInput_Write(uint8 value);
void    VoltInput_SetDriveMode(uint8 mode);
uint8   VoltInput_ReadDataReg(void);
uint8   VoltInput_Read(void);
void    VoltInput_SetInterruptMode(uint16 position, uint16 mode);
uint8   VoltInput_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the VoltInput_SetDriveMode() function.
     *  @{
     */
        #define VoltInput_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define VoltInput_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define VoltInput_DM_RES_UP          PIN_DM_RES_UP
        #define VoltInput_DM_RES_DWN         PIN_DM_RES_DWN
        #define VoltInput_DM_OD_LO           PIN_DM_OD_LO
        #define VoltInput_DM_OD_HI           PIN_DM_OD_HI
        #define VoltInput_DM_STRONG          PIN_DM_STRONG
        #define VoltInput_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define VoltInput_MASK               VoltInput__MASK
#define VoltInput_SHIFT              VoltInput__SHIFT
#define VoltInput_WIDTH              1u

/* Interrupt constants */
#if defined(VoltInput__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in VoltInput_SetInterruptMode() function.
     *  @{
     */
        #define VoltInput_INTR_NONE      (uint16)(0x0000u)
        #define VoltInput_INTR_RISING    (uint16)(0x0001u)
        #define VoltInput_INTR_FALLING   (uint16)(0x0002u)
        #define VoltInput_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define VoltInput_INTR_MASK      (0x01u) 
#endif /* (VoltInput__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define VoltInput_PS                     (* (reg8 *) VoltInput__PS)
/* Data Register */
#define VoltInput_DR                     (* (reg8 *) VoltInput__DR)
/* Port Number */
#define VoltInput_PRT_NUM                (* (reg8 *) VoltInput__PRT) 
/* Connect to Analog Globals */                                                  
#define VoltInput_AG                     (* (reg8 *) VoltInput__AG)                       
/* Analog MUX bux enable */
#define VoltInput_AMUX                   (* (reg8 *) VoltInput__AMUX) 
/* Bidirectional Enable */                                                        
#define VoltInput_BIE                    (* (reg8 *) VoltInput__BIE)
/* Bit-mask for Aliased Register Access */
#define VoltInput_BIT_MASK               (* (reg8 *) VoltInput__BIT_MASK)
/* Bypass Enable */
#define VoltInput_BYP                    (* (reg8 *) VoltInput__BYP)
/* Port wide control signals */                                                   
#define VoltInput_CTL                    (* (reg8 *) VoltInput__CTL)
/* Drive Modes */
#define VoltInput_DM0                    (* (reg8 *) VoltInput__DM0) 
#define VoltInput_DM1                    (* (reg8 *) VoltInput__DM1)
#define VoltInput_DM2                    (* (reg8 *) VoltInput__DM2) 
/* Input Buffer Disable Override */
#define VoltInput_INP_DIS                (* (reg8 *) VoltInput__INP_DIS)
/* LCD Common or Segment Drive */
#define VoltInput_LCD_COM_SEG            (* (reg8 *) VoltInput__LCD_COM_SEG)
/* Enable Segment LCD */
#define VoltInput_LCD_EN                 (* (reg8 *) VoltInput__LCD_EN)
/* Slew Rate Control */
#define VoltInput_SLW                    (* (reg8 *) VoltInput__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define VoltInput_PRTDSI__CAPS_SEL       (* (reg8 *) VoltInput__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define VoltInput_PRTDSI__DBL_SYNC_IN    (* (reg8 *) VoltInput__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define VoltInput_PRTDSI__OE_SEL0        (* (reg8 *) VoltInput__PRTDSI__OE_SEL0) 
#define VoltInput_PRTDSI__OE_SEL1        (* (reg8 *) VoltInput__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define VoltInput_PRTDSI__OUT_SEL0       (* (reg8 *) VoltInput__PRTDSI__OUT_SEL0) 
#define VoltInput_PRTDSI__OUT_SEL1       (* (reg8 *) VoltInput__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define VoltInput_PRTDSI__SYNC_OUT       (* (reg8 *) VoltInput__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(VoltInput__SIO_CFG)
    #define VoltInput_SIO_HYST_EN        (* (reg8 *) VoltInput__SIO_HYST_EN)
    #define VoltInput_SIO_REG_HIFREQ     (* (reg8 *) VoltInput__SIO_REG_HIFREQ)
    #define VoltInput_SIO_CFG            (* (reg8 *) VoltInput__SIO_CFG)
    #define VoltInput_SIO_DIFF           (* (reg8 *) VoltInput__SIO_DIFF)
#endif /* (VoltInput__SIO_CFG) */

/* Interrupt Registers */
#if defined(VoltInput__INTSTAT)
    #define VoltInput_INTSTAT            (* (reg8 *) VoltInput__INTSTAT)
    #define VoltInput_SNAP               (* (reg8 *) VoltInput__SNAP)
    
	#define VoltInput_0_INTTYPE_REG 		(* (reg8 *) VoltInput__0__INTTYPE)
#endif /* (VoltInput__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_VoltInput_H */


/* [] END OF FILE */
