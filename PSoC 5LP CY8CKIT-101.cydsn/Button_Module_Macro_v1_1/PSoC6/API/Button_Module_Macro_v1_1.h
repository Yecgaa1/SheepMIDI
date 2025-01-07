/*******************************************************************************
* 文件名: `$INSTANCE_NAME`.h  
* 版  本：1.1
* 日  期：2024-11-9
* 作  者：Inhaul Hsu
********************************************************************************
  按键模块配置、API函数声明
  按键消息循环队列大小设置、API函数声明

 【按键消息格式】：         动作类型(b7..b6)             , 按键编号(b5..b0)
                  ---------------------------------------------------------
                  BUTTON_ACTION_TYPE_SGLCLICK     ——单击        0..31
                  BUTTON_ACTION_TYPE_LONG         ——长按  (编号63预留不可用)
                  BUTTON_ACTION_TYPE_DBLCLICK     ——双击
                  BUTTON_ACTION_TYPE_RELEASINGUP  ——松开
                  BUTTON_ACTION_TYPE_PUSHINGDOWN  ——按下
   
   注：BUTTON_ACTION_TYPE_PUSHINGDOWN、BUTTON_ACTION_TYPE_SGLCLICK为相同值，乃基
   于以下原因：
     - 将按键动作划分为两类：基本动作 —— 按下（时刻）、松开（时刻）
                             扩展动作 —— 长按（及配套的长按结束时的松开动作）、双击
     - 当某按键被配置为仅需检测基本动作时，其按键消息中的动作类型仅会包括
       BUTTON_ACTION_TYPE_PUSHINGDOWN 和 BUTTON_ACTION_TYPE_RELEASINGUP
     - 当某按键被配置为需检测扩展动作时，其按键消息中的动作类型会包括
       BUTTON_ACTION_TYPE_SGLCLICK、BUTTON_ACTION_TYPE_LONG、BUTTON_ACTION_TYPE_DBLCLICK
       和 BUTTON_ACTION_TYPE_RELEASINGUP(仅标示长按结束时的松开，而无关单击、双击时的松开)

 【注】：
  1. 对外API函数为：
       `$INSTANCE_NAME`_Start()、`$INSTANCE_NAME`_GetPBStatus()
       `$INSTANCE_NAME`_SetEquivSglclickGen()、`$INSTANCE_NAME`_GetEquivSglclickGen()
       `$INSTANCE_NAME`_GetMessage()、`$INSTANCE_NAME`_SendMessage()
       应用程序收发消息部分的代码需兼容无OS和FreeRTOS时，可使用：
       _`$INSTANCE_NAME`_GetMessage()、_`$INSTANCE_NAME`_SendMessage()
  2. 支持两种按键单击消息生成方式：
     - 快速（`$INSTANCE_NAME`.h中的宏SGL_CLICK_MSG_GEN_METHOD值为0）：单击按下时刻
       生成消息（若需检测双击，则双击过程中会先发送一条单击消息）
     - 延迟（宏SGL_CLICK_MSG_GEN_METHOD值为1）：单击按下后0.4秒再生成消息（双击过
       程中仅会发送双击消息，长按过程中的首次单击消息会延迟0.4s发送）
     延迟方式仅对于需检测双击动作的按键有效，若按键不需检测双击动作，则实际实现为
     快速方式。
  3. 各按键可被独立配置为仅需检测基本动作，以及需检测长按 和/或 双击。
  4. 可全局和单独使能各按键于其长按期间生成等效的连续单击消息
  5. 驱动层内部支持按键分页(分组)处理，适用于按键数量较多和降低每次的平均处理时间。
  6. 移植本按键模块到不同软硬件平台上时，仅需考虑本文件和`$INSTANCE_NAME`.c中带!!P标记
     的注释处的内容指引。
  7. 应用程序可配置的功能和参数位于本文件中带!A标记的注释处。
********************************************************************************
*/
#ifndef __`$INSTANCE_NAME`_H_
#define __`$INSTANCE_NAME`_H_

