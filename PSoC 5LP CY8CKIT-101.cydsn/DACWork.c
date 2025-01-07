#include "DACWork.h"
#include "piano.h"

/* 定时器中断服务函数 */
static void Int_DAC_WC1_handler(void);
static void Int_DAC_WC2_handler(void);

char tmp[10] = "";
int i = 0;
bool WC1Update = false, WC2Update = false;

void InitSynthChannels()
{
    for (int ch = 0; ch < NUM_CHANNELS; ch++)
    {
        for (int n = 0; n < MAX_NOTES_PER_CHANNEL; n++)
        {
            synthChannels[ch].notes[n].data = NULL;
            synthChannels[ch].notes[n].current_step = 0;
            synthChannels[ch].notes[n].active = false;
        }
    }
}
bool addNote(int channel, const uint8_t (*noteData)[50])
{
    if (channel < 0 || channel >= NUM_CHANNELS)
    {
        return false; // 无效的通道
    }

    Channel *ch = &synthChannels[channel];
    for (int n = 0; n < MAX_NOTES_PER_CHANNEL; n++)
    {
        if (!ch->notes[n].active)
        {
            ch->notes[n].data = noteData;
            ch->notes[n].current_step = 0;
            ch->notes[n].active = true;
            return true; // 成功添加
        }
    }
    return false; // 通道已满
}

bool removeNote(int channel, int noteIndex)
{
    if (channel < 0 || channel >= NUM_CHANNELS || noteIndex < 0 || noteIndex >= MAX_NOTES_PER_CHANNEL)
    {
        return false; // 无效的通道或音符索引
    }

    Channel *ch = &synthChannels[channel];
    if (ch->notes[noteIndex].active)
    {
        ch->notes[noteIndex].active = false;
        ch->notes[noteIndex].data = NULL;
        ch->notes[noteIndex].current_step = 0;
        return true; // 成功删除
    }
    return false; // 音符未激活
}

void synthesize(uint8_t output[50]) {
    // 初始化输出缓冲区
    memset(output, 0, 50);

    // 临时变量用于累加
    int temp[50] = {0};
    int active_notes = 0; // 记录当前活跃的音符总数

    // 遍历所有通道
    for (int ch = 0; ch < NUM_CHANNELS; ch++) {
        Channel *channel = &synthChannels[ch];
        // 遍历通道中的所有音符
        for (int n = 0; n < MAX_NOTES_PER_CHANNEL; n++) {
            SynthNote *note = &channel->notes[n];
            if (note->active && note->data != NULL) {
                // 确保当前步数在范围内
                if (note->current_step < 48) {
                    // 合成当前步的50个数据点
                    for (int i = 0; i < 50; i++) {
                        temp[i] += note->data[note->current_step][i];
                    }
                    active_notes++; // 每个活跃音符贡献一次
                    // 移动到下一步
                    note->current_step++;
                    // 如果音符合成完成，自动删除该音符
                    if (note->current_step >= 48) {
                        // 使用 removeNote 函数删除音符
                        removeNote(ch, n);
                    }
                } else {
                    // 安全起见，再次删除已完成的音符
                    removeNote(ch, n);
                }
            }
        }
    }

    // 避免除以零
    if (active_notes == 0) {
        // 如果没有活跃的音符，输出全零
        memset(output, 0, 50);
        return;
    }

    // 将累加后的数据除以活跃的音符数量，缩放到0-255
    for (int i = 0; i < 50; i++) {
        // 使用整数除法，确保结果在0-255范围内
        // 可以根据需要添加舍入或限制
        int scaled = temp[i] / active_notes;
        if (scaled < 0) scaled = 0;
        if (scaled > 255) scaled = 255;
        output[i] = (uint8_t)scaled;
    }
}


// void synthesize(uint8_t output[50])
// {
//     // 初始化输出缓冲区
//     memset(output, 0, 50);

//     // 临时变量用于累加
//     int temp[50] = {0};
//     int active_notes = 0; // 记录当前活跃的音符总数

//     // 遍历所有通道
//     for (int ch = 0; ch < NUM_CHANNELS; ch++)
//     {
//         Channel *channel = &synthChannels[ch];
//         // 遍历通道中的所有音符
//         for (int n = 0; n < MAX_NOTES_PER_CHANNEL; n++)
//         {
//             SynthNote *note = &channel->notes[n];
//             if (note->active && note->data != NULL)
//             {
//                 // 确保当前步数在范围内
//                 if (note->current_step < 48)
//                 {
//                     // 合成当前步的50个数据点
//                     for (int i = 0; i < 50; i++)
//                     {
//                         temp[i] += note->data[note->current_step][i];
//                     }
//                     active_notes++; // 每个活跃音符贡献一次
//                     // 移动到下一步
//                     note->current_step++;
//                     // 如果音符合成完成，自动删除该音符
//                     if (note->current_step >= 48)
//                     {
//                         // 使用 removeNote 函数删除音符
//                         removeNote(ch, n);
//                     }
//                 }
//                 else
//                 {
//                     // 安全起见，再次删除已完成的音符
//                     removeNote(ch, n);
//                 }
//             }
//         }
//     }

//     // 避免除以零
//     if (active_notes == 0)
//     {
//         // 如果没有活跃的音符，输出全零
//         memset(output, 0, 50);
//         return;
//     }

//     // 将累加后的数据除以活跃的音符数量，缩放到0-255
//     for (int i = 0; i < 50; i++)
//     {
//         // 使用整数除法，确保结果在0-255范围内
//         // 可以根据需要添加舍入或限制
//         int scaled = temp[i] / active_notes;
//         if (scaled < 0)
//             scaled = 0;
//         if (scaled > 255)
//             scaled = 255;
//         output[i] = (uint8_t)scaled;
//         UART_1_PutString(scaled);
//         UART_1_PutString("\r\n");
//     }
// }

static void Int_DAC_WC1_handler(void)
{
    // sprintf(tmp, "WC1:%d", i);
    // i++;
    // UART_1_PutString(tmp);
    // UART_1_PutString("WC1Complete\r\n");
    Control_Reg_Write(1);
    // WaveDAC8_Stop();
    WC1Update = true;
}
static void Int_DAC_WC2_handler(void)
{
    // sprintf(tmp, "WC2:%d", i);
    // i++;
    // UART_1_PutString(tmp);
    // WaveDAC8_Stop();
    // UART_1_PutString("WC2");
    Control_Reg_Write(0);
    WC2Update = true;
}

void InitDACWork()
{
    DAC_WC1_ClearPending();
    DAC_WC1_StartEx(Int_DAC_WC1_handler);
    DAC_WC2_ClearPending();
    DAC_WC2_StartEx(Int_DAC_WC2_handler);
    InitSynthChannels();
}

void DACWork_key(uint8 key)
{
    key -= 48;
    switch (key)
    {
    case 0:
        addNote(0, DACData_C4);
        break;
    case 1:
        addNote(0, DACData_D4);
        break;
    case 2:
        addNote(0, DACData_E4);
        break;
    case 3:
        addNote(0, DACData_F4);
        break;
    case 4:
        addNote(0, DACData_G4);
        break;
    case 5:

        break;
    case 6:

        break;
    case 7:

        break;

    default:
        break;
    }
    // if (key <= 8)
    // {
    //     WaveDAC8_Start();
    // }
}