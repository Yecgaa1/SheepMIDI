/*******************************************************************************
* 文件名: Button_Module.c  
* 版  本：1.1
* 日  期：2024-11-9
* 作  者：Inhaul Hsu
********************************************************************************
  按键模块、按键消息循环队列各函数实现

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
  1. 支持两种按键单击消息生成方式：
     - 快速（Button_Module.h中的宏SGL_CLICK_MSG_GEN_METHOD值为0）：单击按下时刻
       生成消息（若需检测双击，则双击过程中会先发送一条单击消息）
     - 延迟（宏SGL_CLICK_MSG_GEN_METHOD值为1）：单击按下后0.4秒再生成消息（双击
       过程中仅会发送双击消息，长按过程中的首次单击消息会延迟0.4s发送）
     延迟方式仅对于需检测双击动作的按键有效，若按键不需检测双击动作，则实际实现为
     快速方式。
  2. 各按键可被独立配置为仅需检测基本动作，以及需检测长按 和/或 双击。
  3. 可全局和单独使能各按键于其长按期间生成等效的连续单击消息
  4. 驱动层内部支持按键分页(分组)处理，适用于按键数量较多和降低每次的平均处理时间。
  5. 本文件中带!!P标记的注释处内容与移植相关
********************************************************************************
*/
#include "Button_Module.h"

/***************************************************************************
 *  Place your additional header code
 **************************************************************************/
/* `#START Button_Module_Add_Header_Code` */

/* `#END` */

/* 作为各按键最近一次动作类型的数组元素的初始值（初始动作均为松开）*/
#define BUTTON_ACTION_TYPE_BTYE_INIT   (BUTTON_ACTION_TYPE_RELEASINGUP |       \
                                        (BUTTON_ACTION_TYPE_RELEASINGUP << 2)| \
                                        (BUTTON_ACTION_TYPE_RELEASINGUP << 4)| \
                                        (BUTTON_ACTION_TYPE_RELEASINGUP << 6))

/*!!P 按键消息循环队列所需变量、宏的定义 */
#if PORTING_TO_OS_Q == NON_OS
  /* 自定义的按键消息循环队列：定义静态缓冲区、队列头指针、尾指针 */
  static uint8 qbutton_buff[NUM_QELEMS];
  static uint8 qbutton_front, qbutton_rear;

  /* Button_Module_Handler()函数中，QBUTTON_SENDMESSAGExxx()宏函数用名为_button_event的变量(保存待发消息)发送消息 */
  static uint8 _button_event;
  /* 宏PTR_OFFSETS为兼容诸如uC/OS、Mbed的引用方式发送消息的OS API函数而预留 */
  #define PTR_OFFSETS
#elif PORTING_TO_OS_Q == FREERTOS
  /* FreeRTOS队列句柄 */
  QueueHandle_t qbutton = NULL;

  /* Button_Module_Handler()函数中，QBUTTON_SENDMESSAGExxx()宏函数用名为_button_event的变量(保存待发消息)发送消息 */
  static uint8 _button_event;
  /* 宏PTR_OFFSETS为兼容诸如uC/OS、Mbed的引用方式发送消息的OS API函数而预留 */
  #define PTR_OFFSETS
#else
  /* 对于其它OS，请于下方注释#START ～ #END之间添加OS消息队列所需变量、宏的定义 */
  /*
  #if PORTING_TO_OS_Q == uCOS_III
    // uC/OS-III消息队列控制块
    OS_Q qbutton;
    // 保存uC/OS-III发送消息API函数的执行结果
    static OS_ERR err_var;

    // Button_Module_Handler()函数中，QBUTTON_SENDMESSAGExxx()宏函数用名为_button_event的数组元素msg_idx发送消息
    static uint8  _button_event[NUM_QELEMS];
    static uint8  msg_idx = NUM_QELEMS;
    // PTR_OFFSETS宏值为当前待发消息在_button_event数组中的索引
    #define PTR_OFFSETS    +(msg_idx = ((msg_idx < (NUM_QELEMS -1u))? msg_idx +1u : 0), msg_idx)
  #endif // PORTING_TO_OS_Q == uCOS_III
  */
  /*************************************************************************
   *  Place your queue definitions under other OS
   ************************************************************************/
  /* `#START Button_Module_Queue_OS_Defs` */

  /* `#END` */
#endif /* PORTING_TO_OS_Q */

/* 各按键开合状态的上次采样值 */
#if NUMPOW_BUTTONS_PER_BANK < 0     /* 按键不分页时 */
  /* bitX~bit0对应ButtonX~Button0 */
  static ButtonBank button_status_prev = (ButtonBank)BUTTON_ALL_OFF;
#else                               /* 按键分页时 */
  /* 每元素保存一个按键页的状态，靠b0位排列 */
  static ButtonBank button_status_prev[NUM_BUTTON_BANKS] =
                                     {(ButtonBank)BUTTON_ALL_OFF
  #if NUM_BUTTON_BANKS >= 2
                                     ,(ButtonBank)BUTTON_ALL_OFF
    #if NUM_BUTTON_BANKS >= 3
                                     ,(ButtonBank)BUTTON_ALL_OFF
      #if NUM_BUTTON_BANKS >= 4
                                     ,(ButtonBank)BUTTON_ALL_OFF
        #if NUM_BUTTON_BANKS >= 5
                                     ,(ButtonBank)BUTTON_ALL_OFF
          #if NUM_BUTTON_BANKS >= 6
                                     ,(ButtonBank)BUTTON_ALL_OFF
            #if NUM_BUTTON_BANKS >= 7
                                     ,(ButtonBank)BUTTON_ALL_OFF
              #if NUM_BUTTON_BANKS >= 8
                                     ,(ButtonBank)BUTTON_ALL_OFF
              #endif
            #endif
          #endif
        #endif
      #endif
    #endif
  #endif /* NUM_BUTTON_BANKS ......更多按键页时需额外附加数组元素的初始化 */
                                     };
#endif /* NUMPOW_BUTTONS_PER_BANK */

/* 有需要检测扩展动作的按键时，保存各按键的最近一次动作类型，每个按键占2个bit，第0号元素的b1b0位对应Button0 */
#if NUM_BUTTONS_EX_ACT > 0
  static uint8 button_action_type[(NUM_BUTTONS_EX_ACT+3)/4];
#endif /* NUM_BUTTONS_EX_ACT */

/* 按键长按、双击动作检测配置（依据按键长按、双击动作检测配置字）*/
#if NUMPOW_BUTTONS_PER_BANK < 0     /* 按键不分页时 */
  /* bitX~bit0对应ButtonX~Button0 */
  static const ButtonBank long_press_det_config = (ButtonBank)LONG_PRESS_DET_CONFIG;
  static const ButtonBank dbl_click_det_config = (ButtonBank)DBL_CLICK_DET_CONFIG;
