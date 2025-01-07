/*******************************************************************************
* 文件名: MatrixKbLED.c  
* 版  本：1.0
* 日  期：2021-12-24
* 作  者：Inhaul Hsu
*
*    矩阵键盘/LED API函数、按键消息循环队列API函数
*
*   【按键消息格式】： [类型标志(b7位)] [按键符号(b6~b0位)]
*                       0——按下          keysym_table[][]值
*                       1——松开
*     旋转编码开关专用  0——CW顺时针旋转    'o'
*     旋转编码开关专用  1——CCW逆时针旋转   'o'
*
*******************************************************************************/
#include "project.h"

/* io_reg_bak 仅用于与移植相关的宏函数 */
#if(PORTING_TO_CY8CKIT == 1)              // PSoC 5LP处理器、GCC编译器时
static uint8 io_reg_bak;
#elif(PORTING_TO_CY8CKIT == 2)            // PSoC 6处理器、GCC编译器时
static uint32 io_reg_bak;
#endif

/* --------------- LED状态映射，位值为1/0表示该LED灭/亮 -----------------
   第0~3(行)元素值为 b[7..0]=[x, x,  x,  x,  COL3,COL2,COL1,COL0]
   第4(行)元素值为   b[7..0]=[x,RA2,RA1,RA0, COL3,COL2,COL1,COL0]
   ______________________________________________________________________
   |       b7   b6   b5   b4  b3        b2         b1         b0        |
   ----------------------------------------------------------------------
   | [4]:  x,  D22, D21, D20, D19,      D18,       D17,       D16       |
   | [3]:  x,   x,   x,   x,  D13_S15B, D12R_S14B, D12G_S13B, D12B_S12B | NUM_LEDROWS-1
   | [2]:  x,   x,   x,   x,  D11_S11B, D10_S10B,  D9_S9B,    D8_S8B    | .
   | [1]:  x,   x,   x,   x,  D7_S7B,   D6_S6B,    D5_S5B,    D4_S4B    | .
   | [0]:  x,   x,   x,   x,  D3_S3B,   D2_S2B,    D1_S1B,    D0_S0B    | 0
   ----------------------------------------------------------------------
                                           (NUM_LEDCOLS-1) .. 0
*/
uint8 ledstatus_map[5] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF};


/* --------------- 按键状态映射，位值为1/0表示该键松开/按下 ---------------
   第0~3(行)元素值为 b[7..0]=[x,   x,  x,  x,  COL3,COL2,COL1,COL0]
   第4(行)元素值为   b[7..0]=[EN, RA2,RA1,RA0, COL3,COL2,COL1,COL0]
   ________________________________________________________________________
   |         b7      b6      b5      b4     b3     b2      b1      b0     |
   ------------------------------------------------------------------------
   | [4]:  S16_Dwn,S16_Ctr,S16_CHB,S16_CHA, S17, S16_Left,S16_Up,S16_Rght |
   | [3]:    x,      x,      x,      x,     S15,   S14,    S13,    S12    | NUM_KEYROWS-1
   | [2]:    x,      x,      x,      x,     S11,   S10,    S9,     S8     | .
   | [1]:    x,      x,      x,      x,     S7,    S6,     S5,     S4     | .
   | [0]:    x,      x,      x,      x,     S3,    S2,     S1,     S0     | 0
   ------------------------------------------------------------------------
                                                (NUM_KEYCOLS-1) .. 0
*/
static uint8 keystatus_map_prev[5] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF};


/* ----- 按键行列号至按键符号查找表，第4行为五向旋转编码开关各键 +S17 ------
   _________________________________________________________________________
   |         [][7]   [][6]   [][5]   [][4]   [][3]  [][2]   [][1]   [][0]  |
   -------------------------------------------------------------------------
   | [4][]: S16_Dwn,S16_Ctr,S16_CHB,S16_CHA, S17, S16_Left,S16_Up,S16_Rght |
   | [3][]:   x,      x,      x,      x,     S15,   S14,    S13,    S12    | NUM_KEYROWS-1
   | [2][]:   x,      x,      x,      x,     S11,   S10,    S9,     S8     | .
   | [1][]:   x,      x,      x,      x,     S7,    S6,     S5,     S4     | .
   | [0][]:   x,      x,      x,      x,     S3,    S2,     S1,     S0     | 0
   -------------------------------------------------------------------------
                                                 (NUM_KEYCOLS-1) .. 0
*/
#if(CUST_KEYSYM_TBL == 0)
const char8 keysym_table[5][8]={{'0', '1', '2', '3', 'x', 'x', 'x', 'x'}, 
                                {'4', '5', '6', '7', 'x', 'x', 'x', 'x'}, 
                                {'8', '9', 'A', 'B', 'x', 'x', 'x', 'x'}, 
                                {'C', 'D', 'E', 'F', 'x', 'x', 'x', 'x'},
                                {'>', '^', '<', 'S', '-', '=', '+', 'v'}};
#else
extern const char8 keysym_table[5][8];
#endif

static uint8 rotary_action = 0x00; // 标记旋转编码开关旋转动作 b0——CW, b1——CCW，sticky bits

static uint8 iRow = 8;
static uint8 initvar;         // 用于获得CHB, CHA开关初始状态，避免启动时产生此开关消息

#if(MESSAGE_GEN == 1)
static uint8 qbuff[QSIZE];
static QUEUE Qkey;

// Qkey_FeedData()函数的内联版，仅于本文件中可见
#define QKEY_FEEDDATA(dat)    do{if(Qkey.front != (Qkey.rear +1) % Qkey.maxsize){ \
                                   Qkey.pBase[Qkey.rear] = (dat); \
                                   Qkey.rear = (Qkey.rear + 1) % Qkey.maxsize;}\
                              }while(0)
static void  Qkey_Init(void);
static uint8 Qkey_IsFull(void);
static uint8 Qkey_IsEmpty(void);
#endif

static void MatrixKbLED_Handle(void);


