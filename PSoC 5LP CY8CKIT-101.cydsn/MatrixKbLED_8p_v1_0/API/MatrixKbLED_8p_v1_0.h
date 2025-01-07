/*******************************************************************************
* 文件名: `$INSTANCE_NAME`.h  
* 版  本：1.0
* 日  期：2021-12-24
* 作  者：Inhaul Hsu
*
*    矩阵键盘行数/列数设置、矩阵LED行数/列数设置、API函数声明及其所用宏定义
*    按键消息循环队列大小设置、结构体定义、API函数声明
*    可选生成按键消息
*    可选使用用户自定义的按键行列号至按键符号查找表
*    每轮所用的扫描次数设置
*    移植相关的宏函数定义
*
*******************************************************************************/

#ifndef __`$INSTANCE_NAME`_H_
#define __`$INSTANCE_NAME`_H_

#define MESSAGE_GEN      `$MESSAGE_GEN`    // 生成按键消息

#define NUM_KEYROWS      `$NUM_KEYROWS`    // 此值限于R3~R0行按键区域，取值0~4
#define NUM_KEYCOLS      `$NUM_KEYCOLS`    // 此值限于COL3~COL0列按键区域，取值0~4

#define NUM_LEDROWS      `$NUM_LEDROWS`    // 此值限于R7~R4行LED区域，取值0~4
#define NUM_LEDCOLS      `$NUM_LEDCOLS`    // 此值限于COL3~COL0列LED区域，取值0~4

#define LVLNUM_BARLED   (NUM_LEDCOLS+4)  // LED条指示级别数（LED条灯数+1）
                                         // 指示级: 0(全灭) ~ LVLNUM_BARLED-1(全亮)
    
#define CUST_KEYSYM_TBL  `$CUSTOM_KEYSYM_TABLE`    // 使用用户自定义的按键行列号至按键符号查找表
#define SCANS_PER_ROUND  `$SCANS_PER_ROUND`   // 每轮所用的扫描次数，取值应为6（数值对应NUM_LEDROWS=4时）、
                              // 或10（数值对应NUM_KEYROWS=NUM_LEDROWS=4时）。
                              // 行数设置与所述不同时，该宏值无实际意义，仅作两种不同扫描
                              // 方法的标示。实际的每轮扫描次数为：
                              // --------------------------------------------------------
                              // NUM_LEDROWS\SCANS_PER_ROUND |   6            10
                              // --------------------------------------------------------
                              //     4                       |   6     3+ NUM_KEYROWS +3
                              //     3                       |   5     3+ NUM_KEYROWS +2
                              //     2                       |   4     3+ NUM_KEYROWS +1
                              //     1                       |   3     3+ NUM_KEYROWS
                              //     0                       |   3     3+ NUM_KEYROWS
                              // --------------------------------------------------------

#define QSIZE            16   // 队列大小（至多容纳消息数）


/* ------------------------------------ 【移植相关开始】 ---------------------------------------
                      需根据不同的硬件平台和编译器来定义以下9个宏函数                         */
#define PORTING_TO_CY8CKIT     `$PORTING_TO_CY8CKIT`     // 1——CY8CKIT-050、2——CY8CKIT-062

#if(PORTING_TO_CY8CKIT == 1)         // PSoC 5LP处理器、GCC编译器时
/* 以下头文件为PSoC 5LP处理器、GCC编译器实现宏函数时所需 */
#include "cydevice_trm.h"

#define `$INSTANCE_NAME`_EN_DM_STRONG   6
#define `$INSTANCE_NAME`_EN_DM_DIG_HIZ  1

#define `$INSTANCE_NAME`_RA_DM_STRONG   6
#define `$INSTANCE_NAME`_RA_DM_DIG_HIZ  1

#define `$INSTANCE_NAME`_COL_DM_OD_LO   4
#define `$INSTANCE_NAME`_COL_DM_RES_UP  2
#define `$INSTANCE_NAME`_COL_DM_STRONG  6

/* 设置1位引脚EN的驱动模式，可选参数: `$INSTANCE_NAME`_EN_DM_STRONG（强驱动）
                                      `$INSTANCE_NAME`_EN_DM_DIG_HIZ（高阻数字输入） */