#else                               /* 按键分页时 */
  /* NUM_BUTTON_BANKS_EX_ACT为扩展动作按键的页数 */
  #define NUM_BUTTON_BANKS_EX_ACT  ((NUM_BUTTONS_EX_ACT + NUM_BUTTONS_PER_BANK -1)/NUM_BUTTONS_PER_BANK)
  #if NUM_BUTTON_BANKS_EX_ACT == 0
    #undef  NUM_BUTTON_BANKS_EX_ACT
    #define NUM_BUTTON_BANKS_EX_ACT  1
  #endif /* NUM_BUTTON_BANKS_EX_ACT */

  /* 每元素保存一个按键页的长按动作检测配置，靠b0位排列 */
  static const ButtonBank long_press_det_config[NUM_BUTTON_BANKS_EX_ACT] =
                          {(ButtonBank)LONG_PRESS_DET_CONFIG
  #if NUM_BUTTON_BANKS_EX_ACT >= 2
                          ,(ButtonBank)(LONG_PRESS_DET_CONFIG >> NUM_BUTTONS_PER_BANK)
    #if NUM_BUTTON_BANKS_EX_ACT >= 3
                          ,(ButtonBank)(LONG_PRESS_DET_CONFIG >> (NUM_BUTTONS_PER_BANK*2))
      #if NUM_BUTTON_BANKS_EX_ACT >= 4
                          ,(ButtonBank)(LONG_PRESS_DET_CONFIG >> (NUM_BUTTONS_PER_BANK*3))
        #if NUM_BUTTON_BANKS_EX_ACT >= 5
                          ,(ButtonBank)(LONG_PRESS_DET_CONFIG >> (NUM_BUTTONS_PER_BANK*4))
          #if NUM_BUTTON_BANKS_EX_ACT >= 6
                          ,(ButtonBank)(LONG_PRESS_DET_CONFIG >> (NUM_BUTTONS_PER_BANK*5))
            #if NUM_BUTTON_BANKS_EX_ACT >= 7
                          ,(ButtonBank)(LONG_PRESS_DET_CONFIG >> (NUM_BUTTONS_PER_BANK*6))
              #if NUM_BUTTON_BANKS_EX_ACT >= 8
                          ,(ButtonBank)(LONG_PRESS_DET_CONFIG >> (NUM_BUTTONS_PER_BANK*7))
              #endif
            #endif
          #endif
        #endif
      #endif
    #endif
  #endif /* NUM_BUTTON_BANKS_EX_ACT ......更多按键页时需额外附加数组元素的初始化 */
                          };
  /* 每元素保存一个按键页的双击动作检测配置，靠b0位排列 */
  static const ButtonBank dbl_click_det_config[NUM_BUTTON_BANKS_EX_ACT] =
                          {(ButtonBank)DBL_CLICK_DET_CONFIG
  #if NUM_BUTTON_BANKS_EX_ACT >= 2
                          ,(ButtonBank)(DBL_CLICK_DET_CONFIG >> NUM_BUTTONS_PER_BANK)
    #if NUM_BUTTON_BANKS_EX_ACT >= 3
                          ,(ButtonBank)(DBL_CLICK_DET_CONFIG >> (NUM_BUTTONS_PER_BANK*2))
      #if NUM_BUTTON_BANKS_EX_ACT >= 4
                          ,(ButtonBank)(DBL_CLICK_DET_CONFIG >> (NUM_BUTTONS_PER_BANK*3))
        #if NUM_BUTTON_BANKS_EX_ACT >= 5
                          ,(ButtonBank)(DBL_CLICK_DET_CONFIG >> (NUM_BUTTONS_PER_BANK*4))
          #if NUM_BUTTON_BANKS_EX_ACT >= 6
                          ,(ButtonBank)(DBL_CLICK_DET_CONFIG >> (NUM_BUTTONS_PER_BANK*5))
            #if NUM_BUTTON_BANKS_EX_ACT >= 7
                          ,(ButtonBank)(DBL_CLICK_DET_CONFIG >> (NUM_BUTTONS_PER_BANK*6))
              #if NNUM_BUTTON_BANKS_EX_ACT >= 8
                          ,(ButtonBank)(DBL_CLICK_DET_CONFIG >> (NUM_BUTTONS_PER_BANK*7))
              #endif
            #endif
          #endif
        #endif
      #endif
    #endif
  #endif /* NUM_BUTTON_BANKS_EX_ACT ......更多按键页时需额外附加数组元素的初始化 */
                          };
#endif /* NUMPOW_BUTTONS_PER_BANK */


/* 长按期间，生成等效连续单击消息的配置（依据“等效单击生成”配置字）*/
#if BUTTON_EQUIV_SGLCLICK_INTERVAL_MS > 0
#if NUMPOW_BUTTONS_PER_BANK < 0     /* 按键不分页时 */
  /* bitX~bit0对应ButtonX~Button0 */
  static ButtonBank equiv_sglclick_gen_config;
#else                               /* 按键分页时 */
  /* 每元素保存一个按键页的“等效单击生成”配置，靠b0位排列 */
  static ButtonBank equiv_sglclick_gen_config[NUM_BUTTON_BANKS];
#endif /* NUMPOW_BUTTONS_PER_BANK */
#endif /* BUTTON_EQUIV_SGLCLICK_INTERVAL_MS */


/* 将动作类型字节变量act_byte中的，自第bitnum_start位起的两位信息，更新为动作类型act_type */
#define UPDATE_ACTION_TYPE(act_byte, bitnum_start, act_type)                             do{         \
                                                     act_byte &= ~(0x03u <<(bitnum_start));          \
                                                     act_byte |= act_type <<(bitnum_start);}while(0)

/*!!P Button_Module_Handler()函数中所用的，发送消息的内联版或OS版宏函数定义 */
#if PORTING_TO_OS_Q == NON_OS
  /* _Button_Module_SendMessage()函数的内联版，缩短耗时，仅于本文件中可见 */
  #define QBUTTON_SENDMESSAGE(dat, msg_var)  do{if(qbutton_front != ((qbutton_rear <NUM_QELEMS-1u)?  \
                                                                    qbutton_rear+1u : 0))            \
                                                { qbutton_buff[qbutton_rear] = (dat);                \
                                                  qbutton_rear = (qbutton_rear < NUM_QELEMS-1u)?     \
                                                                 qbutton_rear+1u : 0;}               \
                                             }while(0)
  /* QBUTTON_SENDMESSAGE_EX()使用临时变量tmp_var，同等操作的耗时比QBUTTON_SENDMESSAGE()少些微 */
  #define QBUTTON_SENDMESSAGE_EX(num_btn, act_type, msg_var, tmp_var)         do{ msg_var = num_btn; \
                                       msg_var |= act_type << MSG_BUTTON_ACTION_TYPE_MASK_SHIFTS;    \
                                       tmp_var = (qbutton_rear <NUM_QELEMS-1u)? qbutton_rear+1u : 0; \
                                       if(qbutton_front!=tmp_var){qbutton_buff[qbutton_rear]=msg_var;\
                                                                  qbutton_rear = tmp_var;} }while(0)
#elif PORTING_TO_OS_Q == FREERTOS
  /* 使用FreeRTOS消息队列的实现版 */
  #define QBUTTON_SENDMESSAGE(dat, msg_var)    do{ msg_var = dat;                                    \
                                               xQueueSendFromISR(qbutton, &msg_var, NULL); }while(0)
  #define QBUTTON_SENDMESSAGE_EX(num_btn, act_type, msg_var, redun)    do{ msg_var = num_btn;        \
                                       msg_var |= act_type << MSG_BUTTON_ACTION_TYPE_MASK_SHIFTS;    \
                                       xQueueSendFromISR(qbutton, &msg_var, NULL); }while(0)
#else
  /* 对于其它OS，请于下方注释#START ～ #END之间添加发送消息的OS消息队列版宏函数定义 */
  /*
  #if PORTING_TO_OS_Q == uCOS_III
    #define QBUTTON_SENDMESSAGE(dat, msg_pvar)                            do{ *(msg_pvar) = dat;     \
                                OSQPost(&qbutton, (msg_pvar), 1, OS_OPT_POST_FIFO, &err_var);}while(0)
    #define QBUTTON_SENDMESSAGE_EX(num_btn, act_type, msg_pvar, redun)    do{ *(msg_pvar) = num_btn; \
                                *(msg_pvar) |= act_type << MSG_BUTTON_ACTION_TYPE_MASK_SHIFTS;       \
                                OSQPost(&qbutton, (msg_pvar), 1, OS_OPT_POST_FIFO, &err_var);}while(0)
  #endif // PORTING_TO_OS_Q == uCOS_III
  */
  /*************************************************************************
   *  Place your QBUTTON_SENDMESSAGExxx() macro definitions under other OS
   ************************************************************************/
  /* `#START Button_Module_SendMsgMacro_OS_Defs` */

  /* `#END` */