#if(PORTING_TO_CY8CKIT == 1)              // PSoC 5LP处理器、GCC编译器时
CY_ISR(MyISR)
{
    /*  Place your Interrupt code here. */
    /* `#START MatrixKbLED_Intc` */

    /* `#END` */

    MatrixKbLED_Handle();
}
#elif(PORTING_TO_CY8CKIT == 2)            // PSoC 6处理器、GCC编译器时
/*******************************************************************************
*  函数名：MatrixKbLED_SysInt_Tmr_MKbLED_handler
********************************************************************************
*
*  【功    能】
*    矩阵键盘/LED 定时器中断服务程序
*    每发生一次计数值上溢/下溢时，调用一次MatrixKbLED_Handle()。
*    用户可在两处`#START` ～ `#END`注释之间，添加附加代码，以充分利用此定时器中断。
*
*  【入口参数】
*    无
*
*  【返 回 值】
*    无
*
*******************************************************************************/
void MatrixKbLED_SysInt_Tmr_MKbLED_handler(void)
{
    /***************************************************************************
     *  Place your additional codes
     **************************************************************************/
    /* `#START MatrixKbLED_SysInt_Tmr_MKbLED_handler_header` */

    /* `#END` */

    if((Cy_TCPWM_GetInterruptStatus(MatrixKbLED_Timer_MKbLED_HW,   \
            MatrixKbLED_Timer_MKbLED_CNT_NUM) & CY_TCPWM_INT_ON_TC) != 0)
    {
        Cy_TCPWM_ClearInterrupt(MatrixKbLED_Timer_MKbLED_HW,       \
                                MatrixKbLED_Timer_MKbLED_CNT_NUM, CY_TCPWM_INT_ON_TC);
        MatrixKbLED_Handle();

        /***************************************************************************
         *  Place your additional codes
         **************************************************************************/
        /* `#START MatrixKbLED_SysInt_Tmr_MKbLED_handler_TC` */

        /* `#END` */
    }
}
#endif


#if(PORTING_TO_CY8CKIT == 1)              // PSoC 5LP处理器、GCC编译器时
/*******************************************************************************
*  函数名：MatrixKbLED_Start
********************************************************************************
*
*  【功    能】
*    矩阵键盘/LED初始化
*
*  【入口参数】
*    无
*
*  【返 回 值】
*    无
*
*******************************************************************************/
void MatrixKbLED_Start(void)
{
    EN_WRITE(0);
    EN_SETDRIVEMODE(MatrixKbLED_EN_DM_STRONG);
    RA_WRITE(0xFF);
    RA_SETDRIVEMODE(MatrixKbLED_RA_DM_STRONG);
    COL_WRITE(0xFF);
    COL_SETDRIVEMODE(MatrixKbLED_COL_DM_RES_UP);

#if(MESSAGE_GEN == 1)
    Qkey_Init();
#endif
    MatrixKbLED_isr_MKbLED_StartEx(MyISR);
    iRow = 8;
    initvar = 0;
}
#elif(PORTING_TO_CY8CKIT == 2)            // PSoC 6处理器、GCC编译器时
/*******************************************************************************
*  函数名：MatrixKbLED_Start
********************************************************************************
*
*  【功    能】
*    矩阵键盘/LED初始化
*
*  【入口参数】
*    无
*
*  【返 回 值】
*    无
*
*******************************************************************************/
void MatrixKbLED_Start(void)
{
    /* 矩阵键盘/LED GPIO初始化 */
    cy_stc_gpio_pin_config_t pinConfig = {
        /*.outVal =*/ 0UL,                  /* Output = 0 */
        /*.driveMode =*/ CY_GPIO_DM_STRONG, /* Resistive pull-down, input buffer on */
        /*.hsiom =*/ 0UL,                   /* Software controlled pin */
        /*.intEdge =*/ CY_GPIO_INTR_DISABLE,/* Disable pin interrupt */
        /*.intMask =*/ 0UL,                 /* Disable port interrupt for this pin */
        /*.vtrip =*/ CY_GPIO_VTRIP_CMOS,    /* CMOS voltage trip */
        /*.slewRate =*/ CY_GPIO_SLEW_FAST,  /* Fast slew rate */
        /*.driveSel =*/ CY_GPIO_DRIVE_FULL, /* Full drive strength */
        /*.vregEn =*/ 0UL,                  /* SIO-specific setting - ignored */
        /*.ibufMode =*/ 0UL,                /* SIO-specific setting - ignored */
        /*.vtripSel =*/ 0UL,                /* SIO-specific setting - ignored */
        /*.vrefSel =*/ 0UL,                 /* SIO-specific setting - ignored */
        /*.vohSel =*/ 0UL                   /* SIO-specific setting - ignored */
    };
    
    Cy_GPIO_Pin_Init(GPIO_PRT1, 5, &pinConfig);  // EN（P1.5，初始化为GPIO、强驱动、输出0）
    
    pinConfig.outVal = 1UL;
    Cy_GPIO_Pin_Init(GPIO_PRT12, 6, &pinConfig); // RA0（P12.6，初始化为GPIO、强驱动、输出1）
    Cy_GPIO_Pin_Init(GPIO_PRT12, 7, &pinConfig); // RA1（P12.7，初始化为GPIO、强驱动、输出1）
    Cy_GPIO_Pin_Init(GPIO_PRT12, 4, &pinConfig); // RA2（P12.4，初始化为GPIO、强驱动、输出1）

    pinConfig.driveMode = CY_GPIO_DM_PULLUP;
    Cy_GPIO_Pin_Init(GPIO_PRT1, 2, &pinConfig);  // COL0（P1.2，初始化为GPIO、电阻上拉、输出1）
    Cy_GPIO_Pin_Init(GPIO_PRT1, 3, &pinConfig);  // COL1（P1.3，初始化为GPIO、电阻上拉、输出1）
    Cy_GPIO_Pin_Init(GPIO_PRT1, 4, &pinConfig);  // COL2（P1.4，初始化为GPIO、电阻上拉、输出1）
    Cy_GPIO_Pin_Init(GPIO_PRT12, 5, &pinConfig); // COL3（P12.5，初始化为GPIO、电阻上拉、输出1）
    
    
    /* 矩阵键盘/LED 定时器及其中断初始化 */
    const cy_stc_sysint_t MatrixKbLED_SysInt_Tmr_MKbLED_cfg = {
        .intrSrc = (IRQn_Type)MatrixKbLED_SysInt_Tmr_MKbLED__INTC_NUMBER,
        .intrPriority = MatrixKbLED_SysInt_Tmr_MKbLED__INTC_CORTEXM4_PRIORITY
    };
    
    Cy_SysInt_Init(&MatrixKbLED_SysInt_Tmr_MKbLED_cfg, MatrixKbLED_SysInt_Tmr_MKbLED_handler);
    NVIC_ClearPendingIRQ(MatrixKbLED_SysInt_Tmr_MKbLED_cfg.intrSrc);
    NVIC_EnableIRQ(MatrixKbLED_SysInt_Tmr_MKbLED_cfg.intrSrc);

    MatrixKbLED_Timer_MKbLED_Start();
    
#if(SCANS_PER_ROUND == 10)
    Cy_TCPWM_Counter_SetPeriod(MatrixKbLED_Timer_MKbLED_HW,      \
                               MatrixKbLED_Timer_MKbLED_CNT_NUM, 1000000/500ul -1ul);
#elif(SCANS_PER_ROUND == 6)
    Cy_TCPWM_Counter_SetPeriod(MatrixKbLED_Timer_MKbLED_HW,      \
                               MatrixKbLED_Timer_MKbLED_CNT_NUM, 1000000/303ul -1ul);
#endif
    
    /* 按键消息循环队列等其它初始化 */
#if(MESSAGE_GEN == 1)
    Qkey_Init();
#endif
    iRow = 8;
    initvar = 0;
}
#endif