#if defined(__cplusplus)
  extern "C" {
#endif

#define _BUTTON_MODULE_VER_       110ul /* 版本v1.1.0 */

/* OS宏名及其对应编号 */
#define NON_OS                    0
#define FREERTOS                  1
#define OTHER_OS                  2

/*!!P 选择按键模块是否使用某操作系统的消息队列服务，NON_OS时使用自定义的按键消息队列，将根据所选生成代码。
      - `$INSTANCE_NAME`.c中：
        按键消息循环队列所需变量、宏的定义
        发送消息的内联版或OS版宏函数定义（`$INSTANCE_NAME`_Handler()函数中使用）
        `$INSTANCE_NAME`_Start()中的按键消息循环队列初始化
        自定义的按键消息循环队列的消息收发函数定义和实现（仅NON_OS时）
      - `$INSTANCE_NAME`.h中：
        `$INSTANCE_NAME`_Start()移植实现所需的OS队列头文件包含（仅FreeRTOS时）
        收发消息的宏函数定义、API函数声明（仅NON_OS时）
        收发消息宏函数定义（仅FreeRTOS时）
      OTHER_OS时这些代码需用户添加
*/
/* 0==宏NON_OS, 1==宏FREERTOS, 2==宏OTHER_OS */
#define PORTING_TO_OS_Q           `$PORTING_TO_OS_Q`

/*!!P 定义uint8、uint16、uint32基本数据类型 */
#include <cy_syslib.h>

/*!!P 设置按键总数，v1版只支持1~32 */
#define NUM_BUTTONS               `$NUM_BUTTONS`u

#if (_BUTTON_MODULE_VER_ < 200ul) && (NUM_BUTTONS > 32)
  #error The total number of buttons supported by version 1.1 cannot exceed 32 !
#endif /* NUM_BUTTONS */

/* 选择按键是否分页，以及每页按键数（!!P 应根据按键的硬件电路、按键数量和CPU性能合理选择）：
    -1 不分页，2 每页4个按键，3 每页8个按键，4 每页16个按键，5 每页32个按键
   注：按键分页时，此值为NUM_BUTTONS_PER_BANK“每页按键数”值的2的幂次
*/
#define NUMPOW_BUTTONS_PER_BANK   `$NUM_BUTTONS_PER_BANK`

#if NUMPOW_BUTTONS_PER_BANK >= 0
  /* 每页按键数 */
  #define NUM_BUTTONS_PER_BANK    (1u << NUMPOW_BUTTONS_PER_BANK)
  /* 按键页数 */
  #define NUM_BUTTON_BANKS        ((NUM_BUTTONS + NUM_BUTTONS_PER_BANK -1)/NUM_BUTTONS_PER_BANK)
#endif /* NUMPOW_BUTTONS_PER_BANK */

/* 按键消息队列大小（最多容纳元素数，实际可容纳-1条消息）*/
#define NUM_QELEMS                8u

/*!A 选择按键单击消息生成方式：
     0——快速，单击按下时刻生成消息（若需检测双击，则双击过程中会先发送一条单击消息）
     1——延迟，单击按下后0.4秒再生成消息（双击过程中仅发送双击消息，长按过程中的首次单击消息会延迟0.4s发送）
  注：延迟方式仅对于需检测双击动作的按键有效，若按键不需检测双击动作，则实际实现为快速方式。
*/
#define SGL_CLICK_MSG_GEN_METHOD  `$SGL_CLICK_MSG_GEN_METHOD`

/*!A 按键长按动作检测配置字（每位对应一个按键(b0位对应0号按键)，位值1表示需检测长按）*/
#define LONG_PRESS_DET_CONFIG     `$LONG_PRESS_DET_CONFIG`ul
/*!A 按键双击动作检测配置字（每位对应一个按键(b0位对应0号按键)，位值1表示需检测双击）*/
#define DBL_CLICK_DET_CONFIG      `$DBL_CLICK_DET_CONFIG`ul
/*【注意】：请将所有需检测扩展动作（长按和/或双击）的按键连续编号，且从0号开始编起 */

/* 以下直至NUM_BUTTONS_EX_ACT宏定义结束，为确定需检测扩展动作（长按和/或双击）的按键数 */
#if LONG_PRESS_DET_CONFIG == 0
  #define NUM_BUTTONS_LONG_PRESS  0
#elif (LONG_PRESS_DET_CONFIG >> 1) == 0
  #define NUM_BUTTONS_LONG_PRESS  1
#elif (LONG_PRESS_DET_CONFIG >> 2) == 0
  #define NUM_BUTTONS_LONG_PRESS  2
#elif (LONG_PRESS_DET_CONFIG >> 3) == 0
  #define NUM_BUTTONS_LONG_PRESS  3
#elif (LONG_PRESS_DET_CONFIG >> 4) == 0
  #define NUM_BUTTONS_LONG_PRESS  4
#elif (LONG_PRESS_DET_CONFIG >> 5) == 0
  #define NUM_BUTTONS_LONG_PRESS  5
#elif (LONG_PRESS_DET_CONFIG >> 6) == 0
  #define NUM_BUTTONS_LONG_PRESS  6
#elif (LONG_PRESS_DET_CONFIG >> 7) == 0
  #define NUM_BUTTONS_LONG_PRESS  7
#elif (LONG_PRESS_DET_CONFIG >> 8) == 0
  #define NUM_BUTTONS_LONG_PRESS  8
#elif (LONG_PRESS_DET_CONFIG >> 9) == 0
  #define NUM_BUTTONS_LONG_PRESS  9
#elif (LONG_PRESS_DET_CONFIG >> 10) == 0
  #define NUM_BUTTONS_LONG_PRESS  10
#elif (LONG_PRESS_DET_CONFIG >> 11) == 0
  #define NUM_BUTTONS_LONG_PRESS  11
#elif (LONG_PRESS_DET_CONFIG >> 12) == 0
  #define NUM_BUTTONS_LONG_PRESS  12
#elif (LONG_PRESS_DET_CONFIG >> 13) == 0
  #define NUM_BUTTONS_LONG_PRESS  13
#elif (LONG_PRESS_DET_CONFIG >> 14) == 0
  #define NUM_BUTTONS_LONG_PRESS  14
#elif (LONG_PRESS_DET_CONFIG >> 15) == 0
  #define NUM_BUTTONS_LONG_PRESS  15
#elif (LONG_PRESS_DET_CONFIG >> 16) == 0
  #define NUM_BUTTONS_LONG_PRESS  16
#elif (LONG_PRESS_DET_CONFIG >> 17) == 0
  #define NUM_BUTTONS_LONG_PRESS  17
#elif (LONG_PRESS_DET_CONFIG >> 18) == 0
  #define NUM_BUTTONS_LONG_PRESS  18
#elif (LONG_PRESS_DET_CONFIG >> 19) == 0
  #define NUM_BUTTONS_LONG_PRESS  19
#elif (LONG_PRESS_DET_CONFIG >> 20) == 0
  #define NUM_BUTTONS_LONG_PRESS  20
#elif (LONG_PRESS_DET_CONFIG >> 21) == 0
  #define NUM_BUTTONS_LONG_PRESS  21
#elif (LONG_PRESS_DET_CONFIG >> 22) == 0
  #define NUM_BUTTONS_LONG_PRESS  22
#elif (LONG_PRESS_DET_CONFIG >> 23) == 0
  #define NUM_BUTTONS_LONG_PRESS  23
#elif (LONG_PRESS_DET_CONFIG >> 24) == 0
  #define NUM_BUTTONS_LONG_PRESS  24
#elif (LONG_PRESS_DET_CONFIG >> 25) == 0
  #define NUM_BUTTONS_LONG_PRESS  25
#elif (LONG_PRESS_DET_CONFIG >> 26) == 0
  #define NUM_BUTTONS_LONG_PRESS  26
#elif (LONG_PRESS_DET_CONFIG >> 27) == 0
  #define NUM_BUTTONS_LONG_PRESS  27
#elif (LONG_PRESS_DET_CONFIG >> 28) == 0
  #define NUM_BUTTONS_LONG_PRESS  28
#elif (LONG_PRESS_DET_CONFIG >> 29) == 0
  #define NUM_BUTTONS_LONG_PRESS  29
#elif (LONG_PRESS_DET_CONFIG >> 30) == 0
  #define NUM_BUTTONS_LONG_PRESS  30
#elif (LONG_PRESS_DET_CONFIG >> 31) == 0
  #define NUM_BUTTONS_LONG_PRESS  31
#else
  #define NUM_BUTTONS_LONG_PRESS  32
#endif /* LONG_PRESS_DET_CONFIG */

#if DBL_CLICK_DET_CONFIG == 0
  #define NUM_BUTTONS_DBL_CLICK   0
#elif (DBL_CLICK_DET_CONFIG >> 1) == 0
  #define NUM_BUTTONS_DBL_CLICK   1
#elif (DBL_CLICK_DET_CONFIG >> 2) == 0
  #define NUM_BUTTONS_DBL_CLICK   2
#elif (DBL_CLICK_DET_CONFIG >> 3) == 0
  #define NUM_BUTTONS_DBL_CLICK   3
#elif (DBL_CLICK_DET_CONFIG >> 4) == 0
  #define NUM_BUTTONS_DBL_CLICK   4
#elif (DBL_CLICK_DET_CONFIG >> 5) == 0
  #define NUM_BUTTONS_DBL_CLICK   5
#elif (DBL_CLICK_DET_CONFIG >> 6) == 0
  #define NUM_BUTTONS_DBL_CLICK   6
#elif (DBL_CLICK_DET_CONFIG >> 7) == 0
  #define NUM_BUTTONS_DBL_CLICK   7
#elif (DBL_CLICK_DET_CONFIG >> 8) == 0
  #define NUM_BUTTONS_DBL_CLICK   8
#elif (DBL_CLICK_DET_CONFIG >> 9) == 0
  #define NUM_BUTTONS_DBL_CLICK   9
#elif (DBL_CLICK_DET_CONFIG >> 10) == 0
  #define NUM_BUTTONS_DBL_CLICK   10
#elif (DBL_CLICK_DET_CONFIG >> 11) == 0
  #define NUM_BUTTONS_DBL_CLICK   11
#elif (DBL_CLICK_DET_CONFIG >> 12) == 0
  #define NUM_BUTTONS_DBL_CLICK   12
#elif (DBL_CLICK_DET_CONFIG >> 13) == 0
  #define NUM_BUTTONS_DBL_CLICK   13
#elif (DBL_CLICK_DET_CONFIG >> 14) == 0
  #define NUM_BUTTONS_DBL_CLICK   14
#elif (DBL_CLICK_DET_CONFIG >> 15) == 0
  #define NUM_BUTTONS_DBL_CLICK   15
#elif (DBL_CLICK_DET_CONFIG >> 16) == 0
  #define NUM_BUTTONS_DBL_CLICK   16
#elif (DBL_CLICK_DET_CONFIG >> 17) == 0
  #define NUM_BUTTONS_DBL_CLICK   17
#elif (DBL_CLICK_DET_CONFIG >> 18) == 0
  #define NUM_BUTTONS_DBL_CLICK   18
#elif (DBL_CLICK_DET_CONFIG >> 19) == 0
  #define NUM_BUTTONS_DBL_CLICK   19
#elif (DBL_CLICK_DET_CONFIG >> 20) == 0
  #define NUM_BUTTONS_DBL_CLICK   20
#elif (DBL_CLICK_DET_CONFIG >> 21) == 0
  #define NUM_BUTTONS_DBL_CLICK   21
#elif (DBL_CLICK_DET_CONFIG >> 22) == 0
  #define NUM_BUTTONS_DBL_CLICK   22
#elif (DBL_CLICK_DET_CONFIG >> 23) == 0
  #define NUM_BUTTONS_DBL_CLICK   23
#elif (DBL_CLICK_DET_CONFIG >> 24) == 0
  #define NUM_BUTTONS_DBL_CLICK   24
#elif (DBL_CLICK_DET_CONFIG >> 25) == 0
  #define NUM_BUTTONS_DBL_CLICK   25
#elif (DBL_CLICK_DET_CONFIG >> 26) == 0
  #define NUM_BUTTONS_DBL_CLICK   26
#elif (DBL_CLICK_DET_CONFIG >> 27) == 0
  #define NUM_BUTTONS_DBL_CLICK   27
#elif (DBL_CLICK_DET_CONFIG >> 28) == 0
  #define NUM_BUTTONS_DBL_CLICK   28
#elif (DBL_CLICK_DET_CONFIG >> 29) == 0
  #define NUM_BUTTONS_DBL_CLICK   29
#elif (DBL_CLICK_DET_CONFIG >> 30) == 0
  #define NUM_BUTTONS_DBL_CLICK   30
#elif (DBL_CLICK_DET_CONFIG >> 31) == 0
  #define NUM_BUTTONS_DBL_CLICK   31
#else
  #define NUM_BUTTONS_DBL_CLICK   32
#endif /* DBL_CLICK_DET_CONFIG */

#if NUM_BUTTONS_LONG_PRESS > NUM_BUTTONS_DBL_CLICK
  #if NUM_BUTTONS_LONG_PRESS <= NUM_BUTTONS
    #define NUM_BUTTONS_EX_ACT    NUM_BUTTONS_LONG_PRESS
  #else
    #define NUM_BUTTONS_EX_ACT    NUM_BUTTONS
  #endif
#else
  #if NUM_BUTTONS_DBL_CLICK <= NUM_BUTTONS
    #define NUM_BUTTONS_EX_ACT    NUM_BUTTONS_DBL_CLICK
  #else
    #define NUM_BUTTONS_EX_ACT    NUM_BUTTONS
  #endif
#endif /* NUM_BUTTONS_LONG_PRESS, NUM_BUTTONS_DBL_CLICK */

/*!A 设置是否检测双击后未松开期间的长按动作 */
#define LONG_PRESS_FR_DBLCLICK_SUPPORT  `$LONG_PRESS_FR_DBLCLICK_SUPPORT`

/*!A 设置是否在长按期间，生成等效的连续单击消息（每位对应一个按键(b0位对应0号按键)，位值1表示需生成）*/
#define EQUIV_SGLCLICK_GEN_CONFIG     `$EQUIV_SGLCLICK_GEN_CONFIG`ul

/*!A 按键开合状态采样周期（单位为ms）
     当按键不分页时，此值应取为20～50（更宽泛值10～90）
     当全部按键分为NUM_BUTTON_BANKS个页处理时，此值应取为<=90/NUM_BUTTON_BANKS
*/
#define BUTTON_SAMPLE_PERIOD_MS       20u

/*!A 用于判定按键双击的连续两次单击时间间隔阈值（单位为ms）*/
#define BUTTON_DBLCLICK_INTERVAL_MS   400ul

/*!A 用于判定按键长按的按下保持时间阈值（单位为ms）*/
#define BUTTON_LONGPRESS_HOLDTIME_MS  2000ul

/*!A 用于长按期间，生成等效的连续单击消息的时间间隔（单位为ms）。宏值为0时全局禁止此功能 */
#define BUTTON_EQUIV_SGLCLICK_INTERVAL_MS  `$EQUIV_SGLCLICK_INTERVAL_MS`ul
#define EQUIV_SGLCLICK_INTERVAL_MS    BUTTON_EQUIV_SGLCLICK_INTERVAL_MS

#if NUMPOW_BUTTONS_PER_BANK >= 0  /* 按键分页时 */
  #warning Please confirm that the macro 'BUTTON_SAMPLE_PERIOD_MS' has been set correctly
#endif
/*【注意】：按键分页时，所有按键采样一轮的时间间隔（即每按键的开合状态采样周期）为：
            BUTTON_SAMPLE_PERIOD_MS * NUM_BUTTON_BANKS，此值应取为<=90（单位为ms）
            其中NUM_BUTTON_BANKS为按键页数（= 按键总数/每页按键数，向上取整）
   如果此值>90，请于下方注释#START ～ #END之间添加：
   #undef BUTTON_SAMPLE_PERIOD_MS  再重新定义符合要求的BUTTON_SAMPLE_PERIOD_MS

   如需更改BUTTON_DBLCLICK_INTERVAL_MS、BUTTON_LONGPRESS_HOLDTIME_MS、
   BUTTON_EQUIV_SGLCLICK_INTERVAL_MS宏值，也请于下方注释#START ～ #END之间添加代码。
*/
/***************************************************************************
 *  Place your BUTTON_XXX_MS redefinitions
 **************************************************************************/
/* `#START `$INSTANCE_NAME`_BUTTON_XXX_MS_Redefs` */

/* `#END` */

/* BUTTON_DBLCLICK_INTERVAL_MS的驱动程序内部使用版（单位为采样次数）*/
#if NUMPOW_BUTTONS_PER_BANK < 0
  #define BUTTON_DBLCLICK_INTERVAL        (BUTTON_DBLCLICK_INTERVAL_MS/BUTTON_SAMPLE_PERIOD_MS)
#else
  #define BUTTON_DBLCLICK_INTERVAL        ((BUTTON_DBLCLICK_INTERVAL_MS/NUM_BUTTON_BANKS) \
                                            /BUTTON_SAMPLE_PERIOD_MS)
