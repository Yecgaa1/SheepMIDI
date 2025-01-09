/* =============================================================================
 * 本示例演示了段式LCD、LED电平计、耳机放大器模块（CY8CKIT-101）的基本应用
 *
 * 示例使用了自定义的按键模块宏组件Button Module Macro，根据演示需求对其进行了配置。
 *
 * 【运行前的连接】：
 *  1. 用导线连接CY8CKIT-050板的插孔P0_6，至CY8CKIT-101板的P4插座IN1孔。
 *  2. 用导线连接CY8CKIT-050板的插孔P6_5，至CY8CKIT-101板的P4插座IN2孔。
 *  3. 拔下CY8CKIT-050板上电位器R56下方位置的红色跳线帽
 *  4. 将耳机插入CY8CKIT-101板的3.5mm耳机插座
 *  5. 用导线连接CY8CKIT-050板的插孔VIN，至CY8CKIT-101板的P6插座VIN孔，为CY8CKIT-101
 *     板供电。（当CY8CKIT-101与CY8CKIT-102板组合使用时，无需此步的导线连接）
 *
 * 【预期的运行效果】：
 *  1. 在段式LCD上显示测得的VDAC8经PGA放大后的电压（mV数），该电压引出至P0_6和IN1。
 *     如果电压值为可靠的低电平（3.3V电源时），则还会在屏的左下侧显示小三角标识。
 *     如果电压值为可靠的高电平（3.3V电源时），则还会在屏的左上侧显示小三角标识。
 *  2. 用户单击一次SW3，将以约32mV的步长递增上述电压，直至约3V。
 *     用户单击一次SW2，将以约32mV的步长递减上述电压，直至约0V。
 *     用户可进行长按，长按期间将自动地快速连续递增或递减。
 *  3. 在CY8CKIT-101的LED电平计（由10个绿色LED组成）上线性地指示上述电压，LED电平
 *     计指示的满量程范围为0 ~ 2.3V，各LED呈现的是左亮右暗的渐变效果。
 *  4. 耳机将听到报警声，调节电位器可改变音量。
 *  5. 手指在电容感应滑条上触摸或者滑动，可以调节段式LCD的显示对比度。
 * =============================================================================
 */
#include "project.h"
#include "stdbool.h"
#include "stdio.h"
#include "DACWork.h"
/* 定义此宏表示VDAC8至ADC的总误差已校正 */
#define VDAC_TO_ADC_CALIBRATED

#ifdef VDAC_TO_ADC_CALIBRATED
/* 校正前，VDAC8输出0V时的ADC测量读数 */
#define V0_READ_BEFORE_CAL 20L
/* 校正前，VDAC8输出最大电压时的ADC测量读数 */
#define VMAX_READ_BEFORE_CAL 3022L
/* VDAC8输出0V时的ADC输入端真实值 */
#define V0_ACTUAL 23L
/* VDAC8输出最大电压时的ADC输入端真实值 */
#define VMAX_ACTUAL 2956L
/* VDAC8至ADC的总增益误差修正系数（勿对其整体加括号）*/
#define GAIN_FACTOR_COR (VMAX_ACTUAL - V0_ACTUAL) / (VMAX_READ_BEFORE_CAL - V0_READ_BEFORE_CAL)
#else
#define V0_READ_BEFORE_CAL 0L
#define V0_ACTUAL 0L
#define GAIN_FACTOR_COR 1L
#endif // VDAC_TO_ADC_CALIBRATED

void MatrixKbLED_Task(void);
void LCD_Indicator(uint8 msg, uint32 key_statuses);
extern bool WC1Update, WC2Update;

/*******************************************************************************
 *                                    main()
 *
 * Description : This is the standard entry point for C code.  It is assumed that
 *               your code will call main() once you have performed all necessary
 *               initialization.
 *
 * Argument(s) : none
 * Return(s)   : none
 * Caller(s)   : Startup Code.
 ********************************************************************************
 */