/*******************************************************************************
*  函数名：MatrixKbLED_Handle
********************************************************************************
*
*  【功    能】
*    矩阵键盘采样、按键动作识别、按键消息入队列，矩阵LED显示刷新
*        本函数每次扫描若干行按键状态和/或刷新显示一行LED，可配置为每轮6次扫描
*    （当NUM_LEDROWS=4时），或每轮10次扫描（当NUM_KEYROWS=NUM_LEDROWS=4时）。
*    故若20ms完成一轮扫描，则当NUM_KEYROWS=NUM_LEDROWS=4时，应每隔约3.3ms、2ms
*   （分别对应每轮6、10次扫描）调用一次本函数。
*        宏SCANS_PER_ROUND配置每轮所用的扫描次数，此值越小，则本函数最大执行时间
*    越长。行数设置与上所述不同时，SCANS_PER_ROUND宏值无实际意义，仅作三种不同扫
*    描方法的标示。实际的每轮扫描次数为：
*        ----------------------------------------------------------
*         NUM_LEDROWS\SCANS_PER_ROUND |   6            10
*        ----------------------------------------------------------
*             4                       |   6     3+ NUM_KEYROWS +3
*             3                       |   5     3+ NUM_KEYROWS +2
*             2                       |   4     3+ NUM_KEYROWS +1
*             1                       |   3     3+ NUM_KEYROWS
*             0                       |   3     3+ NUM_KEYROWS
*        ----------------------------------------------------------
*        可根据实际的每轮扫描次数，算得本函数应被调用的时间间隔。
*
*
*   扫描处理时行号与各按键/LED的对应关系（花括号之间为一次扫描）：
*   SCANS_PER_ROUND=10时
*   --------------------------------------------------------------------------------------
*   行号iRow:   8    9(I/O设置准备)    10   0   1    2    {  3      4  }   5     6     7  
*   --------------------------------------------------------------------------------------
*   按键/LED：D22~16 五向旋转编码开关+S17 S3~0 S7~4 S11~8 {S15~12 R4LED} R5LED R6LED R7LED
*   --------------------------------------------------------------------------------------
*
*   SCANS_PER_ROUND=6时
*   --------------------------------------------------------------------------------------
*   行号iRow:   8    9(I/O设置准备)  { 10   0    1    2     3      4   }   5     6     7  
*   --------------------------------------------------------------------------------------
*   按键/LED：D22~16 {五向旋转编码开关+S17 S3~0 S7~4 S11~8 S15~12 R4LED} R5LED R6LED R7LED
*   --------------------------------------------------------------------------------------
*
*
*        扫描耗时测试（PSoC 5LP, fCPU=63.429M, 各行/列设置均为4，GCC速度优化，示波器测量）：
*        SCANS_PER_ROUND=10时
*        -----------------------------------------------------------
*        ANO R10行，无按键动作                    4.5us             
*        ANO R10行，1按键动作                     8.2us(520 clks)
*        ANO R10行，2按键动作                     8.7us(552 clks) 
*
*        R0行，无按键动作                         2.4us
*        R0行，1按键动作                          4.7us
*        R0行，2按键动作                          5.2us
*        合并扫描R4LED阶段                       +0.8us
*        -----------------------------------------------------------
*
*        SCANS_PER_ROUND=6时
*        -----------------------------------------------------------
*        R10~R4LED，无按键动作                    11.0us
*        R10~R4LED，1按键动作(ANO)                15.7us(996 clks)
*        R10~R4LED，1按键动作(非ANO)              15.0us
*        R10~R4LED，2按键动作(1ANO+1非)           17.7us(1123 clks)
*        R10~R4LED，3按键动作(1ANO+2非(不同行))   19.2us
*        R10~R4LED，3按键动作(1ANO+2非(同行))     18.3us
*        -----------------------------------------------------------
*
*        * 使用QKEY_FEEDDATA()，比Qkey_FeedData()缩减耗时约1半
*
*
*  【入口参数】
*    无
*
*  【返 回 值】
*    无
*
*  【全局变量共享】
*    static uint8/uint32 io_reg_bak 仅用于与移植相关的宏函数
*    uint8 ledstatus_map[]：        用作LED显示缓冲区，本函数只读
*    uint8 keystatus_map_prev[]:    按键状态映射
*    const char8 keysym_table[][]： 按键行列号至按键符号查找表，用于发送按键消息
*    static uint8 rotary_action     标记旋转编码开关旋转动作，与函数MatrixKbLED_GetKey()共享
*    static uint8 iRow;             标记将或正在扫描的行号，与函数MatrixKbLED_Start()共享
*                                   MatrixKbLED_Start()设置其初值为8
*    static uint8 initvar;          用于获得CHB, CHA按键初始状态，避免启动时产生此按键消息
*                                   与MatrixKbLED_Start()共享，MatrixKbLED_Start()设其初值为0
*    static uint8 qbuff[QSIZE];     用于发送按键消息（按键消息循环队列）
*    static QUEUE Qkey;             用于发送按键消息（按键消息循环队列）
*
*******************************************************************************/
static void MatrixKbLED_Handle(void)
{
    uint8 iCol;
    uint8 keystatus_iRow_cur;
    uint8 byt, byt_bit_iCol;
    static uint8 cnt_sw_qd = 100;   // 用于旋转编码开关旋转动作识别的计数器

    
    /* 扫描若干行按键和/或一行LED，唯最末行按键与后续首行LED合并扫描 */
    switch(iRow)
    {
        case 8:
            /* 扫描R8LED行 */
            COL_WRITE(0xFF);
            RA_WRITE(0xFF);
            EN_WRITE(0);
            RA_WRITE(ledstatus_map[4]>>4);
            COL_WRITE(ledstatus_map[4]);
            iRow = 9;                             // 准备下一次扫描五向旋转编码开关+S17
            break;
            
        case 9:
            /* 扫描五向旋转编码开关+S17 —— 阶段1（I/O设置准备） */
            RA_SETDRIVEMODE(MatrixKbLED_RA_DM_DIG_HIZ);
            COL_SETDRIVEMODE(MatrixKbLED_COL_DM_OD_LO);
            COL_WRITE(0xFF);
            iRow = 10;
            break;

        case 10:
            /* 扫描五向旋转编码开关+S17 —— 阶段2 */
            keystatus_iRow_cur = RA_READ();       // 读Ctr按键, CHB, CHA开关状态
            if(initvar == 0)
            {                                     // 获得CHB, CHA开关初始状态，
                                                  // 避免启动时产生此按键消息
                keystatus_map_prev[4] &= 0xCF;
                keystatus_map_prev[4] |= (keystatus_iRow_cur &0x03)<<4;
                initvar = 1;
            }
            RA_WRITE(0xFF);
            RA_SETDRIVEMODE(MatrixKbLED_RA_DM_STRONG);
            keystatus_iRow_cur <<= 4;
            
            iCol = COL_READ();                    // 读S17, Left, Up, Right按键状态
            iCol = ~iCol;
            keystatus_iRow_cur += iCol&0x0F;
            EN_SETDRIVEMODE(MatrixKbLED_EN_DM_DIG_HIZ);    // 读Down按键状态
            iCol = EN_READ(); iCol = EN_READ();            // 提供少量的延时以满足EN建立时间
            iCol = EN_READ();
#if(NUM_KEYROWS != 0)
            iRow = 0;                             // 准备扫描第0行按键
            EN_WRITE(1);
            EN_SETDRIVEMODE(MatrixKbLED_EN_DM_STRONG);
            RA_WRITE(0x00);
            COL_WRITE(0xFF);
            COL_SETDRIVEMODE(MatrixKbLED_COL_DM_RES_UP);
#endif
            keystatus_iRow_cur = iCol == 0? (keystatus_iRow_cur | 0x80):(keystatus_iRow_cur & 0x7F);
#if(MESSAGE_GEN == 1)
            byt = keystatus_iRow_cur ^ keystatus_map_prev[4];
            byt &= 0xF0 + (0x0F>>(4-NUM_KEYCOLS));
            if(byt != 0x00)                                              // 该行有按键动作
            {
                for(iCol=0; iCol<8; iCol++)
                {
                    byt_bit_iCol = byt & (0x01<<iCol);
                    if(byt_bit_iCol != 0)                                // 第iCol列有按键动作
                    {
                        if((keystatus_iRow_cur & byt_bit_iCol) == 0x00)  // 第iCol列，按键按下
                        {
                            QKEY_FEEDDATA(keysym_table[4][iCol]);
                            
                            /* 旋转编码开关旋转动作识别 —— 末段 */
                            if((iCol == 4) || (iCol == 5))
                            {
                                if(cnt_sw_qd > 10)      // CHA、CHB两开关同时断开超过20ms
                                {                       // 否则视为抖动，重新查找两开关同时断开状态
                                    
                                    if(iCol == 4)       // CW 顺时针旋转
                                    {
                                        QKEY_FEEDDATA('o');
                                        rotary_action |= 0x01;
                                    }
                                    else                // CCW 逆时针旋转
                                    {
                                        QKEY_FEEDDATA((uint8)'o'|0x80);
                                        rotary_action |= 0x02;
                                    }
                                }
                                cnt_sw_qd = 0;
                            }
                        }
                        else                                             // 第iCol列，按键松开
                        {
                            QKEY_FEEDDATA(keysym_table[4][iCol]|0x80);
                            
                            /* 旋转编码开关旋转动作识别 —— 前段 */
                            if((iCol == 4) || (iCol == 5))
                            {
                                // 查找CHA、CHB两开关同时断开状态
                                if((cnt_sw_qd == 0) || (cnt_sw_qd == 1))
                                    cnt_sw_qd ++;
                                else
                                    cnt_sw_qd = 1;
                            }
                        }
                    }
                }
            }
#else
            byt = keystatus_iRow_cur ^ keystatus_map_prev[4];
            for(byt_bit_iCol=0x10; byt_bit_iCol<=0x20; byt_bit_iCol<<=1)
            {
                if((byt & byt_bit_iCol) != 0)                            // CHA/CHB有动作
                {
                    if((keystatus_iRow_cur & byt_bit_iCol) == 0x00)      // CHA/CHB闭合
                    {
                        /* 旋转编码开关旋转动作识别 —— 末段 */
                        if(cnt_sw_qd > 10)      // CHA、CHB两开关同时断开超过20ms
                        {                       // 否则视为抖动，重新查找两开关同时断开状态
                            rotary_action |= byt_bit_iCol >>4;
                        }
                        cnt_sw_qd = 0;
                    }
                    else                                                 // CHA/CHB断开
                    {
                        /* 旋转编码开关旋转动作识别 —— 前段：查找CHA、CHB两开关同时断开状态 */
                        ((cnt_sw_qd == 0) || (cnt_sw_qd == 1))? cnt_sw_qd++ : (cnt_sw_qd =1);
                    }
                }
            }
#endif
            keystatus_map_prev[4] = keystatus_iRow_cur;
#if(NUM_KEYROWS != 0)
  #if(SCANS_PER_ROUND == 6)
            COL_SETDRIVEMODE(MatrixKbLED_COL_DM_STRONG);    //! 加速驱动至高电平（相比电阻上拉时），
                                                            // 否则可能不能连续扫描所有按键行
  #elif(SCANS_PER_ROUND == 10)
            break;
  #endif
#else
  #if(NUM_LEDROWS >= 1)                             /* 特殊参数处理 */
            EN_WRITE(1);
            EN_SETDRIVEMODE(MatrixKbLED_EN_DM_STRONG);
            RA_WRITE(4);
            COL_SETDRIVEMODE(MatrixKbLED_COL_DM_RES_UP);
            COL_WRITE(ledstatus_map[0]);
            iRow = NUM_LEDROWS>=2? 5:8;
            break;
  #elif(NUM_LEDROWS == 0)                           /* 特殊参数处理 */
            EN_SETDRIVEMODE(MatrixKbLED_EN_DM_STRONG);
            COL_SETDRIVEMODE(MatrixKbLED_COL_DM_RES_UP);
            iRow = 8;
            break;
  #endif
#endif

        case 0:
        case 1:
        case 2:
        case 3:
#if(SCANS_PER_ROUND == 10)
            /* 扫描此有效按键行 */
            if(iRow < NUM_KEYROWS)
#elif(SCANS_PER_ROUND == 6)
            /* 扫描所有有效按键行及首个有效LED行 */
            while(iRow < NUM_KEYROWS)
#endif
            {
                COL_SETDRIVEMODE(MatrixKbLED_COL_DM_RES_UP);
                keystatus_iRow_cur = COL_READ();                             // 读第iRow行的各列值
#if(MESSAGE_GEN == 1)
                byt = keystatus_iRow_cur ^ keystatus_map_prev[iRow];
                byt &= 0xFF>>(8-NUM_KEYCOLS);
                if(byt != 0x00)                                              // 第iRow行有按键动作
                {
                    for(iCol=0; iCol<NUM_KEYCOLS; iCol++)
                    {
                        byt_bit_iCol = byt & (0x01<<iCol);
                        if(byt_bit_iCol != 0)                                // 第iCol列有按键动作
                        {
                            if((keystatus_iRow_cur & byt_bit_iCol) == 0x00)  // 第iRow行iCol列，按键按下
                            {
                                QKEY_FEEDDATA(keysym_table[iRow][iCol]);
                                //ledstatus_map[iRow] &= ~(0x01<<iCol);        // 点亮该键所对应的LED
                            }
                            else                                             // 第iRow行iCol列，按键松开
                            {
                                QKEY_FEEDDATA(keysym_table[iRow][iCol]|0x80);
                                //ledstatus_map[iRow] |= 0x01<<iCol;           // 熄灭该键所对应的LED
                            }
                        }
                    }
                }
#endif
                keystatus_map_prev[iRow] = keystatus_iRow_cur;
                iRow ++;
                if(iRow < NUM_KEYROWS)        /* 下一行仍为有效按键行，切换RA地址，准备下一次按键行扫描 */
                {
#if(SCANS_PER_ROUND == 6)
                    COL_SETDRIVEMODE(MatrixKbLED_COL_DM_STRONG);  //! 加速驱动至高电平（相比电阻上拉时），
                                                                  // 否则可能不能连续扫描所有按键行
#endif
                    RA_WRITE(iRow);
                }
                else if(iRow == NUM_KEYROWS)  /* 按键行已扫描完 */
                {
                    if(NUM_LEDROWS >=1)           // 若有LED行，R4LED行显示刷新
                    {
                        RA_WRITE(4);
                        COL_WRITE(ledstatus_map[0]);
                    }
                    iRow = NUM_LEDROWS>=2? 5:8;   // 若有两个及以上LED行，准备下一次扫描R5LED行，
                                                  // 否则准备扫描R8LED行
                }
            }
            break;

        case 5:
        case 6:
        case 7:
            /* 扫描此有效LED行 */
            if(iRow < 4+NUM_LEDROWS)
            {
                COL_WRITE(0xFF);
                RA_WRITE(iRow);
                COL_WRITE(ledstatus_map[iRow-4]);
                iRow ++;
                iRow = iRow == 4+NUM_LEDROWS? 8: iRow; /* LED行已扫描完，准备下一次扫描R8LED行 */
            }
            break;
            
        default:
            break;
    }
    
    /* 旋转编码开关旋转动作识别 —— 中段 */
    if((cnt_sw_qd >= 2) && (cnt_sw_qd < 100))
    {                     // CHA、CHB两开关已同时断开，此后cnt_sw_qd计同时断开时长最多至200ms
        cnt_sw_qd ++;
    }
}