#define EN_SETDRIVEMODE(drvmode)   do{if((drvmode) == `$INSTANCE_NAME`_EN_DM_STRONG){            \
                                        io_reg_bak = *(volatile uint8 *)CYREG_PRT4_PC3;     \
                                        io_reg_bak &= 0xF1;                                 \
                                        io_reg_bak |= 12;                                   \
                                        *(volatile uint8 *)CYREG_PRT4_PC3 = io_reg_bak;}    \
                                      else{io_reg_bak = *(volatile uint8 *)CYREG_PRT4_PC3;  \
                                        io_reg_bak &= 0xF1;                                 \
                                        io_reg_bak |= 2;                                    \
                                        *(volatile uint8 *)CYREG_PRT4_PC3 = io_reg_bak;}}while(0)
#define EN_WRITE(val)              do{io_reg_bak = *(volatile uint8 *)CYREG_PRT4_PC3;       \
                                      io_reg_bak &= 0xFE;                                   \
                                      io_reg_bak |= (val);                                  \
                                      *(volatile uint8 *)CYREG_PRT4_PC3 = io_reg_bak;}while(0)
#define EN_READ()                  ((*(volatile uint8 *)CYREG_PRT4_PC3 & 0x10) >>4)

/* 设置3位引脚RA的驱动模式，可选参数: `$INSTANCE_NAME`_RA_DM_STRONG（强驱动）
                                      `$INSTANCE_NAME`_RA_DM_DIG_HIZ（高阻数字输入） */
#define RA_SETDRIVEMODE(drvmode)   do{if((drvmode) == `$INSTANCE_NAME`_RA_DM_STRONG){            \
                                        *(volatile uint8 *)CYREG_PRT4_DM2 |= 0x07;          \
                                        *(volatile uint8 *)CYREG_PRT4_DM1 |= 0x07;          \
                                        *(volatile uint8 *)CYREG_PRT4_DM0 &= 0xF8;}         \
                                      else{*(volatile uint8 *)CYREG_PRT4_DM2 &= 0xF8;       \
                                        *(volatile uint8 *)CYREG_PRT4_DM1 &= 0xF8;          \
                                        *(volatile uint8 *)CYREG_PRT4_DM0 |= 0x07;}}while(0)
#define RA_WRITE(val)              do{io_reg_bak = *(volatile uint8 *)CYREG_PRT4_DR;        \
                                      io_reg_bak &= 0xF8;                                   \
                                      io_reg_bak |= (val)& 0x07;                            \
                                      *(volatile uint8 *)CYREG_PRT4_DR = io_reg_bak;}while(0)
#define RA_READ()                  (*(volatile uint8 *)CYREG_PRT4_PS & 0x07)

/* 设置1~4位引脚COL的驱动模式，可选参数: `$INSTANCE_NAME`_COL_DM_OD_LO（开漏无上拉、低驱动）
                                         `$INSTANCE_NAME`_COL_DM_RES_UP（开漏有上拉、低驱动）
                                         `$INSTANCE_NAME`_COL_DM_STRONG（强驱动）（SCANS_PER_ROUND=10时无需此模式）*/
#define COL_SETDRIVEMODE(drvmode)  do{if((drvmode) == `$INSTANCE_NAME`_COL_DM_OD_LO){            \
                                        *(volatile uint8 *)CYREG_PRT12_DM2 |= 0x0F;         \
                                        *(volatile uint8 *)CYREG_PRT12_DM1 &= 0xF0;         \
                                        *(volatile uint8 *)CYREG_PRT12_DM0 &= 0xF0;}        \
                                      else if((drvmode) == `$INSTANCE_NAME`_COL_DM_RES_UP){      \
                                        *(volatile uint8 *)CYREG_PRT12_DM2 &= 0xF0;         \
                                        *(volatile uint8 *)CYREG_PRT12_DM1 |= 0x0F;         \
                                        *(volatile uint8 *)CYREG_PRT12_DM0 &= 0xF0;}        \
                                      else{*(volatile uint8 *)CYREG_PRT12_DM2 |= 0x0F;      \
                                        *(volatile uint8 *)CYREG_PRT12_DM1 |= 0x0F;         \
                                        *(volatile uint8 *)CYREG_PRT12_DM0 &= 0xF0;}}while(0)
#define COL_WRITE(val)             do{io_reg_bak = *(volatile uint8 *)CYREG_PRT12_DR;       \
                                      io_reg_bak &= 0xF0;                                   \
                                      io_reg_bak += (val)& 0x0F;                            \
                                      *(volatile uint8 *)CYREG_PRT12_DR = io_reg_bak;}while(0)
#define COL_READ()                 (*(volatile uint8 *)CYREG_PRT12_PS & 0x0F)