#endif /* NUMPOW_BUTTONS_PER_BANK */

/* 按键分页时，针对需检测双击且单击消息生成方式为“延迟”情形，调整BUTTON_DBLCLICK_INTERVAL宏值 */
#if NUMPOW_BUTTONS_PER_BANK >= 0
  #define BUTTON_DBLCLICK_INTERVAL_BANK_RND  (BUTTON_DBLCLICK_INTERVAL_MS/(NUM_BUTTON_BANKS*BUTTON_SAMPLE_PERIOD_MS))
  #undef BUTTON_DBLCLICK_INTERVAL
  #if BUTTON_DBLCLICK_INTERVAL_BANK_RND *(NUM_BUTTON_BANKS * BUTTON_SAMPLE_PERIOD_MS) < BUTTON_DBLCLICK_INTERVAL_MS
    #define BUTTON_DBLCLICK_INTERVAL  ((BUTTON_DBLCLICK_INTERVAL_BANK_RND +1)*NUM_BUTTON_BANKS)
  #else
    #define BUTTON_DBLCLICK_INTERVAL  (BUTTON_DBLCLICK_INTERVAL_BANK_RND *NUM_BUTTON_BANKS)
  #endif
#endif /* NUMPOW_BUTTONS_PER_BANK */

/* BUTTON_LONGPRESS_HOLDTIME_MS的驱动程序内部使用版（单位为采样次数）*/
#if NUMPOW_BUTTONS_PER_BANK < 0
  #define BUTTON_LONGPRESS_HOLDTIME       (BUTTON_LONGPRESS_HOLDTIME_MS/BUTTON_SAMPLE_PERIOD_MS)