/*******************************************************************************
*  函数名：MatrixKbLED_SetLED
********************************************************************************
*
*  【功    能】
*    点亮或者熄灭若干个LED
*
*  【入口参数】
*    leds：指定要点亮或熄灭的LED，使用以下宏、或者多个宏的或：
*          LED_D0_S0B、LED_D1_S1B、LED_D2_S2B、LED_D3_S3B、LED_D4_S4B、LED_D5_S5B、
*          LED_D6_S6B、LED_D7_S7B、LED_D8_S8B、LED_D9_S9B、LED_D10_S10B、
*          LED_D11_S11B、LED_D12B_S12B、LED_D12G_S13B、LED_D12R_S14B、LED_D13_S15B、
*          LED_D16、LED_D17、LED_D18、LED_D19、LED_D20、LED_D21、LED_D22
*
*          使用宏LED_ALL时，操作以上所有LED
*
*    onoff：使用宏LEDON——点亮、宏LEDOFF——熄灭
*
*  【返 回 值】
*    无
*
*******************************************************************************/
void  MatrixKbLED_SetLED(uint32 leds, uint8 onoff)
{
    uint8 i;
    uint8 row, col;
    
    if(leds == LED_ALL)
    {
        for(i=0; i<5; i++)
        {
            ledstatus_map[i] = onoff == LEDON? 0x00:0xFF;
        }
    }
    else
    {
        for(i=0; i<MAXLEDNUM; i++)
        {
            if(leds == 0)
            {
                break;
            }
            if((leds & 0x00000001) !=0)
            {
                row = i/4;
                col = i%4;
                if((i >= 20) && (i <= 22))
                {
                    row = 4;
                    col += 4;
                }
                if(row < NUM_LEDROWS)
                {
                    if(col < NUM_LEDCOLS)
                    {
                        ledstatus_map[row] = onoff == LEDON? ledstatus_map[row] & ~(0x01<<col):\
                                                             ledstatus_map[row] | 0x01<<col;
                    }
                }
                else if(row == 4)
                {
                    if((col < NUM_LEDCOLS) || (col >= 4))
                    {
                        ledstatus_map[4] = onoff == LEDON? ledstatus_map[4] & ~(0x01<<col):\
                                                           ledstatus_map[4] | 0x01<<col;
                    }
                }
            }
            leds >>=1;
        }
    }
}