int main(void)
{
    /* 使能全局中断 */
    CYGlobalIntEnable;

    // 初始化串口模块
    UART_1_Start();
    UART_2_Start();
    TCJSendEnd();
    UART_1_PutString("hello!");

    /* 初始化WaveDAC8组件 */
    WaveDAC8_Start();

    /* 初始化按键模块组件 */
    Button_Module_Start();

    InitDACWork(); // 初始化DAC模块

    LCD_Char_Start();    /* 初始化LCD_Char组件 */
    MatrixKbLED_Start(); /* 初始化MatrixKbLED组件 */

    //后续合成都会输出在这两个uint8
    uint8_t Wave1_Output[50];
    uint8_t Wave2_Output[50];
    WaveDAC8_Wave1Setup(Wave1_Output, 50);
    WaveDAC8_Wave2Setup(Wave2_Output, 50);
    // uint8 time = 0;

    while (1)
    {
        // 串口处理模块
        char tmp[20] = "";
        uint8 t = UART_2_GetRxBufferSize();
        if (t != 0)
        {
            for (uint8 i = 0; i < t; i++)
            {
                tmp[i] = UART_2_GetChar();
            }
            ScreenWork(tmp); // 解析数据
        }

        // DAC更新
        if (WC1Update)
        {
            WC1Update = false;
            Synthesize(Wave1_Output);
        }
        else if (WC2Update)
        {
            WC2Update = false;
            Synthesize(Wave2_Output);
        }

        /* Timer_Button定时器运行了一个周期（默认为BUTTON_SAMPLE_PERIOD_MS =20ms）*/
        if ((Timer_Button_ReadStatusRegister() & Timer_Button_STATUS_TC) != 0)
        {
            MatrixKbLED_Task(); /* 周期性运行MatrixKbLED任务 */
        }
    }
}

/*******************************************************************************
 *  函数名：MatrixKbLED_Task
 ********************************************************************************
 *  【功    能】
 *    MatrixKbLED任务
 *    (1) 实现流水灯效果
 *    (2) 对于用户的按键动作，于对应的LED上指示。
 *    (3) 对于旋转编码开关的旋转动作，于LED灯条上指示。
 *    (4) 于LCD上显示按键动作、全部按键以及旋转动作的当前状态
 *
 *  【入口参数】
 *    无
 *
 *  【返 回 值】
 *    无
 *******************************************************************************/