#elif(PORTING_TO_CY8CKIT == 2)       // PSoC 6处理器、GCC编译器时
/* 以下头文件为PSoC 6处理器、GCC编译器实现宏函数时所需 */
#include "cy_gpio.h"
#include "cydevice_trm.h"

#define `$INSTANCE_NAME`_EN_DM_STRONG   CY_GPIO_DM_STRONG
#define `$INSTANCE_NAME`_EN_DM_DIG_HIZ  CY_GPIO_DM_HIGHZ

#define `$INSTANCE_NAME`_RA_DM_STRONG   CY_GPIO_DM_STRONG
#define `$INSTANCE_NAME`_RA_DM_DIG_HIZ  CY_GPIO_DM_HIGHZ

#define `$INSTANCE_NAME`_COL_DM_OD_LO   CY_GPIO_DM_OD_DRIVESLOW
#define `$INSTANCE_NAME`_COL_DM_RES_UP  CY_GPIO_DM_PULLUP
#define `$INSTANCE_NAME`_COL_DM_STRONG  CY_GPIO_DM_STRONG

/* 设置1位引脚EN的驱动模式，可选参数: `$INSTANCE_NAME`_EN_DM_STRONG（强驱动）
                                      `$INSTANCE_NAME`_EN_DM_DIG_HIZ（高阻数字输入） */
#define EN_SETDRIVEMODE(drvmode)   do{if((drvmode) == `$INSTANCE_NAME`_EN_DM_STRONG){                   \
                                        io_reg_bak = *(volatile uint32_t *)CYREG_GPIO_PRT1_CFG;    \
                                        io_reg_bak &= 0xFFEFFFFFul;                                \
                                        io_reg_bak |= 0x00E00000ul;                                \
                                        *(volatile uint32_t *)CYREG_GPIO_PRT1_CFG = io_reg_bak;}   \
                                      else{io_reg_bak = *(volatile uint32_t *)CYREG_GPIO_PRT1_CFG; \
                                        io_reg_bak &= 0xFF8FFFFFul;                                \
                                        io_reg_bak |= 0x00800000ul;                                \
                                        *(volatile uint32_t *)CYREG_GPIO_PRT1_CFG = io_reg_bak;}}while(0)
#define EN_WRITE(val)              do{io_reg_bak = *(volatile uint32_t *)CYREG_GPIO_PRT1_OUT;      \
                                      io_reg_bak &= 0xFFFFFFDFul;                                  \
                                      io_reg_bak |= (val)<<5;                                      \
                                      *(volatile uint32_t *)CYREG_GPIO_PRT1_OUT = io_reg_bak;}while(0)
#define EN_READ()                  ((*(volatile uint32_t *)CYREG_GPIO_PRT1_IN & 0x00000020ul) >>5)
    
/* 设置3位引脚RA的驱动模式，可选参数: `$INSTANCE_NAME`_RA_DM_STRONG（强驱动）
                                      `$INSTANCE_NAME`_RA_DM_DIG_HIZ（高阻数字输入） */
#define RA_SETDRIVEMODE(drvmode)   do{if((drvmode) == `$INSTANCE_NAME`_RA_DM_STRONG){                    \
                                        io_reg_bak = *(volatile uint32_t *)CYREG_GPIO_PRT12_CFG;    \
                                        io_reg_bak &= 0xEEFEFFFFul;                                 \
                                        io_reg_bak |= 0xEE0E0000ul;                                 \
                                        *(volatile uint32_t *)CYREG_GPIO_PRT12_CFG = io_reg_bak;}   \
                                      else{io_reg_bak = *(volatile uint32_t *)CYREG_GPIO_PRT12_CFG; \
                                        io_reg_bak &= 0x88F8FFFFul;                                 \
                                        io_reg_bak |= 0x88080000ul;                                 \
                                        *(volatile uint32_t *)CYREG_GPIO_PRT12_CFG = io_reg_bak;}}while(0)
#define RA_WRITE(val)              do{io_reg_bak = *(volatile uint32_t *)CYREG_GPIO_PRT12_OUT;      \
                                      io_reg_bak &= 0xFFFFFF2Ful;                                   \
                                      io_reg_bak |= ((val)& 0x03ul)<<6;                             \
                                      io_reg_bak |= ((val)& 0x04ul)<<2;                             \
                                      *(volatile uint32_t *)CYREG_GPIO_PRT12_OUT = io_reg_bak;}while(0)