/*******************************************************************************
*  函数名：MatrixKbLED_SetLED_RC
********************************************************************************
*
*  【功    能】
*    点亮或者熄灭指定行列处的一个LED
*
*  【入口参数】
*    row、col：LED所在的行、列号，如下表所示排布
    ____________________________________________________________________
    |     7    6    5    4    3         2          1          0        | col
    --------------------------------------------------------------------
    | 4:  x,  D22, D21, D20, D19,      D18,       D17,       D16       |
    | 3:  x,   x,   x,   x,  D13_S15B, D12R_S14B, D12G_S13B, D12B_S12B | NUM_LEDROWS-1
    | 2:  x,   x,   x,   x,  D11_S11B, D10_S10B,  D9_S9B,    D8_S8B    | .
    | 1:  x,   x,   x,   x,  D7_S7B,   D6_S6B,    D5_S5B,    D4_S4B    | .
    | 0:  x,   x,   x,   x,  D3_S3B,   D2_S2B,    D1_S1B,    D0_S0B    | 0
    --------------------------------------------------------------------
     row                                   (NUM_LEDCOLS-1) .. 0

*    onoff：使用宏LEDON——点亮、宏LEDOFF——熄灭
*
*  【返 回 值】
*    无
*
*******************************************************************************/
void  MatrixKbLED_SetLED_RC(uint8 row, uint8 col, uint8 onoff)
{
    if(((row < NUM_LEDROWS) && (col < NUM_LEDCOLS)) || 
        ((row == 4) && ((col < NUM_LEDCOLS) || ((col >= 4) &&(col <= 6)))))
    {
        if(onoff == LEDON)
        {
            ledstatus_map[row] &= ~(0x01<<col);
        }
        else if(onoff == LEDOFF)
        {
            ledstatus_map[row] |= 0x01<<col;
        }
    }
}