#endif /* PORTING_TO_OS_Q */


static inline ButtonBank Button_Module_GetCurrentPBStatus(uint8 bank);
static void Button_Module_Handler(uint8 bank);

/* 定时器中断服务函数 */
static void Int_Tmr_Button_handler(void);


/*******************************************************************************
*  函数名：Int_Tmr_Button_handler
********************************************************************************
*
*  【功    能】
*    按键模块定时器中断服务程序
*    每发生一次计数值到达0时，调用一次Button_Module_Handler()。
*    用户可在两处`#START` ～ `#END`注释之间，添加附加代码，以充分利用此定时器中断。
*
*  【入口参数】
*    无
*
*  【返 回 值】
*    无
*
*******************************************************************************/
static void Int_Tmr_Button_handler(void)
{
#if NUMPOW_BUTTONS_PER_BANK >= 0
  static uint8 bank = 0;
#endif /* NUMPOW_BUTTONS_PER_BANK */
  
  /*************************************************************************
   *  Place your additional codes
   ************************************************************************/
  /* `#START Int_Tmr_Button_handler_header` */

  /* `#END` */

#if NUMPOW_BUTTONS_PER_BANK >= 0
  Button_Module_Handler(bank);
  /* 按键分页时，切换为下一页号 */
    bank = (bank < (NUM_BUTTON_BANKS - 1u))? bank + 1u : 0;
#else
  Button_Module_Handler(0);
#endif /* NUMPOW_BUTTONS_PER_BANK */

  /*************************************************************************
   *  Place your additional codes
   ************************************************************************/
  /* `#START Int_Tmr_Button_handler_TC` */

  /* `#END` */
}


/*******************************************************************************
*  函数名：Button_Module_Start
********************************************************************************
*
*  【功    能】
*    初始化按键模块，含初始化按键消息循环队列，以及定时器及其中断
*
*  【入口参数】
*    无
*
*  【返 回 值】
*    0：初始化成功
*    1：初始化失败（按键消息队列创建不成功）
*
*  注：可选内置的临界代码保护功能
*
*  !!P 本函数与移植相关
*******************************************************************************/
uint8 Button_Module_Start(void)
{
  uint8 i;

#ifndef NULL_MCR_CRITICAL_SEC
  uint32 savedIntrStatus = ENT_CRITICAL_SEC();
#endif /* NULL_MCR_CRITICAL_SEC */

  /* 按键相关I/O初始化省略，由PSoC Creator进行 */

  /* 有需要检测扩展动作的按键时，设置这些按键的初始动作类型为松开 */
#if NUM_BUTTONS_EX_ACT > 0
  for(i=0; i<(NUM_BUTTONS_EX_ACT+3)/4; i++)
  {
    button_action_type[i] = BUTTON_ACTION_TYPE_BTYE_INIT;
  }
#endif /* NUM_BUTTONS_EX_ACT */

  /* 当全局使能了长按期间生成等效的连续单击消息时
     初始设置各按键在长按期间是否生成等效连续单击消息（依据“等效单击生成”配置字）
  */
#if BUTTON_EQUIV_SGLCLICK_INTERVAL_MS > 0
#if NUMPOW_BUTTONS_PER_BANK < 0     /* 按键不分页时 */
  equiv_sglclick_gen_config = (ButtonBank)EQUIV_SGLCLICK_GEN_CONFIG;
#else                               /* 按键分页时 */
  for(i=0; i<NUM_BUTTON_BANKS; i++)
  {
    equiv_sglclick_gen_config[i] = (ButtonBank)(EQUIV_SGLCLICK_GEN_CONFIG >> (NUM_BUTTONS_PER_BANK*i));
  }
#endif /* NUMPOW_BUTTONS_PER_BANK */
#endif /* BUTTON_EQUIV_SGLCLICK_INTERVAL_MS */

  /* 按键消息循环队列初始化 */
#if PORTING_TO_OS_Q == NON_OS
  qbutton_front = qbutton_rear = 0;
  #ifndef NULL_MCR_CRITICAL_SEC
    EXT_CRITICAL_SEC(savedIntrStatus);
  #endif /* NULL_MCR_CRITICAL_SEC */
  i = 0;
#else /* OS队列时，只创建一次。再次调用本函数时，清空OS队列。*/
  #ifndef NULL_MCR_CRITICAL_SEC
    EXT_CRITICAL_SEC(savedIntrStatus);
  #endif /* NULL_MCR_CRITICAL_SEC */
  if(qbutton == NULL)
  {
  #if PORTING_TO_OS_Q == FREERTOS
    qbutton = xQueueCreate((UBaseType_t)NUM_QELEMS, (UBaseType_t)1);
    i = (qbutton != NULL)? 0 : 1u;
  #else
    /* 对于其它OS，请于下方注释#START ～ #END之间添加创建OS消息队列的代码 */
    /***********************************************************************
     *  Place your queue initialization using OS system service
     **********************************************************************/
    /* `#START Button_Module_Queue_OS_Init` */

    /* `#END` */
  #endif /* PORTING_TO_OS_Q */
  }
  else
  {
  #if PORTING_TO_OS_Q == FREERTOS
    xQueueReset(qbutton);
    i = 0;
  #else
    /* 对于其它OS，请于下方注释#START ～ #END之间添加清空OS消息队列的代码 */
    /***********************************************************************
     *  Place your queue reset using OS system service
     **********************************************************************/
    /* `#START Button_Module_Queue_OS_Reset` */

    /* `#END` */
  #endif /* PORTING_TO_OS_Q */
  }
#endif /* PORTING_TO_OS_Q */

  /* 定时器TC中断初始化 */
  isr_Tmr_Button_ClearPending();
  isr_Tmr_Button_StartEx(Int_Tmr_Button_handler);

  /* 定时器初始化 */
  Timer_Button_Start();
  Timer_Button_WritePeriod(BUTTON_SAMPLE_PERIOD_MS);

  return i;
}