#else
  #define BUTTON_LONGPRESS_HOLDTIME       ((BUTTON_LONGPRESS_HOLDTIME_MS/NUM_BUTTON_BANKS) \
                                            /BUTTON_SAMPLE_PERIOD_MS)
#endif /* NUMPOW_BUTTONS_PER_BANK */

/* BUTTON_EQUIV_SGLCLICK_INTERVAL_MS的驱动程序内部使用版（单位为采样次数）*/
#if NUMPOW_BUTTONS_PER_BANK < 0
  #define BUTTON_EQUIV_SGLCLICK_INTERVAL  (BUTTON_EQUIV_SGLCLICK_INTERVAL_MS/BUTTON_SAMPLE_PERIOD_MS)
#else
  #define BUTTON_EQUIV_SGLCLICK_INTERVAL  ((BUTTON_EQUIV_SGLCLICK_INTERVAL_MS/NUM_BUTTON_BANKS) \
                                           /BUTTON_SAMPLE_PERIOD_MS)
#endif /* NUMPOW_BUTTONS_PER_BANK */
#if (BUTTON_EQUIV_SGLCLICK_INTERVAL == 0) && (BUTTON_EQUIV_SGLCLICK_INTERVAL_MS > 0)
  #warning Please increase the value of the macro 'EQUIV_SGLCLICK_INTERVAL_MS'