#define RA_READ()                  (io_reg_bak = *(volatile uint32_t *)CYREG_GPIO_PRT12_IN,  \
                                   io_reg_bak >>= 1,                                         \
                                   io_reg_bak |= (io_reg_bak & 0x08ul) << 4,                 \
                                   io_reg_bak >>= 5,                                         \
                                   io_reg_bak &= 0x07ul)

/* 设置1~4位引脚COL的驱动模式，可选参数: `$INSTANCE_NAME`_COL_DM_OD_LO（开漏无上拉、低驱动）
                                         `$INSTANCE_NAME`_COL_DM_RES_UP（开漏有上拉、低驱动）
                                         `$INSTANCE_NAME`_COL_DM_STRONG（强驱动）（SCANS_PER_ROUND=10时无需此模式）*/
#define COL_SETDRIVEMODE(drvmode)  do{if((drvmode) == `$INSTANCE_NAME`_COL_DM_OD_LO){                   \
                                        io_reg_bak = *(volatile uint32_t *)CYREG_GPIO_PRT1_CFG;    \
                                        io_reg_bak &= 0xFFFCCCFFul;                                \
                                        io_reg_bak |= 0x000CCC00ul;                                \
                                        *(volatile uint32_t *)CYREG_GPIO_PRT1_CFG = io_reg_bak;    \
                                        io_reg_bak = *(volatile uint32_t *)CYREG_GPIO_PRT12_CFG;   \
                                        io_reg_bak &= 0xFFCFFFFFul;                                \
                                        io_reg_bak |= 0x00C00000ul;                                \
                                        *(volatile uint32_t *)CYREG_GPIO_PRT12_CFG = io_reg_bak;}  \
                                      else if((drvmode) == `$INSTANCE_NAME`_COL_DM_RES_UP){             \
                                        io_reg_bak = *(volatile uint32_t *)CYREG_GPIO_PRT1_CFG;    \
                                        io_reg_bak &= 0xFFFAAAFFul;                                \
                                        io_reg_bak |= 0x000AAA00ul;                                \
                                        *(volatile uint32_t *)CYREG_GPIO_PRT1_CFG = io_reg_bak;    \
                                        io_reg_bak = *(volatile uint32_t *)CYREG_GPIO_PRT12_CFG;   \
                                        io_reg_bak &= 0xFFAFFFFFul;                                \
                                        io_reg_bak |= 0x00A00000ul;                                \
                                        *(volatile uint32_t *)CYREG_GPIO_PRT12_CFG = io_reg_bak;}  \
/*                                      else{io_reg_bak = *(volatile uint32_t *)CYREG_GPIO_PRT1_CFG; \
                                        io_reg_bak &= 0xFFFEEEFFul;                                \
                                        io_reg_bak |= 0x000EEE00ul;                                \
                                        *(volatile uint32_t *)CYREG_GPIO_PRT1_CFG = io_reg_bak;    \
                                        io_reg_bak = *(volatile uint32_t *)CYREG_GPIO_PRT12_CFG;   \
                                        io_reg_bak &= 0xFFEFFFFFul;                                \
                                        io_reg_bak |= 0x00E00000ul;                                \
                                        *(volatile uint32_t *)CYREG_GPIO_PRT12_CFG = io_reg_bak;}}while(0)
*/                                      else{io_reg_bak = *(volatile uint32_t *)CYREG_GPIO_PRT1_CFG; \
                                        io_reg_bak &= 0xFFFAAAFFul;                                \
                                        io_reg_bak |= 0x000AAA00ul;                                \
                                        *(volatile uint32_t *)CYREG_GPIO_PRT1_CFG = io_reg_bak;    \
                                        io_reg_bak = *(volatile uint32_t *)CYREG_GPIO_PRT12_CFG;   \
                                        io_reg_bak &= 0xFFAFFFFFul;                                \
                                        io_reg_bak |= 0x00A00000ul;                                \
                                        *(volatile uint32_t *)CYREG_GPIO_PRT12_CFG = io_reg_bak;}}while(0)
#define COL_WRITE(val)             do{io_reg_bak = *(volatile uint32_t *)CYREG_GPIO_PRT1_OUT;      \
                                      io_reg_bak &= 0xFFFFFFE3ul;                                  \
                                      io_reg_bak |= ((val)& 0x07ul)<<2;                            \
                                      *(volatile uint32_t *)CYREG_GPIO_PRT1_OUT = io_reg_bak;      \
                                      io_reg_bak = *(volatile uint32_t *)CYREG_GPIO_PRT12_OUT;     \
                                      io_reg_bak &= 0xFFFFFFDFul;                                  \
                                      io_reg_bak |= ((val)& 0x08ul)<<2;                            \
                                      *(volatile uint32_t *)CYREG_GPIO_PRT12_OUT = io_reg_bak;}while(0)