/*******************************************************************************
*  函数名：Button_Module_GetCurrentPBStatus
********************************************************************************
*
*  【功    能】
*    使用GPIO固件获取所有按键或单个按键页的当前开合状态（约定各按键向b0方向排列）
*
*  【入口参数】
*    bank：按键不分页时，该参数不使用
*          按键分页时，为按键页编号0 ~ NUM_BUTTON_BANKS-1
*
*  【返 回 值】
*    按键当前开合状态：按键不分页时，每位值对应所有各按键的BUTTON_ON或BUTTON_OFF
*                      按键分页时，为指定页的各按键开合状态，向b0方向排列
*
*  注：正确且高效地获取单个按键页当前开合状态与硬件配置有关，属于移植相关内容，当
*      前代码对按键分页的情形仅做了形式上的处理（仅获取页0或1按键状态，见代码注释）
*
*  !!P 本函数与移植相关
*******************************************************************************/
static inline ButtonBank Button_Module_GetCurrentPBStatus(uint8 bank)
{
  ButtonBank val;
  
#if NUMPOW_BUTTONS_PER_BANK < 0     /* 按键不分页时 */
  (void)bank;
#endif /* NUMPOW_BUTTONS_PER_BANK */

  /* 获取所有按键当前开合状态，或者第bank页按键当前状态。本例获取开发板上的所有用户机械按键状态。
     可在下方注释#START ～ #END之间添加获取其它开发板上的用户机械按键、及更多按键的状态读取代码 */
  val  = (ButtonBank)(BUTTON_ALL_OFF << NUM_BUTTONS);
  /* P6[1](SW2), P15[5](SW3) */
  // val |= (ButtonBank)((CyPins_ReadPin(Pin_SW2__0__PC) ==0)? BUTTON_ON : BUTTON_OFF);
  // val |= (ButtonBank)((CyPins_ReadPin(Pin_SW3__0__PC) ==0)? BUTTON_ON : BUTTON_OFF) <<1;
  /*************************************************************************
   *  Place your additional button status reads
   ************************************************************************/
  /* `#START Button_Module_Add_Button_Status_Reads` */

  /* `#END` */

#if NUMPOW_BUTTONS_PER_BANK < 0     /* 按键不分页时 */
  return val;
#else                               /* 按键分页时 */
  /* 每页4按键时，ButtonBank类型为uint8，val值保存页0、页1按键开合状态。
     此处从val中提取页0、1按键状态，其余页各按键状态固定为BUTTON_OFF */
  #if NUM_BUTTONS_PER_BANK == 4
    switch(bank)
    {
      case 0:
        return val;
      case 1:
        return val >> 4;
      default:
        return (ButtonBank)BUTTON_ALL_OFF;
    }
  /* 每页8、16、32按键时，ButtonBank类型为uint8、16、32，val值保存页0按键开合状态。
     此处从val中提取页0按键状态，其余页各按键状态固定为BUTTON_OFF */
  #else
    return (bank == 0)? val : (ButtonBank)BUTTON_ALL_OFF;
  #endif /* NUM_BUTTONS_PER_BANK */
#endif /* NUMPOW_BUTTONS_PER_BANK */
}


/*******************************************************************************
*  函数名：Button_Module_GetPBStatus
********************************************************************************
*
*  【功    能】
*    获取单个按键、单个按键页、所有按键的开合状态（约定各按键向b0方向排列）
*
*  【入口参数】
*    which：按键不分页时
*             按键编号0 ~ NUM_BUTTONS-1，适用于获取单个按键状态
*             0xFF，适用于获取所有按键状态
*           按键分页时
*             按键编号0 ~ NUM_BUTTONS-1，适用于获取单个按键状态
*             0x80 + “按键页编号0 ~ NUM_BUTTON_BANKS-1”，适用于获取单个按键页状态
*
*  【返 回 值】
*    按键开合状态：单个按键时，为BUTTON_ON或BUTTON_OFF
*                  所有按键或单个按键页时，每位值对应一个按键的BUTTON_ON或BUTTON_OFF
*
*  注：可选内置的临界代码保护功能
*******************************************************************************/
ButtonBank Button_Module_GetPBStatus(uint8 which)
{
  ButtonBank val = (ButtonBank)BUTTON_OFF;
#ifndef NULL_MCR_CRITICAL_SEC
  uint32 savedIntrStatus = ENT_CRITICAL_SEC();
#endif /* NULL_MCR_CRITICAL_SEC */

#if NUMPOW_BUTTONS_PER_BANK < 0     /* 按键不分页时 */
  if(which == 0xFFu)
  { /* 所有按键 */
    val = button_status_prev;
  }
  else if(which < NUM_BUTTONS)
  { /* 单个按键 */
    val = (button_status_prev >> which) & (ButtonBank)1u;
  }
#else                               /* 按键分页时 */
  if(which < NUM_BUTTONS)
  { /* 单个按键 */
    val = (ButtonBank)(which & ~(0xFFu << NUMPOW_BUTTONS_PER_BANK));
    which >>= NUMPOW_BUTTONS_PER_BANK;
    val = (button_status_prev[which] >> val) & (ButtonBank)1u;
  }
  else if(which > 0x7Fu)
  { /* 单个按键页 */
    which &= 0x7Fu;
    if(which < NUM_BUTTON_BANKS)
    {
      val = button_status_prev[which];
    }
    else
    { /* 非法按键页号 */
      val = (ButtonBank)BUTTON_ALL_OFF;
    }
  }
#endif /* NUMPOW_BUTTONS_PER_BANK */

#ifndef NULL_MCR_CRITICAL_SEC
  EXT_CRITICAL_SEC(savedIntrStatus);
#endif /* NULL_MCR_CRITICAL_SEC */
  return val;
}


/*******************************************************************************
*  函数名：Button_Module_Handler
********************************************************************************
*
*  【功    能】
*    全部按键或单个按键页的按键开合状态采样、按键动作识别、按键消息入队列
*    应每隔BUTTON_SAMPLE_PERIOD_MS ms调用一次本函数
*
*    说明：1. 仅需检测基本动作（按下和松开动作）时，只发送按下和松开消息。
*          2. 需检测长按动作时，一次长按过程先发送一条单击消息，再发送一条长按消
*             息，按键松开时发送一条松开消息。
*          3. 需检测双击动作时，一次双击过程：
*             - 快速方式时，先发送一条单击消息，再发送一条双击消息，按键最后松开
*               时不发送松开消息。
*             - 延迟方式时，仅发送一条双击消息。
*          4. 有需检测扩展动作时，单击、双击时不发送松开消息。
*          5. 如果想获知单击后、或者双击后按键处于闭合还是断开状态，请配合使用函
*             数Button_Module_GetPBStatus()
*          6. 可配置各按键于其长按期间生成等效的连续单击消息
*             宏BUTTON_EQUIV_SGLCLICK_INTERVAL_MS设置等效单击消息的发送间隔，宏值
*             为0时全局禁止此功能。
*             Button_Module_SetEquivSglclickGen()和Button_Module_GetEquivSglclickGen()
*             函数分别设置和获取某按键的配置。
*
*  【入口参数】
*    bank：按键不分页时，该参数值不使用
*          按键分页时，为按键页编号0 ~ NUM_BUTTON_BANKS-1
*
*  【返 回 值】
*    无
*
*******************************************************************************/
#if NUMPOW_BUTTONS_PER_BANK < 0     /* 按键不分页时的版本 */