#endif /* BUTTON_EQUIV_SGLCLICK_INTERVAL */

/*!A 设置按键开合状态的逻辑值（仅需设置宏BUTTON_ON）*/
#define BUTTON_ON                 `$BUTTON_ON`ul  /* 单个按键闭合时的“逻辑”值（不一定需为引脚电平值）*/
#if BUTTON_ON == 0
  #define BUTTON_ALL_ON           0ul                 /* 所有按键闭合 */
#else
  #define BUTTON_ALL_ON           0xFFFFFFFFul        /* 所有按键闭合 */
#endif /* BUTTON_ON */

#define BUTTON_OFF                (!BUTTON_ON)        /* 单个按键断开 */
#define BUTTON_ALL_OFF            (~BUTTON_ALL_ON)    /* 所有按键断开 */

/* 按键动作类型：单击、长按、双击、松开（时刻）、按下（时刻）*/
#define BUTTON_ACTION_TYPE_SGLCLICK     3u
#define BUTTON_ACTION_TYPE_LONG         2u
#define BUTTON_ACTION_TYPE_DBLCLICK     1u
#define BUTTON_ACTION_TYPE_RELEASINGUP  0u
#define BUTTON_ACTION_TYPE_PUSHINGDOWN  BUTTON_ACTION_TYPE_SGLCLICK