void MatrixKbLED_Task(void)
{
    static uint8 cnt4Marquee = 0;    /* 软件定时器的计数器，用于流水速度计时 */
    static uint8 ikey = 0;           /* 用于实现流水灯效果，为keysym_seq[]数组元素的索引号，该数组记录了各灯的点亮次序。
                                        或用于获取某个LED的当前状态，以避免流水灯影响已按下键所对应的LED亮灯 */
    static uint8 tff = 0;            /* 等效T触发器，用于实现流水灯效果 */
    static uint8 barled_lvl = 0;     /* LED灯条的指示级 */
    static uint32 leds = LED_D0_S0B; /* 指定需点亮或者熄灭的若干个LED */
    // static uint32 keys_prev = 0;     /* 保存上一次的各按键开合状态 */
    uint32 keys; /* 保存当前的各按键开合状态 */
    uint8 qdat;  /* 保存按键消息 */
    uint8 rc;    /* 保存按键所在的行列号 */

    /* cnt4Marquee于0~4之间循环递增 */
    cnt4Marquee = (cnt4Marquee < 5 - 1) ? cnt4Marquee + 1 : 0;

#if (MESSAGE_GEN == 1) /* 当配置MatrixKbLED组件生成按键消息时 */

    /* A.按键消息获取及在LCD、LED、LED灯条上显示 */
    qdat = Qkey_FetchData();
    if (qdat != 0) // 有按键事件
    {
        /* 获取全部按键以及旋转动作的当前状态 */
        keys = MatrixKbLED_GetKey(SW_ALL);

        char tmp[20] = "";
        // sprintf(tmp, "%c,%c\r\n", qdat, '0' + ((keys >> 19) & 1u));
        // UART_1_PutString(tmp);
        sprintf(tmp, "%d,%d\r\n", qdat, keys);
        UART_1_PutString(tmp);

        DACWork_key(qdat);
        /* 在LCD上显示按键动作和当前状态信息 */
        LCD_Indicator(qdat, keys);

        /* 搜索按键符号所在的行号、列号 */
        rc = MatrixKbLED_KeySym2RC(qdat);
        if ((qdat & 0x80) == 0) // 按下
        {                       /* 点亮对应行、列处的LED */
            if ((rc >> 4) != 4) // 避开LED灯条
            {
                MatrixKbLED_SetLED_RC(rc >> 4, rc & 0x0Fu, LEDON);
            }
        }
        else                    // 松开
        {                       /* 熄灭对应行、列处的LED */
            if ((rc >> 4) != 4) // 避开LED灯条
            {
                MatrixKbLED_SetLED_RC(rc >> 4, rc & 0x0Fu, LEDOFF);
            }
        }

        /* 旋转编码开关旋转动作消息处理 */
        if (qdat == 'o') // CW，顺时针旋转，LED灯条指示级增1
        {
            if (barled_lvl < LVLNUM_BARLED - 1)
            {
                barled_lvl++;
                MatrixKbLED_SetBarLEDLevel(barled_lvl);
            }
        }
        else if (qdat == 0x80 + (uint8)'o') // CCW，逆时针旋转，LED灯条指示级减1
        {
            if (barled_lvl > 0)
            {
                barled_lvl--;
                MatrixKbLED_SetBarLEDLevel(barled_lvl);
            }
        }
    }

    /* B.模拟按键依次流水动作，从而等效实现流水灯效果。此段演示使用非用户自定义的按键符号表！
        （使能该段代码时，应同时注释掉下面的C.矩阵LED流水灯显示代码）*/
/*    const char8 keysym_seq[(NUM_KEYROWS+1)*NUM_KEYCOLS +3] = {'0', '1', '2', '3',  // 该数组记录了各灯的点亮次序
                                                              '4', '5', '6', '7',
                                                              '8', '9', 'A', 'B',
                                                              'C', 'D', 'E', 'F',
                                                              '>', '^', '<', 'v', '-', '=', '+'};
    if(cnt4Marquee == 0)                  // 每250ms时长到时
    {
        if(tff == 0)
        {
            if(ikey < (NUM_KEYROWS+1)*NUM_KEYCOLS +3)     // 1.依次发送各键按下消息（1.2.步交替进行）
            {
                MatrixKbLED_SimKeyAction(keysym_seq[ikey], KEYON);
            }
            else if((ikey >= (NUM_KEYROWS+1)*NUM_KEYCOLS +3) &&
                (ikey <= (NUM_KEYROWS+1)*NUM_KEYCOLS +9)) // 3.连续发送7次旋转编码开关顺时针旋转消息
            {
                MatrixKbLED_SimKeyAction('o', KEYON);
                ikey ++;
                tff = !tff;
            }
            else
            {
                ikey = (NUM_KEYROWS+1)*NUM_KEYCOLS +3;
            }
        }
        else
        {
            if(ikey < (NUM_KEYROWS+1)*NUM_KEYCOLS +3)     // 2.依次发送各键松开消息（2.1.步交替进行）
            {
                MatrixKbLED_SimKeyAction(keysym_seq[ikey], KEYOFF);
                ikey ++;
            }
            else if((ikey >= (NUM_KEYROWS+1)*NUM_KEYCOLS +3) &&
                (ikey <= (NUM_KEYROWS+1)*NUM_KEYCOLS +9)) // 4.连续发送7次旋转编码开关逆时针旋转消息
            {
                MatrixKbLED_SimKeyAction('o', KEYOFF);
                ikey ++;
                tff = !tff;
            }
            else
            {
                ikey = 0;
            }
        }
        tff = !tff;
    }
*/
#else /* 当配置MatrixKbLED组件不生成按键消息时 */

    /* A.全部按键状态及旋转动作状态获取，以及在LCD、LED灯条上显示 */
    /* 获取全部按键以及旋转动作的当前状态 */
    keys = MatrixKbLED_GetKey(SW_ALL);
    if (keys != keys_prev) // 有按键/开关动作
    {
        /* 在LCD第三、四行显示所有按键及旋转动作当前状态 */
        LCD_Indicator(0, keys);

        /* 旋转编码开关旋转动作信息处理 */
        if ((keys & ROTARY_ACTION_CW) != 0) // CW，顺时针旋转，LED灯条指示级增1
        {
            if (barled_lvl < LVLNUM_BARLED - 1)
            {
                barled_lvl++;
                MatrixKbLED_SetBarLEDLevel(barled_lvl);
            }
        }
        else if ((keys & ROTARY_ACTION_CCW) != 0) // CCW，逆时针旋转，LED灯条指示级减1
        {
            if (barled_lvl > 0)
            {
                barled_lvl--;
                MatrixKbLED_SetBarLEDLevel(barled_lvl);
            }
        }
        keys_prev = keys;
    }

#endif

    /* C.矩阵LED流水灯显示（修改显示缓冲区），不影响已按下键所对应的LED灯亮
        （使能该段代码时，应同时注释掉上面的B.模拟按键依次流水动作代码） */
    if (cnt4Marquee == 0) // 每250ms时长到时
    {
        // 某LED亮、灭一次（即闪烁一次）
        if (tff == 0)
        {
            ikey = MatrixKbLED_GetLED(leds);
            MatrixKbLED_SetLED(leds, LEDON);
        }
        else
        {
            if (ikey == LEDOFF) // 避开已按下键所对应的LED亮灯
            {
                MatrixKbLED_SetLED(leds, LEDOFF);
            }

            if (leds < LED_D13_S15B)
            {
                leds <<= 1;
            }
            else if (leds == LED_D13_S15B)
            {
                leds = LED_D19;
            }
            else if (leds == LED_D19)
            {
                leds = LED_D16;
            }
            else if (leds < LED_D18)
            {
                leds <<= 1;
            }
            else if (leds == LED_D18)
            {
                leds = LED_D20;
            }
            else
            {
                leds <<= 1;
            }

            if (leds > LED_D22)
            {
                leds = LED_D0_S0B;
            }
        }
        tff = !tff;
    }
}