static void Button_Module_Handler(uint8 bank)
{
/* 以下最外层#if...#endif为：有需要检测扩展动作的按键时的变量定义 */
#if NUM_BUTTONS_EX_ACT > 0
  static uint8 button_samples; /* 按键采样次数（即Button_Module_Handler()调用次数）*/
  static uint8 tm_1st_sgl_pushing[NUM_BUTTONS_EX_ACT]; /* 各按键的最近单击按下时刻 */
  
  uint8 idx_bit;
#endif /* NUM_BUTTONS_EX_ACT */
  uint8 iButton;

  ButtonBank button_status_cur; /* 各按键开合状态的当前采样值，bitX~bit0对应ButtonX~Button0 */
  ButtonBank act_flags;         /* 标记各按键是否有动作，某位值为1表示对应按键有动作 */

  /* 起始按键编号 */
  iButton = 0;

  /* 读各按键当前开合状态 */
  button_status_cur = Button_Module_GetCurrentPBStatus(bank);

  /* 与上一状态采样值比较，标记各按键是否有动作至act_flags */
  act_flags = button_status_cur ^ button_status_prev;

/* 以下最外层#if...#endif为：需要检测扩展动作的按键处理 ...... */
#if NUM_BUTTONS_EX_ACT > 0
  
  /* 时间变量增一 */
  button_samples++;

  for(; iButton <NUM_BUTTONS_EX_ACT; iButton++)
  {
    tm_1st_sgl_pushing[iButton]++;

    /* 计算第iButton号按键的动作类型信息在其数组元素button_action_type[]中的起始位号 */
    idx_bit = iButton & 0x03u;
    idx_bit <<= 1;
    
    /* bank保存第iButton号按键最近的动作类型 */
    bank = (button_action_type[iButton >>2] >>idx_bit) & 0x03u;

    if(((act_flags >> iButton) & (ButtonBank)1u) != 0)  /* 第iButton号按键有动作 */
    {
      if(((button_status_cur >> iButton) & (ButtonBank)1u) == (ButtonBank)BUTTON_ON)
      {                                             /* 第iButton号按键按下 */
        if(((dbl_click_det_config >> iButton) & (ButtonBank)1u) != 0)
        {                                           /* 需检测双击时 */
          if((bank == BUTTON_ACTION_TYPE_SGLCLICK) &&
             (tm_1st_sgl_pushing[iButton] < BUTTON_DBLCLICK_INTERVAL))
          { /* 第iButton号按键最近为单击，且距“最近单击按下时刻”的间隔时间
               短于双击间隔阈值，判定为双击 */
            UPDATE_ACTION_TYPE(button_action_type[iButton >>2], idx_bit, BUTTON_ACTION_TYPE_DBLCLICK);
            /* 构造iButton号按键双击消息，并将其入队列 */
            QBUTTON_SENDMESSAGE_EX(iButton, BUTTON_ACTION_TYPE_DBLCLICK, _button_event PTR_OFFSETS, idx_bit);
          }
          else
          { /* 判定为单击 */
            UPDATE_ACTION_TYPE(button_action_type[iButton >>2], idx_bit, BUTTON_ACTION_TYPE_SGLCLICK);
  #if SGL_CLICK_MSG_GEN_METHOD == 0
            /* 构造iButton号按键单击消息，并将其入队列 */
            QBUTTON_SENDMESSAGE_EX(iButton, BUTTON_ACTION_TYPE_SGLCLICK, _button_event PTR_OFFSETS, idx_bit);
  #endif /* SGL_CLICK_MSG_GEN_METHOD */
          }
        }
        else                                        /* 不需检测双击时 */
        { /* 判定为单击 */
          UPDATE_ACTION_TYPE(button_action_type[iButton >>2], idx_bit, BUTTON_ACTION_TYPE_SGLCLICK);
          /* 构造iButton号按键单击消息，并将其入队列 */
          QBUTTON_SENDMESSAGE_EX(iButton, BUTTON_ACTION_TYPE_SGLCLICK, _button_event PTR_OFFSETS, idx_bit);
        }
        /* 更新此按键的最近单击按下时刻值为0 */
        tm_1st_sgl_pushing[iButton] = 0;
      }
      else                                          /* 第iButton号按键松开 */
      {
        if(((long_press_det_config >> iButton) & (ButtonBank)1u) != 0)
        {                                           /* 需检测长按时 */
          if(bank == BUTTON_ACTION_TYPE_LONG)
          { /* 最近为长按，判定为松开（结束长按动作） */
            UPDATE_ACTION_TYPE(button_action_type[iButton >>2], idx_bit, BUTTON_ACTION_TYPE_RELEASINGUP);
            /* 构造iButton号按键松开消息，并将其入队列 */
            QBUTTON_SENDMESSAGE_EX(iButton, BUTTON_ACTION_TYPE_RELEASINGUP, _button_event PTR_OFFSETS, idx_bit);
          }
        }
  #ifndef STRICT_EXT_DET_CONFIG
        else if(((dbl_click_det_config >> iButton) & (ButtonBank)1u) == 0)
        { /* 不需检测该按键的长按和双击时，判定为松开，发送iButton号按键松开消息
           注：此为按键动作检测配置字不合理的防护性处理，若能遵Button_Module.h所述配置，删除此else if */
          UPDATE_ACTION_TYPE(button_action_type[iButton >>2], idx_bit, BUTTON_ACTION_TYPE_RELEASINGUP);
          QBUTTON_SENDMESSAGE_EX(iButton, BUTTON_ACTION_TYPE_RELEASINGUP, _button_event PTR_OFFSETS, idx_bit);
        }
  #endif /* STRICT_EXT_DET_CONFIG */
      }
    }
    else                                            /* 第iButton号按键无动作 */
    {
      if((bank == BUTTON_ACTION_TYPE_SGLCLICK)
  #if LONG_PRESS_FR_DBLCLICK_SUPPORT == 1
         || (bank == BUTTON_ACTION_TYPE_DBLCLICK)
  #endif /* LONG_PRESS_FR_DBLCLICK_SUPPORT */
                                                 )
      { /* 第iButton号按键最近为单击（或双击） */
        if(tm_1st_sgl_pushing[iButton] > BUTTON_LONGPRESS_HOLDTIME)
        { /* 且已过了2s（长按按下保持时长阈值）*/
          if(((long_press_det_config >> iButton) & (ButtonBank)1u) != 0)  /* 需检测长按时 */
          {
            if(((button_status_cur >> iButton) & (ButtonBank)1u) == (ButtonBank)BUTTON_ON)
            { /* 第iButton号按键当前保持按下，判定为长按 */
              UPDATE_ACTION_TYPE(button_action_type[iButton >>2], idx_bit, BUTTON_ACTION_TYPE_LONG);
              /* 构造iButton号按键长按消息，并将其入队列 */
              QBUTTON_SENDMESSAGE_EX(iButton, BUTTON_ACTION_TYPE_LONG, _button_event PTR_OFFSETS, idx_bit);

  #if BUTTON_EQUIV_SGLCLICK_INTERVAL_MS > 0
              tm_1st_sgl_pushing[iButton] = 0;
  #endif /* BUTTON_EQUIV_SGLCLICK_INTERVAL_MS */
            }
            else
            { /* 期间未有长按，将该按键动作类型更新为松开 */
              UPDATE_ACTION_TYPE(button_action_type[iButton >>2], idx_bit, BUTTON_ACTION_TYPE_RELEASINGUP);
            }
          }
          else                                      /* 不需检测长按时 */
          { /* 将该按键动作类型更新为松开 */
            UPDATE_ACTION_TYPE(button_action_type[iButton >>2], idx_bit, BUTTON_ACTION_TYPE_RELEASINGUP);
          }
        }
  #if SGL_CLICK_MSG_GEN_METHOD == 1
        else if((tm_1st_sgl_pushing[iButton] == BUTTON_DBLCLICK_INTERVAL)
  #if LONG_PRESS_FR_DBLCLICK_SUPPORT == 1
                && (bank == BUTTON_ACTION_TYPE_SGLCLICK)
  #endif /* LONG_PRESS_FR_DBLCLICK_SUPPORT */
                                                        )
        { /* 且已过了0.4s（双击时的连续两次单击时间间隔阈值）*/
          if(((dbl_click_det_config >> iButton) & (ButtonBank)1u) != 0)
          { /* 需检测双击时，构造iButton号按键单击消息，并将其入队列 */
            QBUTTON_SENDMESSAGE_EX(iButton, BUTTON_ACTION_TYPE_SGLCLICK, _button_event PTR_OFFSETS, idx_bit);
          }
        }
  #endif /* SGL_CLICK_MSG_GEN_METHOD */
      }
  #if BUTTON_EQUIV_SGLCLICK_INTERVAL_MS > 0
      else
      {
        if(((equiv_sglclick_gen_config >> iButton) & (ButtonBank)1u) != 0)
        { /* 需生成第iButton号按键等效单击消息时 */
          if(bank == BUTTON_ACTION_TYPE_LONG)
          { /* 第iButton号按键最近为长按 */
            if(tm_1st_sgl_pushing[iButton] >= BUTTON_EQUIV_SGLCLICK_INTERVAL)
            {
              tm_1st_sgl_pushing[iButton] = 0;
            }
            if(tm_1st_sgl_pushing[iButton] == 0)
            { /* 构造iButton号按键单击消息，并将其入队列 */
              QBUTTON_SENDMESSAGE_EX(iButton, BUTTON_ACTION_TYPE_SGLCLICK, _button_event PTR_OFFSETS, idx_bit);
            }
          }
        }
      }
  #endif /* BUTTON_EQUIV_SGLCLICK_INTERVAL_MS */
    }
  }
#endif /* NUM_BUTTONS_EX_ACT */

/* 以下if为：只需检测基本动作的按键处理 ...... */

  /* 有按键动作时，搜索有动作的按键，并判断其是什么动作 */
  act_flags &= (ButtonBank)(-1) << NUM_BUTTONS_EX_ACT;
  if(act_flags != 0)
  {
    for(; iButton <NUM_BUTTONS; iButton++)
    {
      if(((act_flags >> iButton) & (ButtonBank)1u) != 0)      /* 第iButton号按键有动作 */
      { /* 构造第iButton号按键按下/松开消息，并将其入队列 */
        bank  = iButton;
        bank |= (((button_status_cur >> iButton) & (ButtonBank)1u) == (ButtonBank)BUTTON_ON)?
                BUTTON_ACTION_TYPE_PUSHINGDOWN << MSG_BUTTON_ACTION_TYPE_MASK_SHIFTS:
                BUTTON_ACTION_TYPE_RELEASINGUP << MSG_BUTTON_ACTION_TYPE_MASK_SHIFTS;
        QBUTTON_SENDMESSAGE(bank, _button_event PTR_OFFSETS);
      }
    }
  }

  /* 最后，将各按键的上一状态，更新为当前状态 */
  button_status_prev = button_status_cur;
}