/*******************************************************************************
*  函数名：MatrixKbLED_SetBarLEDLevel
********************************************************************************
*
*  【功    能】
*    设置LED灯条的指示级
*
*  【入口参数】
*    level：LED灯条的指示级，取值范围: 0(全灭) ~ LVLNUM_BARLED-1(全亮)
*
*  【返 回 值】
*    无
*
*******************************************************************************/
void  MatrixKbLED_SetBarLEDLevel(uint8 level)
{
    if(level < LVLNUM_BARLED)
    {
        if(level < 4)
        {
            ledstatus_map[4] &= (0x7F>>level);
            ledstatus_map[4] |= (0x7F>>level);
        }
#if(LVLNUM_BARLED >= 5)
        if((level >= 4) && (level <= 6))
        {
            ledstatus_map[4] &= (NUM_LEDCOLS==4)?((0x07>>(level -3))|0x88):
                                ((0x07>>(level-NUM_LEDCOLS))|
                                 0x80|((0xF8>>(3-NUM_LEDCOLS))&0x0F));
            ledstatus_map[4] |= (NUM_LEDCOLS==4)?((0x07>>(level -3))|0x88):
                                ((0x07>>(level-NUM_LEDCOLS))|
                                 0x80|((0xF8>>(3-NUM_LEDCOLS))&0x0F));
        }
#if(LVLNUM_BARLED == 8)
        if(level == 7)
        {
            ledstatus_map[4] &= 0x80;
            ledstatus_map[4] |= 0x80;
        }
#endif
#endif
    }
}


/*******************************************************************************
*  函数名：MatrixKbLED_GetLED
********************************************************************************
*
*  【功    能】
*    获取某个LED的当前状态
*
*  【入口参数】
*    led：指定要获取的LED，使用以下宏之一：
*         LED_D0_S0B、LED_D1_S1B、LED_D2_S2B、LED_D3_S3B、LED_D4_S4B、LED_D5_S5B、
*         LED_D6_S6B、LED_D7_S7B、LED_D8_S8B、LED_D9_S9B、LED_D10_S10B、
*         LED_D11_S11B、LED_D12B_S12B、LED_D12G_S13B、LED_D12R_S14B、LED_D13_S15B、
*         LED_D16、LED_D17、LED_D18、LED_D19、LED_D20、LED_D21、LED_D22
*
*  【返 回 值】
*    LEDON：亮
*    LEDOFF：灭
*    0xFF：参数错误
*
*******************************************************************************/
uint8 MatrixKbLED_GetLED(uint32 led)
{
    uint8 i;
    uint8 row, col;
    
    for(i=0; i<MAXLEDNUM; i++)
    {
        if(led == 0)
        {
            return 0xFF;
        }
        if((led & 0x00000001) !=0)
        {
            row = i/4;
            col = i%4;
            if((i >= 20) && (i <= 22))
            {
                row = 4;
                col += 4;
            }
            if(row < NUM_LEDROWS)
            {
                if(col < NUM_LEDCOLS)
                {
                    if((ledstatus_map[row] & (0x01<<col)) == 0)
                        return LEDON;
                    else
                        return LEDOFF;
                }
            }
            else if(row == 4)
            {
                if((col < NUM_LEDCOLS) || (col >= 4))
                {
                    if((ledstatus_map[row] & (0x01<<col)) == 0)
                        return LEDON;
                    else
                        return LEDOFF;
                }
            }
        }
        led >>=1;
    }
    return 0xFF;
}


/*******************************************************************************
*  函数名：MatrixKbLED_GetLED_RC
********************************************************************************
*
*  【功    能】
*    获取指定行列处的一个LED的当前状态
*
*  【入口参数】
*    row、col：LED所在的行、列号，如下表所示排布
    ____________________________________________________________________
    |     7    6    5    4    3         2          1          0        | col
    --------------------------------------------------------------------
    | 4:  x,  D22, D21, D20, D19,      D18,       D17,       D16       |
    | 3:  x,   x,   x,   x,  D13_S15B, D12R_S14B, D12G_S13B, D12B_S12B | NUM_LEDROWS-1
    | 2:  x,   x,   x,   x,  D11_S11B, D10_S10B,  D9_S9B,    D8_S8B    | .
    | 1:  x,   x,   x,   x,  D7_S7B,   D6_S6B,    D5_S5B,    D4_S4B    | .
    | 0:  x,   x,   x,   x,  D3_S3B,   D2_S2B,    D1_S1B,    D0_S0B    | 0
    --------------------------------------------------------------------
     row                                   (NUM_LEDCOLS-1) .. 0

*  【返 回 值】
*    LEDON：亮
*    LEDOFF：灭
*    0xFF：参数错误
*
*******************************************************************************/
uint8  MatrixKbLED_GetLED_RC(uint8 row, uint8 col)
{
    if(((row < NUM_LEDROWS) && (col < NUM_LEDCOLS)) || 
        ((row == 4) && ((col < NUM_LEDCOLS) || ((col >= 4) &&(col <= 6)))))
    {
        if((ledstatus_map[row] & (0x01<<col)) == 0)
            return LEDON;
        else
            return LEDOFF;
    }
    else
    {
        return 0xFF;
    }
}