#define COL_READ()                 (io_reg_bak = *(volatile uint32_t *)CYREG_GPIO_PRT1_IN,                 \
                                   io_reg_bak >>= 2,                                                       \
                                   io_reg_bak &= 0x07ul,                                                   \
                                   io_reg_bak |= (*(volatile uint32_t *)CYREG_GPIO_PRT12_IN & 0x20ul) >>2, \
                                   io_reg_bak &= 0x0Ful)

#else                                // 其它平台时
/*******************************************************************************
*  Place your includes, defines
********************************************************************************/
/* `#START `$INSTANCE_NAME`_Porting` */

/* `#END` */
#endif
/*
--------------------------------------- 【移植相关结束】 -------------------------------------*/


/* 矩阵按键/LED API函数所用宏 */
#define LED_D0_S0B        0x00000001
#define LED_D1_S1B        0x00000002
#define LED_D2_S2B        0x00000004
#define LED_D3_S3B        0x00000008
#define LED_D4_S4B        0x00000010
#define LED_D5_S5B        0x00000020
#define LED_D6_S6B        0x00000040
#define LED_D7_S7B        0x00000080
#define LED_D8_S8B        0x00000100
#define LED_D9_S9B        0x00000200
#define LED_D10_S10B      0x00000400
#define LED_D11_S11B      0x00000800
#define LED_D12B_S12B     0x00001000
#define LED_D12G_S13B     0x00002000
#define LED_D12R_S14B     0x00004000
#define LED_D13_S15B      0x00008000
#define LED_D16           0x00010000
#define LED_D17           0x00020000
#define LED_D18           0x00040000
#define LED_D19           0x00080000
#define LED_D20           0x00100000
#define LED_D21           0x00200000
#define LED_D22           0x00400000
#define LED_ALL           0x007FFFFF
#define MAXLEDNUM         23

#define LEDON             0
#define LEDOFF            1

#define S0                0x00000001
#define S1                0x00000002
#define S2                0x00000004
#define S3                0x00000008
#define S4                0x00000010
#define S5                0x00000020
#define S6                0x00000040
#define S7                0x00000080
#define S8                0x00000100
#define S9                0x00000200
#define S10               0x00000400
#define S11               0x00000800
#define S12               0x00001000
#define S13               0x00002000
#define S14               0x00004000
#define S15               0x00008000
#define S16_RIGHT         0x00010000
#define S16_UP            0x00020000
#define S16_LEFT          0x00040000
#define S17               0x00080000
#define S16_CHA           0x00100000
#define S16_CHB           0x00200000
#define S16_CENTER        0x00400000
#define S16_DOWN          0x00800000
#define SW_ALL            0x00FFFFFF
#define MAXSWNUM          24
#define ROTARY_ACTION_CW  0x10000000
#define ROTARY_ACTION_CCW 0x20000000

#define KEYON             0
#define KEYOFF            1


#if(MESSAGE_GEN == 1)
/* 队列结构体类型定义 */
typedef struct _QUEUE
{
    uint8 *pBase;
    int8 front;
    int8 rear;
    int8 maxsize;
}  QUEUE;


/* 按键消息循环队列API函数声明 */
uint8 Qkey_FetchData(void);
uint8 Qkey_FeedData(uint8 bytemsg);
#endif

/* 矩阵键盘/LED API函数声明 */
void   `$INSTANCE_NAME`_Start(void);
void   `$INSTANCE_NAME`_SetLED(uint32 leds, uint8 onoff);
void   `$INSTANCE_NAME`_SetLED_RC(uint8 row, uint8 col, uint8 onoff);
void   `$INSTANCE_NAME`_SetBarLEDLevel(uint8 level);
uint8  `$INSTANCE_NAME`_GetLED(uint32 led);
uint8  `$INSTANCE_NAME`_GetLED_RC(uint8 row, uint8 col);
uint8  `$INSTANCE_NAME`_KeySym2RC(uint8 keysym);
uint32 `$INSTANCE_NAME`_GetKey(uint32 key);
uint8  `$INSTANCE_NAME`_GetKey_RC(uint8 row, uint8 col);
#if(MESSAGE_GEN == 1)
void   `$INSTANCE_NAME`_SimKeyAction(uint8 keysym, uint8 onoff);
#endif

#endif

/* [] END OF FILE */