#else                               /* 按键分页时的版本 */

static void Button_Module_Handler(uint8 bank)
{
/* 以下最外层#if...#endif为：有需要检测扩展动作的按键时的变量定义 */
#if NUM_BUTTONS_EX_ACT > 0
  static uint8 button_samples; /* 按键采样次数（即Button_Module_Handler()调用次数）*/
  static uint8 tm_1st_sgl_pushing[NUM_BUTTONS_EX_ACT]; /* 各按键的最近单击按下时刻 */

  uint8 idx_bit, act_type;
#endif /* NUM_BUTTONS_EX_ACT */
  uint8 iButton;

  uint8 iButtonEndPlus1;        /* 本按键页的最大按键号+1 */
  uint8 iButton_inbank;         /* 按键号iButton在其按键页中的页内编号(页内偏移号) */

  ButtonBank button_status_cur; /* 各按键开合状态的当前采样值 */
  ButtonBank act_flags;         /* 标记各按键是否有动作，某位值为1表示对应按键有动作 */

  /* 确定本按键页的起始按键编号，和结束按键编号 */
  iButton = bank << NUMPOW_BUTTONS_PER_BANK;
  iButtonEndPlus1 = iButton + NUM_BUTTONS_PER_BANK;
  iButtonEndPlus1 = (iButtonEndPlus1 > NUM_BUTTONS)? NUM_BUTTONS : iButtonEndPlus1;
  /* 计算按键号iButton在其按键页中的页内编号(页内偏移号) */
  iButton_inbank = iButton & ~(0xFFu << NUMPOW_BUTTONS_PER_BANK);

  /* 读本按键页各按键当前开合状态 */
  button_status_cur = Button_Module_GetCurrentPBStatus(bank);

  /* 与上一状态采样值比较，标记各按键是否有动作至act_flags */
  act_flags = button_status_cur ^ button_status_prev[bank];
#if NUM_BUTTONS_PER_BANK == 4
  act_flags &= 0x0Fu;
#endif /* NUM_BUTTONS_PER_BANK */

/* 以下最外层#if...#endif为：需要检测扩展动作的按键处理 ...... */
#if NUM_BUTTONS_EX_ACT > 0
  
  /* 时间变量增一 */
  button_samples++;

  while((iButton <NUM_BUTTONS_EX_ACT) && (iButton <iButtonEndPlus1))
  {
    tm_1st_sgl_pushing[iButton]++;

    /* 计算第iButton号按键的动作类型信息在其数组元素button_action_type[]中的起始位号 */
    idx_bit = iButton & 0x03u;
    idx_bit <<= 1;

    /* act_type保存第iButton号按键最近的动作类型 */
    act_type = (button_action_type[iButton >>2] >>idx_bit) & 0x03u;

    if(((act_flags >> iButton_inbank) & (ButtonBank)1u) != 0)  /* 第iButton号按键有动作 */
    {
      if(((button_status_cur >> iButton_inbank) & (ButtonBank)1u) == (ButtonBank)BUTTON_ON)
      {                                             /* 第iButton号按键按下 */
        if(((dbl_click_det_config[bank] >> iButton_inbank) & (ButtonBank)1u) != 0)
        {                                           /* 需检测双击时 */
          if((act_type == BUTTON_ACTION_TYPE_SGLCLICK) &&
             (tm_1st_sgl_pushing[iButton] < BUTTON_DBLCLICK_INTERVAL))
          { /* 第iButton号按键最近为单击，且距“最近单击按下时刻”的间隔时间
               短于双击间隔阈值，判定为双击 */
            UPDATE_ACTION_TYPE(button_action_type[iButton >>2], idx_bit, BUTTON_ACTION_TYPE_DBLCLICK);
            /* 构造iButton号按键双击消息，并将其入队列 */
            QBUTTON_SENDMESSAGE_EX(iButton, BUTTON_ACTION_TYPE_DBLCLICK, _button_event PTR_OFFSETS, idx_bit);
          }
          else
          { /* 判定为单击 */
            UPDATE_ACTION_TYPE(button_action_type[iButton >>2], idx_bit, BUTTON_ACTION_TYPE_SGLCLICK);
  #if SGL_CLICK_MSG_GEN_METHOD == 0
            /* 构造iButton号按键单击消息，并将其入队列 */
            QBUTTON_SENDMESSAGE_EX(iButton, BUTTON_ACTION_TYPE_SGLCLICK, _button_event PTR_OFFSETS, idx_bit);
  #endif /* SGL_CLICK_MSG_GEN_METHOD */
          }
        }
        else                                        /* 不需检测双击时 */
        { /* 判定为单击 */
          UPDATE_ACTION_TYPE(button_action_type[iButton >>2], idx_bit, BUTTON_ACTION_TYPE_SGLCLICK);
          /* 构造iButton号按键单击消息，并将其入队列 */
          QBUTTON_SENDMESSAGE_EX(iButton, BUTTON_ACTION_TYPE_SGLCLICK, _button_event PTR_OFFSETS, idx_bit);
        }
        /* 更新此按键的最近单击按下时刻值为0 */
        tm_1st_sgl_pushing[iButton] = 0;
      }
      else                                          /* 第iButton号按键松开 */
      {
        if(((long_press_det_config[bank] >> iButton_inbank) & (ButtonBank)1u) != 0)
        {                                           /* 需检测长按时 */
          if(act_type == BUTTON_ACTION_TYPE_LONG)
          { /* 最近为长按，判定为松开（结束长按动作） */
            UPDATE_ACTION_TYPE(button_action_type[iButton >>2], idx_bit, BUTTON_ACTION_TYPE_RELEASINGUP);
            /* 构造iButton号按键松开消息，并将其入队列 */
            QBUTTON_SENDMESSAGE_EX(iButton, BUTTON_ACTION_TYPE_RELEASINGUP, _button_event PTR_OFFSETS, idx_bit);
          }
        }
  #ifndef STRICT_EXT_DET_CONFIG
        else if(((dbl_click_det_config[bank] >> iButton_inbank) & (ButtonBank)1u) == 0)
        { /* 不需检测该按键的长按和双击时，判定为松开，发送iButton号按键松开消息
           注：此为按键动作检测配置字不合理的防护性处理，若能遵Button_Module.h所述配置，删除此else if */
          UPDATE_ACTION_TYPE(button_action_type[iButton >>2], idx_bit, BUTTON_ACTION_TYPE_RELEASINGUP);
          QBUTTON_SENDMESSAGE_EX(iButton, BUTTON_ACTION_TYPE_RELEASINGUP, _button_event PTR_OFFSETS, idx_bit);
        }
  #endif /* STRICT_EXT_DET_CONFIG */
      }
    }
    else                                            /* 第iButton号按键无动作 */
    {
      if((act_type == BUTTON_ACTION_TYPE_SGLCLICK)
  #if LONG_PRESS_FR_DBLCLICK_SUPPORT == 1
         || (act_type == BUTTON_ACTION_TYPE_DBLCLICK)
  #endif /* LONG_PRESS_FR_DBLCLICK_SUPPORT */
                                                     )
      { /* 第iButton号按键最近为单击（或双击） */
        if(tm_1st_sgl_pushing[iButton] > BUTTON_LONGPRESS_HOLDTIME)
        { /* 且已过了2s（长按按下保持时长阈值）*/
          if(((long_press_det_config[bank] >> iButton_inbank) & (ButtonBank)1u) != 0)  /* 需检测长按时 */
          {
            if(((button_status_cur >> iButton_inbank) & (ButtonBank)1u) == (ButtonBank)BUTTON_ON)
            { /* 第iButton号按键当前保持按下，判定为长按 */
              UPDATE_ACTION_TYPE(button_action_type[iButton >>2], idx_bit, BUTTON_ACTION_TYPE_LONG);
              /* 构造iButton号按键长按消息，并将其入队列 */
              QBUTTON_SENDMESSAGE_EX(iButton, BUTTON_ACTION_TYPE_LONG, _button_event PTR_OFFSETS, idx_bit);

  #if BUTTON_EQUIV_SGLCLICK_INTERVAL_MS > 0
              tm_1st_sgl_pushing[iButton] = 0;
  #endif /* BUTTON_EQUIV_SGLCLICK_INTERVAL_MS */
            }
            else
            { /* 期间未有长按，将该按键动作类型更新为松开 */
              UPDATE_ACTION_TYPE(button_action_type[iButton >>2], idx_bit, BUTTON_ACTION_TYPE_RELEASINGUP);
            }
          }
          else                                      /* 不需检测长按时 */
          { /* 将该按键动作类型更新为松开 */
            UPDATE_ACTION_TYPE(button_action_type[iButton >>2], idx_bit, BUTTON_ACTION_TYPE_RELEASINGUP);
          }
        }
  #if SGL_CLICK_MSG_GEN_METHOD == 1
        else if((tm_1st_sgl_pushing[iButton] == BUTTON_DBLCLICK_INTERVAL)
  #if LONG_PRESS_FR_DBLCLICK_SUPPORT == 1
                && (act_type == BUTTON_ACTION_TYPE_SGLCLICK)
  #endif /* LONG_PRESS_FR_DBLCLICK_SUPPORT */
                                                            )
        { /* 且已过了0.4s（双击时的连续两次单击时间间隔阈值）*/
          if(((dbl_click_det_config[bank] >> iButton_inbank) & (ButtonBank)1u) != 0)
          { /* 需检测双击时，构造iButton号按键单击消息，并将其入队列 */
            QBUTTON_SENDMESSAGE_EX(iButton, BUTTON_ACTION_TYPE_SGLCLICK, _button_event PTR_OFFSETS, idx_bit);
          }
        }
  #endif /* SGL_CLICK_MSG_GEN_METHOD */
      }
  #if BUTTON_EQUIV_SGLCLICK_INTERVAL_MS > 0
      else
      {
        if(((equiv_sglclick_gen_config[bank] >> iButton_inbank) & (ButtonBank)1u) != 0)
        { /* 需生成第iButton号按键等效单击消息时 */
          if(act_type == BUTTON_ACTION_TYPE_LONG)
          { /* 第iButton号按键最近为长按 */
            if(tm_1st_sgl_pushing[iButton] >= BUTTON_EQUIV_SGLCLICK_INTERVAL)
            {
              tm_1st_sgl_pushing[iButton] = 0;
            }
            if(tm_1st_sgl_pushing[iButton] == 0)
            { /* 构造iButton号按键单击消息，并将其入队列 */
              QBUTTON_SENDMESSAGE_EX(iButton, BUTTON_ACTION_TYPE_SGLCLICK, _button_event PTR_OFFSETS, idx_bit);
            }
          }
        }
      }
  #endif /* BUTTON_EQUIV_SGLCLICK_INTERVAL_MS */
    }
    /* 切换为下一个按键，并计算其在本按键页中的页内编号(页内偏移号) */
    iButton++;
    iButton_inbank = iButton & ~(0xFFu << NUMPOW_BUTTONS_PER_BANK);
  }
#endif /* NUM_BUTTONS_EX_ACT */

  /* 提前将本按键页各按键的上一状态，更新为当前状态。bank变量后续可作其它用途 */
  button_status_prev[bank] = button_status_cur;

/* 以下if为：只需检测基本动作的按键处理 ...... */

  /* 有按键动作时，搜索有动作的按键，并判断其是什么动作 */
  act_flags &= (ButtonBank)(-1) << iButton_inbank;
  if(act_flags != 0)
  {
    while(iButton < iButtonEndPlus1)
    {
      if(((act_flags >> iButton_inbank) & (ButtonBank)1u) != 0)  /* 第iButton号按键有动作 */
      { /* 构造第iButton号按键按下/松开消息，并将其入队列 */
        bank  = iButton;
        bank |= (((button_status_cur >> iButton_inbank) & (ButtonBank)1u) == (ButtonBank)BUTTON_ON)?
                BUTTON_ACTION_TYPE_PUSHINGDOWN << MSG_BUTTON_ACTION_TYPE_MASK_SHIFTS:
                BUTTON_ACTION_TYPE_RELEASINGUP << MSG_BUTTON_ACTION_TYPE_MASK_SHIFTS;
        QBUTTON_SENDMESSAGE(bank, _button_event PTR_OFFSETS);
      }
      /* 切换为下一个按键，并计算其在本按键页中的页内编号（页内偏移号）*/
      iButton++;
      iButton_inbank = iButton & ~(0xFFu << NUMPOW_BUTTONS_PER_BANK);
    }
  }
}
#endif /* NUMPOW_BUTTONS_PER_BANK */