/*******************************************************************************
*  函数名：MatrixKbLED_KeySym2RC
********************************************************************************
*
*  【功    能】
*    获取按键符号所在的行、列号
*
*  【入口参数】
*    keysym：按键符号（在按键行列号至按键符号查找表keysym_table[5][8]中的符号）
*
*  【返 回 值】
*    低4位：列号
*    高4位：行号
*    0xFF：参数错误
*
*        各键及其行、列号如下表所示排布：
    _____________________________________________________________________
    |      7       6       5       4       3      2       1       0     | col
    ---------------------------------------------------------------------
    | 4: S16_Dwn,S16_Ctr,S16_CHB,S16_CHA, S17, S16_Left,S16_Up,S16_Rght |
    | 3:   x,      x,      x,      x,     S15,   S14,    S13,    S12    | NUM_KEYROWS-1
    | 2:   x,      x,      x,      x,     S11,   S10,    S9,     S8     | .
    | 1:   x,      x,      x,      x,     S7,    S6,     S5,     S4     | .
    | 0:   x,      x,      x,      x,     S3,    S2,     S1,     S0     | 0
    ---------------------------------------------------------------------
     row                                       (NUM_KEYCOLS-1) .. 0

*******************************************************************************/
uint8 MatrixKbLED_KeySym2RC(uint8 keysym)
{
    uint8 ir, jc;
    
    keysym &= 0x7F;
    for(ir=0; ir<NUM_KEYROWS; ir++)
    {
        for(jc=0; jc<NUM_KEYCOLS; jc++)
        {
            if(keysym == keysym_table[ir][jc])
            {
                ir <<= 4;
                ir += jc;
                return ir;
            }
        }
    }
    for(jc=0; jc<8; jc++)
    {
        if(jc == NUM_KEYCOLS)
        {
            jc = 4;
        }
        if(keysym == keysym_table[4][jc])
        {
            return (0x40 +jc);
        }
    }
    return 0xFF;
}


/*******************************************************************************
*  函数名：MatrixKbLED_GetKey
********************************************************************************
*
*  【功    能】
*    获取某个按键、或全部按键的当前状态
*
*  【入口参数】
*    key：指定要获取的按键，使用以下宏之一：
*         S0、S1、S2、S3、S4、S5、S6、S7、S8、S9、S10、S11、S12、S13、S14、S15、
*         S16_RIGHT、S16_UP、S16_LEFT、S17、S16_CHA、S16_CHB、S16_CENTER、S16_DOWN
*
*         使用宏SW_ALL时，获取以上所有按键的当前状态、以及旋转动作状态。
*
*  【返 回 值】
*    单个按键时：
*      KEYON：闭合（按下）
*      KEYOFF：断开（松开）
*      0xFFFFFFFF：参数错误
*
*    全部按键时：
*      32位数据表示所有各按键、以及旋转动作状态，如下表排布：
       _____________________________________________________________________
       b: 31 30 29  28  27..24   23   22  21  20  19  18   17 16     15..0
       ---------------------------------------------------------------------
          0  0  CCW CW  0 0 0 0  Down Ctr CHB CHA S17 Left Up Right  S15..S0
       ---------------------------------------------------------------------
       注：逆时针旋转CCW、顺时针旋转CW位为Sticky位，置位后将保持，读后同时清零。
*
*******************************************************************************/
uint32 MatrixKbLED_GetKey(uint32 key)
{
    uint8  i;
    uint8  row, col;
    uint32 ret;
    
    if(key == SW_ALL)
    {
        ret = 0ul;
        for(row=0; row<4; row++)
        {
            ret += (uint32)(keystatus_map_prev[row] & 0x0F) <<(row*4);
        }
        ret += (uint32)keystatus_map_prev[4] <<16;
        
        ret += (uint32)rotary_action <<28;
        rotary_action = 0;
        return ret;
    }
    
    for(i=0; i<MAXSWNUM; i++)
    {
        if(key == 0)
        {
            return 0xFFFFFFFFul;
        }
        if((key & 0x00000001) !=0)
        {
            row = i/4;
            col = i%4;
            if((i >= 20) && (i <= 23))
            {
                row = 4;
                col += 4;
            }
            if(row < NUM_KEYROWS)
            {
                if(col < NUM_KEYCOLS)
                {
                    if((keystatus_map_prev[row] & (0x01<<col)) == 0)
                        return KEYON;
                    else
                        return KEYOFF;
                }
            }
            else if(row == 4)
            {
                if((col < NUM_KEYCOLS) || (col >= 4))
                {
                    if((keystatus_map_prev[row] & (0x01<<col)) == 0)
                        return KEYON;
                    else
                        return KEYOFF;
                }
            }
        }
        key >>=1;
    }
    return 0xFFFFFFFFul;
}


/*******************************************************************************
*  函数名：MatrixKbLED_GetKey_RC
********************************************************************************
*
*  【功    能】
*    获取指定行列处的一个按键的当前状态
*
*  【入口参数】
*    row、col：按键所在的行、列号，如下表所示排布
    _____________________________________________________________________
    |      7       6       5       4       3      2       1       0     | col
    ---------------------------------------------------------------------
    | 4: S16_Dwn,S16_Ctr,S16_CHB,S16_CHA, S17, S16_Left,S16_Up,S16_Rght |
    | 3:   x,      x,      x,      x,     S15,   S14,    S13,    S12    | NUM_KEYROWS-1
    | 2:   x,      x,      x,      x,     S11,   S10,    S9,     S8     | .
    | 1:   x,      x,      x,      x,     S7,    S6,     S5,     S4     | .
    | 0:   x,      x,      x,      x,     S3,    S2,     S1,     S0     | 0
    ---------------------------------------------------------------------
     row                                       (NUM_KEYCOLS-1) .. 0

*  【返 回 值】
*    KEYON：亮
*    KEYOFF：灭
*    0xFF：参数错误
*
*******************************************************************************/
uint8  MatrixKbLED_GetKey_RC(uint8 row, uint8 col)
{
    if(((row < NUM_KEYROWS) && (col < NUM_KEYCOLS)) || 
        ((row == 4) && ((col < NUM_KEYCOLS) || ((col >= 4) &&(col <= 7)))))
    {
        if((keystatus_map_prev[row] & (0x01<<col)) == 0)
            return KEYON;
        else
            return KEYOFF;
    }
    else
    {
        return 0xFF;
    }
}


