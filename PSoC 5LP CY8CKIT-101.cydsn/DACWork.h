#include "project.h"
#include "stdio.h"
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "UartScreen.h"

// 定义四个通道，每个通道最多四个音符
#define NUM_CHANNELS 4
#define MAX_NOTES_PER_CHANNEL 4

// 定义乐谱信息
#define BARS 4
#define NOTES_PER_BAR 16
#define TOTAL_NOTES (BARS * NOTES_PER_BAR)

#define NOTE_C 0
#define NOTE_D 1
#define NOTE_E 2
#define NOTE_F 3
#define NOTE_G 4
#define NOTE_A 5
#define NOTE_B 6

// 单一音的结构体
typedef struct
{
    int8_t n_Sound;   // 指向48次合成的50个数据点的波形数据
    int current_step; // 当前合成到的位置（0到47）
    bool active;      // 是否处于活动状态
} SynthNote;

// 通道结构体
typedef struct
{
    SynthNote notes[MAX_NOTES_PER_CHANNEL]; // 每个通道最多四个同时合成的音符
    float voiceFact;                        // 通道的合成音量，0-1之间
} Channel;

// 乐谱结构体
typedef struct
{
    SynthNote notes[TOTAL_NOTES]; // 乐谱的存储
    int current_time_step;        // 当前播放到的时间步（0到TOTAL_NOTES）
    int notes_in_bar[BARS];       // 每个小节的实际音符数量
} Score;

Channel synthChannels[NUM_CHANNELS]; // NUM_CHANNELS通道的主存储

void InitDACWork();
void DACWork_key(uint8 key);
void Synthesize(uint8_t output[50]);
bool addNote(int channel, const uint8_t n_Sound);
void init_score(Score *score);
void fill_score_note(Score *score, int bar, int note, const uint8_t n_Sound);
void read_score(Score *score);
void clear_score(Score *score);
void fill_score_example(Score *score);
void FlashLed(uint16 led);