/* 仅当全局使能了生成长按期间的等效连续单击消息时，定义实现以下两个函数 */
#if BUTTON_EQUIV_SGLCLICK_INTERVAL_MS > 0

/*******************************************************************************
*  函数名：Button_Module_SetEquivSglclickGen
********************************************************************************
*
*  【功    能】
*    设置单个按键、单个按键页、所有按键在长按期间是否生成等效连续单击消息（约定各
*    按键向b0方向排列）
*
*  【入口参数】
*    which：按键不分页时
*             按键编号0 ~ NUM_BUTTONS-1，适用于设置单个按键
*             0xFF，适用于设置所有按键
*           按键分页时
*             按键编号0 ~ NUM_BUTTONS-1，适用于设置单个按键
*             0x80 + “按键页编号0 ~ NUM_BUTTON_BANKS-1”，适用于设置单个按键页
*    val:   设置单个按键时
*             0，不生成等效单击消息
*             1，生成等效单击消息
*           设置所有按键、单个按键页时
*             各按键的配置位值组合
*
*  【返 回 值】
*    无
*
*******************************************************************************/
void Button_Module_SetEquivSglclickGen(uint8 which, ButtonBank val)
{
#if NUMPOW_BUTTONS_PER_BANK < 0     /* 按键不分页时 */
  if(which == 0xFFu)
  { /* 所有按键 */
    equiv_sglclick_gen_config = val;
  }
  else if(which < NUM_BUTTONS)
  { /* 单个按键 */
    if(val == (ButtonBank)0)
    {
      equiv_sglclick_gen_config &= ~((ButtonBank)1u << which);
    }
    else
    {
      equiv_sglclick_gen_config |= (ButtonBank)1u << which;
    }
  }
#else                               /* 按键分页时 */
  if(which < NUM_BUTTONS)
  { /* 单个按键 */
    if(val == (ButtonBank)0)
    {
      val = (ButtonBank)(which & ~(0xFFu << NUMPOW_BUTTONS_PER_BANK));
      which >>= NUMPOW_BUTTONS_PER_BANK;
      equiv_sglclick_gen_config[which] &= ~((ButtonBank)1u << val);
    }
    else
    {
      val = (ButtonBank)(which & ~(0xFFu << NUMPOW_BUTTONS_PER_BANK));
      which >>= NUMPOW_BUTTONS_PER_BANK;
      equiv_sglclick_gen_config[which] |= (ButtonBank)1u << val;
    }
  }
  else if(which > 0x7Fu)
  { /* 单个按键页 */
    which &= 0x7Fu;
    if(which < NUM_BUTTON_BANKS)
    {
      equiv_sglclick_gen_config[which] = val;
    }
  }
#endif /* NUMPOW_BUTTONS_PER_BANK */
}