/*******************************************************************************
*  函数名：MatrixKbLED_SimKeyAction
********************************************************************************
*
*  【功    能】
*    模拟按键动作时发送一条消息
*
*  【入口参数】
*    keysym：按键符号
*    onoff：KEYON——按下、KEYOFF——松开

*  【返 回 值】
*    无
*
*******************************************************************************/
#if(MESSAGE_GEN == 1)
void  MatrixKbLED_SimKeyAction(uint8 keysym, uint8 onoff)
{
#if(PORTING_TO_CY8CKIT == 1)              // PSoC 5LP处理器、GCC编译器时
    MatrixKbLED_isr_MKbLED_Disable();
#elif(PORTING_TO_CY8CKIT == 2)            // PSoC 6处理器、GCC编译器时
    NVIC_DisableIRQ((IRQn_Type)MatrixKbLED_SysInt_Tmr_MKbLED__INTC_NUMBER);
#endif
    if(onoff == KEYON)
    {
        Qkey_FeedData(keysym & 0x7F);
    }
    else if(onoff == KEYOFF)
    {
        Qkey_FeedData(keysym | 0x80);
    }
#if(PORTING_TO_CY8CKIT == 1)              // PSoC 5LP处理器、GCC编译器时
    MatrixKbLED_isr_MKbLED_Enable();
#elif(PORTING_TO_CY8CKIT == 2)            // PSoC 6处理器、GCC编译器时
    NVIC_EnableIRQ((IRQn_Type)MatrixKbLED_SysInt_Tmr_MKbLED__INTC_NUMBER);
#endif
}


/*******************************************************************************
*  函数名：Qkey_Init
********************************************************************************
*
*  【功    能】
*    初始化队列（使用静态数组）
*
*  【入口参数】
*    无
*
*  【返 回 值】
*    无
*
*******************************************************************************/
static void Qkey_Init(void)
{
    Qkey.pBase = qbuff;
    Qkey.front = 0;
    Qkey.rear = 0;
    Qkey.maxsize = QSIZE;
}


/*******************************************************************************
*  函数名：Qkey_IsFull
********************************************************************************
*
*  【功    能】
*    判断队列是否满
*
*  【入口参数】
*    无
*
*  【返 回 值】
*    1：队列已满
*    0：队列未满
*
*******************************************************************************/
static uint8 Qkey_IsFull(void)
{
    /* 判断循环队列是否完全满，预留一个字节空间不用 */
    if(Qkey.front == (Qkey.rear +1) % Qkey.maxsize)
        return 1;
    else
        return 0;
}


/*******************************************************************************
*  函数名：Qkey_IsEmpty
********************************************************************************
*
*  【功    能】
*    判断队列是否空
*
*  【入口参数】
*    无
*
*  【返 回 值】
*    1：队列空
*    0：队列非空
*
*******************************************************************************/
static uint8 Qkey_IsEmpty(void)
{
    if(Qkey.front == Qkey.rear)
        return 1;
    else
        return 0;
}


/*******************************************************************************
*  函数名：Qkey_FeedData
********************************************************************************
*
*  【功    能】
*    向队尾添加一条（一字节）消息
*
*  【入口参数】
*    bytemsg：消息数据
*
*  【返 回 值】
*    1：入队列成功
*    0：入队列失败（因队列已满）
*
*******************************************************************************/
uint8 Qkey_FeedData(uint8 bytemsg)
{
    uint8 ret;
    
#if(PORTING_TO_CY8CKIT == 1)              // PSoC 5LP处理器、GCC编译器时
    MatrixKbLED_isr_MKbLED_Disable();
#elif(PORTING_TO_CY8CKIT == 2)            // PSoC 6处理器、GCC编译器时
    NVIC_DisableIRQ((IRQn_Type)MatrixKbLED_SysInt_Tmr_MKbLED__INTC_NUMBER);
#endif
    if(Qkey_IsFull() == 1)
        ret = 0;
    else
    {
        Qkey.pBase[Qkey.rear] = bytemsg;
        Qkey.rear = (Qkey.rear + 1) % Qkey.maxsize;
        ret = 1;
    }
#if(PORTING_TO_CY8CKIT == 1)              // PSoC 5LP处理器、GCC编译器时
    MatrixKbLED_isr_MKbLED_Enable();
#elif(PORTING_TO_CY8CKIT == 2)            // PSoC 6处理器、GCC编译器时
    NVIC_EnableIRQ((IRQn_Type)MatrixKbLED_SysInt_Tmr_MKbLED__INTC_NUMBER);
#endif
    return ret;
}


/*******************************************************************************
*  函数名：Qkey_FetchData
********************************************************************************
*
*  【功    能】
*    从队首获取一条（一字节）消息
*
*  【入口参数】
*    无
*
*  【返 回 值】
*    0：   未获取到消息
*    其它：获取到的消息
*
*******************************************************************************/
uint8 Qkey_FetchData(void)
{
    uint8 bytemsg;

#if(PORTING_TO_CY8CKIT == 1)              // PSoC 5LP处理器、GCC编译器时
    MatrixKbLED_isr_MKbLED_Disable();
#elif(PORTING_TO_CY8CKIT == 2)            // PSoC 6处理器、GCC编译器时
    NVIC_DisableIRQ((IRQn_Type)MatrixKbLED_SysInt_Tmr_MKbLED__INTC_NUMBER);
#endif
    if(Qkey_IsEmpty() == 1)
        bytemsg = 0;
    else
    {
        bytemsg = Qkey.pBase[Qkey.front];
        Qkey.front = (Qkey.front +1) % Qkey.maxsize;
    }
#if(PORTING_TO_CY8CKIT == 1)              // PSoC 5LP处理器、GCC编译器时
    MatrixKbLED_isr_MKbLED_Enable();
#elif(PORTING_TO_CY8CKIT == 2)            // PSoC 6处理器、GCC编译器时
    NVIC_EnableIRQ((IRQn_Type)MatrixKbLED_SysInt_Tmr_MKbLED__INTC_NUMBER);
#endif
    return bytemsg;
}
#endif
/* [] END OF FILE */