/* 按键消息的按键编号域掩码、按键动作类型域掩码 */
#define MSG_BUTTON_NUM_MASK             0x3Fu
#define MSG_BUTTON_ACTION_TYPE_MASK     0xC0u
/* 按键消息的按键编号域掩码移位数、按键动作类型域掩码移位数 */
#define MSG_BUTTON_NUM_MASK_SHIFTS          0
#define MSG_BUTTON_ACTION_TYPE_MASK_SHIFTS  6

/*!!P 定义进入临界区、退出临界区函数名的宏替换（与处理器、编译器相关）
      如果不需要按键模块内部实现对按键消息队列操作的临界代码保护，将这2个宏定义为空值
*/
#include <cy_syslib.h>
#define ENT_CRITICAL_SEC          Cy_SysLib_EnterCriticalSection
#define EXT_CRITICAL_SEC          Cy_SysLib_ExitCriticalSection

/* 以下直至NULL_MCR_CRITICAL_SEC宏定义结束，为判断ENT_CRITICAL_SEC、EXT_CRITICAL_SEC是否定义为空或未定义 */
#if ENT_CRITICAL_SEC -ENT_CRITICAL_SEC -1 == 1
  #define EMPTY_MCR_ENT_CRITICAL_SEC
#endif /* ENT_CRITICAL_SEC */