/*******************************************************************************
 *  函数名：LCD_Indicator
 ********************************************************************************
 *  【功    能】
 *    在LCD上显示按键动作、全部按键以及旋转动作的当前状态
 *
 *    于4x20屏上显示如下：
 *
 *    (1) 将第一、二行分为4个显示区，显示最后3次的按键动作或旋转动作信息。
 *        其中有某个区为空白，其左侧区显示的为最后的一条信息。
 *
 *             显示区1       显示区2       显示区3       显示区4
 *         ___________________________________________________________
 *        |    按键符号      按键符号      按键符号      按键符号     |
 *        |      动作          动作          动作          动作       |
 *        |S17   S15 S14 S13 S12 S11 S10 S9 S8 S7 S6 S5 S4 S3 S2 S1 S0|
 *        |CCW CW  CHB CHA, Left Up Center Down Right                 |
 *         ———————————————————————————————————————————————————————————
 *    (2) 第三、四行显示全部按键以及旋转动作的当前状态
 *        值为1表示松开或CCW旋转，值为0表示按下或CW旋转。
 *        CCW或CW旋转时的值1仅存在于一瞬间，因紧接着的下一次LCD更新将其置为了0
 *
 *  【入口参数】
 *    msg：         按键消息（源自Qkey_FetchData()的返回值）
 *    key_statuses：全部按键以及旋转动作的当前状态（源自MatrixKbLED_GetKey(SW_ALL)的返回值）
 *
 *  【返 回 值】
 *    无
 *******************************************************************************/
void LCD_Indicator(uint8 msg, uint32 key_statuses)
{
    static uint8 pos = 0; /* LCD第一、二行的当前字符位置 */
    uint8 pos_next;       /* LCD第一、二行的下一个显示区起始字符位置 */

    /* 有按键事件时 */
    if (msg != 0)
    {
        /* 清除LCD第一、二行的下一个显示区内容 */
        pos_next = (pos < 20 - 4) ? pos + 4 : 0;
        LCD_Char_Position(0, pos_next);
        LCD_Char_PrintString("  ");
        LCD_Char_Position(1, pos_next);
        LCD_Char_PrintString("   ");

        /* 在LCD第一行的当前显示区显示按键符号 */
        LCD_Char_Position(0, pos);
        LCD_Char_PutChar(' ');
        LCD_Char_PutChar(msg & 0x7F);

        /* 在LCD第二行的当前显示区显示按键动作 */
        LCD_Char_Position(1, pos);
        if ((msg & 0x80) == 0) // 按下或CW旋转
        {
            if (msg != 'o')
            {
                LCD_Char_PrintString("ON ");
            }
            else // CW旋转
            {
                LCD_Char_PrintString("CW ");
            }
        }
        else // 松开或CCW旋转
        {
            if ((msg & 0x7F) != 'o')
            {
                LCD_Char_PrintString("OFF");
            }
            else // CCW旋转
            {
                LCD_Char_PrintString("CCW");
            }
        }

        /* 切换到LCD第一、二行的下一个显示区起始位置 */
        pos = (pos < 20 - 4) ? pos + 4 : 0;
    }

    /* 以下直至结束：在LCD第三、四行显示所有按键及旋转动作当前状态 */
    LCD_Char_Position(2, 1);
    LCD_Char_PutChar('0' + ((key_statuses >> 19) & 1u)); // S17
    LCD_Char_PutChar(' ');                               // ' '

    pos_next = 16; // S15-S0
    do
    {
        pos_next--;
        LCD_Char_PutChar('0' + ((key_statuses >> pos_next) & 1u));
    } while (pos_next > 0);

    LCD_Char_Position(3, 0);
    LCD_Char_PutChar('0' + ((key_statuses >> 29) & 1u)); // CCW
    LCD_Char_PutChar('0' + ((key_statuses >> 28) & 1u)); // CW
    LCD_Char_PutChar(' ');                               // ' '
    LCD_Char_PutChar('0' + ((key_statuses >> 21) & 1u)); // CHB
    LCD_Char_PutChar('0' + ((key_statuses >> 20) & 1u)); // CHA
    LCD_Char_PutChar(',');                               // ','
    LCD_Char_PutChar('0' + ((key_statuses >> 18) & 1u)); // Left
    LCD_Char_PutChar('0' + ((key_statuses >> 17) & 1u)); // Up
    LCD_Char_PutChar('0' + ((key_statuses >> 22) & 1u)); // Center
    LCD_Char_PutChar('0' + ((key_statuses >> 23) & 1u)); // Down
    LCD_Char_PutChar('0' + ((key_statuses >> 16) & 1u)); // Right
}

/* [] END OF FILE */