/*******************************************************************************
*  函数名：Button_Module_GetEquivSglclickGen
********************************************************************************
*
*  【功    能】
*    获取单个按键、单个按键页、所有按键在长按期间是否生成等效连续单击消息（约定各
*    按键向b0方向排列）
*
*  【入口参数】
*    which：按键不分页时
*             按键编号0 ~ NUM_BUTTONS-1，适用于获取单个按键的配置
*             0xFF，适用于获取所有按键的配置
*           按键分页时
*             按键编号0 ~ NUM_BUTTONS-1，适用于获取单个按键的配置
*             0x80 +“按键页编号0 ~ NUM_BUTTON_BANKS-1”，适用于获取单个按键页配置
*
*  【返 回 值】
*    当前的配置值：单个按键时，为0或1
*                  所有按键或单个按键页时，每位对应一个按键的配置值
*
*******************************************************************************/
ButtonBank Button_Module_GetEquivSglclickGen(uint8 which)
{
  ButtonBank val = (ButtonBank)0;

#if NUMPOW_BUTTONS_PER_BANK < 0     /* 按键不分页时 */
  if(which == 0xFFu)
  { /* 所有按键 */
    val = equiv_sglclick_gen_config;
  }
  else if(which < NUM_BUTTONS)
  { /* 单个按键 */
    val = (equiv_sglclick_gen_config >> which) & (ButtonBank)1u;
  }
#else                               /* 按键分页时 */
  if(which < NUM_BUTTONS)
  { /* 单个按键 */
    val = (ButtonBank)(which & ~(0xFFu << NUMPOW_BUTTONS_PER_BANK));
    which >>= NUMPOW_BUTTONS_PER_BANK;
    val = (equiv_sglclick_gen_config[which] >> val) & (ButtonBank)1u;
  }
  else if(which > 0x7Fu)
  { /* 单个按键页 */
    which &= 0x7Fu;
    if(which < NUM_BUTTON_BANKS)
    {
      val = equiv_sglclick_gen_config[which];
    }
  }
#endif /* NUMPOW_BUTTONS_PER_BANK */

  return val;
}

#endif /* BUTTON_EQUIV_SGLCLICK_INTERVAL_MS */


/* 仅不使用OS消息队列服务时，定义实现以下两个消息收发函数 */
#if PORTING_TO_OS_Q == NON_OS

/*******************************************************************************
*  函数名：_Button_Module_GetMessage
********************************************************************************
*
*  【功    能】
*    从队首获取一条消息
*
*  【入口参数】
*    msg_pvar：不使用，为兼容本函数的OS消息队列版而设置
*
*  【返 回 值】
*    0xFF：未获取到消息
*    其它：获取到的消息
*
*  注：可选内置的临界代码保护功能
*******************************************************************************/
uint8 _Button_Module_GetMessage(uint8 *msg_pvar)
{
  uint8 msg;
#ifndef NULL_MCR_CRITICAL_SEC
  uint32 savedIntrStatus = ENT_CRITICAL_SEC();
#endif /* NULL_MCR_CRITICAL_SEC */
  
#if PORTING_TO_OS_Q == NON_OS
  (void)msg_pvar;
#endif /* PORTING_TO_OS_Q */

  if(qbutton_front == qbutton_rear)
  {
#ifndef NULL_MCR_CRITICAL_SEC
    EXT_CRITICAL_SEC(savedIntrStatus);
#endif /* NULL_MCR_CRITICAL_SEC */
    return 0xFFu;
  }
  else
  {
    msg = qbutton_buff[qbutton_front];
    qbutton_front = (qbutton_front < NUM_QELEMS - 1u)? qbutton_front + 1u : 0;
#ifndef NULL_MCR_CRITICAL_SEC
    EXT_CRITICAL_SEC(savedIntrStatus);
#endif /* NULL_MCR_CRITICAL_SEC */
    return msg;
  }
}


/*******************************************************************************
*  函数名：_Button_Module_SendMessage
********************************************************************************
*
*  【功    能】
*    向队尾添加一条消息
*
*  【入口参数】
*    button_msg：消息
*    msg_pvar：  不使用，为兼容本函数的OS消息队列版而设置
*
*  【返 回 值】
*    1：入队列成功
*    0：入队列失败（因队列已满）
*
*  注：可选内置的临界代码保护功能
*******************************************************************************/
uint8 _Button_Module_SendMessage(uint8 button_msg, uint8 *msg_pvar)
{
#ifndef NULL_MCR_CRITICAL_SEC
  uint32 savedIntrStatus = ENT_CRITICAL_SEC();
#endif /* NULL_MCR_CRITICAL_SEC */

#if PORTING_TO_OS_Q == NON_OS
  (void)msg_pvar;
#endif /* PORTING_TO_OS_Q */

  if(qbutton_front == ((qbutton_rear < NUM_QELEMS - 1u)? qbutton_rear + 1u : 0))
  {
#ifndef NULL_MCR_CRITICAL_SEC
    EXT_CRITICAL_SEC(savedIntrStatus);
#endif /* NULL_MCR_CRITICAL_SEC */
    return 0;
  }
  else
  {
    qbutton_buff[qbutton_rear] = button_msg;
    qbutton_rear = (qbutton_rear < NUM_QELEMS - 1u)? qbutton_rear + 1u : 0;
#ifndef NULL_MCR_CRITICAL_SEC
    EXT_CRITICAL_SEC(savedIntrStatus);
#endif /* NULL_MCR_CRITICAL_SEC */
    return 1u;
  }
}

#endif /* PORTING_TO_OS_Q */

/* [] END OF FILE */