#if EXT_CRITICAL_SEC -EXT_CRITICAL_SEC -1 == 1
  #define EMPTY_MCR_EXT_CRITICAL_SEC
#endif /* EXT_CRITICAL_SEC */

/* 如果定义了EMPTY_MCR_CRITICAL_SEC，表示ENT_CRITICAL_SEC和/或EXT_CRITICAL_SEC宏定义为空 */
#if defined(EMPTY_MCR_ENT_CRITICAL_SEC) || defined(EMPTY_MCR_EXT_CRITICAL_SEC)
  #define EMPTY_MCR_CRITICAL_SEC
#endif
/* 如果定义了NULL_MCR_CRITICAL_SEC，表示ENT_CRITICAL_SEC和/或EXT_CRITICAL_SEC宏定义为空或未定义 */
#if defined(EMPTY_MCR_CRITICAL_SEC) || !defined(ENT_CRITICAL_SEC) || !defined(EXT_CRITICAL_SEC)
  #define NULL_MCR_CRITICAL_SEC
#endif


/* ButtonBank数据类型定义 */
#if NUMPOW_BUTTONS_PER_BANK < 0     /* 按键不分页时 */
  #if NUM_BUTTONS <= 8
    typedef uint8 ButtonBank;
  #elif NUM_BUTTONS <= 16
    typedef uint16 ButtonBank;
  #else
    typedef uint32 ButtonBank;
  #endif /* NUM_BUTTONS */
#else                               /* 按键分页时 */
  #if (NUM_BUTTONS_PER_BANK == 4) || (NUM_BUTTONS_PER_BANK == 8) /* 每页4、8个按键 */
    typedef uint8 ButtonBank;
  #elif NUM_BUTTONS_PER_BANK == 16  /* 每页16个按键 */
    typedef uint16 ButtonBank;
  #elif NUM_BUTTONS_PER_BANK == 32  /* 每页32个按键 */
    typedef uint32 ButtonBank;
  #endif /* NUM_BUTTONS_PER_BANK */
#endif /* NUMPOW_BUTTONS_PER_BANK */


/*【 !!P 与移植相关的函数声明，以及在本平台上的移植实现  ...................
【注意】：`$INSTANCE_NAME`_Start()应在`$INSTANCE_NAME`.c中定义实现
          另外，同在`$INSTANCE_NAME`.c中的内部函数`$INSTANCE_NAME`_GetCurrentPBStatus()，也与移植相关，也需定义实现。
*/
/* 移植实现所需的I/O头文件包含 */
#include <cy_gpio.h> /* PDL中的GPIO API函数在该头文件中声明，引脚配置数据类型、相关宏在其中定义 */

/* 移植实现所需的OS消息队列头文件包含 */
#if PORTING_TO_OS_Q == FREERTOS
  #include "FreeRTOS.h"
  #include "queue.h"
#endif /* PORTING_TO_OS_Q */
/* 按键模块初始化（使用了以上头文件）*/
uint8 `$INSTANCE_NAME`_Start(void);        /* ...........................  】*/


/* 按键模块其它API函数声明 */
ButtonBank `$INSTANCE_NAME`_GetPBStatus(uint8 which);

/* 仅当全局使能了生成长按期间的等效连续单击消息时，实现以下两个函数 */
#if BUTTON_EQUIV_SGLCLICK_INTERVAL_MS > 0
  void `$INSTANCE_NAME`_SetEquivSglclickGen(uint8 which, ButtonBank val);
  ButtonBank `$INSTANCE_NAME`_GetEquivSglclickGen(uint8 which);
#endif /* BUTTON_EQUIV_SGLCLICK_INTERVAL_MS */

#define `$INSTANCE_NAME`_GetVersion()            ((uint16)_BUTTON_MODULE_VER_)

/* 按键消息队列不使用操作系统的消息队列服务时，收发消息的宏函数定义、API函数声明 */
#if PORTING_TO_OS_Q == NON_OS
  /* 可使用这两个简洁形式的宏函数代替紧接下方的对应版本 */
  #define `$INSTANCE_NAME`_GetMessage()               _`$INSTANCE_NAME`_GetMessage((uint8 *)0)
  #define `$INSTANCE_NAME`_SendMessage(button_msg)    _`$INSTANCE_NAME`_SendMessage(button_msg, (uint8 *)0)
    
  /* 自定义按键消息循环队列的API函数声明（不使用OS的消息队列服务时）*/
  uint8 _`$INSTANCE_NAME`_GetMessage(uint8 *msg_pvar);
  uint8 _`$INSTANCE_NAME`_SendMessage(uint8 button_msg, uint8 *msg_pvar);
#endif /* PORTING_TO_OS_Q */


/*!!P 按键消息队列使用操作系统的消息队列服务时，收发消息宏函数定义 */
#if PORTING_TO_OS_Q == FREERTOS
  extern QueueHandle_t qbutton;
  /* 按键消息队列收发消息宏函数（使用FreeRTOS的消息队列服务时）*/
  #define `$INSTANCE_NAME`_GetMessage(msg_pvar)        ((xQueueReceive(qbutton, msg_pvar, 0) == \
                                                     errQUEUE_EMPTY)? 0xFFu : *(msg_pvar))
  #define `$INSTANCE_NAME`_SendMessage(msg, msg_pvar)  (*(msg_pvar) = (msg),                    \
                                                     ((xQueueSend(qbutton, msg_pvar, 0) ==pdPASS)? 1u :0))
  /* 别名，兼容不使用OS的消息队列服务时的版本 */
  #define _`$INSTANCE_NAME`_GetMessage           `$INSTANCE_NAME`_GetMessage
  #define _`$INSTANCE_NAME`_SendMessage          `$INSTANCE_NAME`_SendMessage
#else
  /* 对于其它OS，请于下方注释#START ～ #END之间添加收发消息宏函数的定义 */
  /*************************************************************************
   *  Place your queue declarations and Rx/Tx macros under other OS
   ************************************************************************/
  /* `#START `$INSTANCE_NAME`_Queue_OS_Decs_RTxMacros` */
  
  /* `#END` */
#endif /* PORTING_TO_OS_Q */

#if defined(__cplusplus)
  }
#endif

#endif /* __`$INSTANCE_NAME`_H_ */

/* [] END OF FILE */
