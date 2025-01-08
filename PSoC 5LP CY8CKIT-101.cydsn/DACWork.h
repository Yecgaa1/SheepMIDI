#include "project.h"
#include "stdio.h"
#include <string.h>
#include <stdint.h>
#include <stdbool.h>


// 定义三个通道，每个通道最多三个音符
#define NUM_CHANNELS 4
#define MAX_NOTES_PER_CHANNEL 4


#define BARS 4
#define NOTES_PER_BAR 12
#define TOTAL_NOTES (BARS * NOTES_PER_BAR)

typedef struct
{
    const uint8_t (*data)[50]; // 指向48次合成的50个数据点的波形数据
    int current_step;          // 当前合成到的位置（0到47）
    bool active;               // 是否处于活动状态
} SynthNote;

typedef struct
{
    SynthNote notes[MAX_NOTES_PER_CHANNEL]; // 每个通道最多三个同时合成的音符
} Channel;

// 乐谱结构体
typedef struct
{
    SynthNote notes[TOTAL_NOTES];
    int current_time_step; // 当前播放到的时间步（0到63）
    int notes_in_bar[BARS];    // 每个小节的实际音符数量
} Score;

Channel synthChannels[NUM_CHANNELS];

void InitDACWork();
void DACWork_key(uint8 key);
void Synthesize(uint8_t output[50]);
bool addNote(int channel, const uint8_t (*noteData)[50]);
void init_score(Score *score);
void fill_score_note(Score *score, int bar, int note, const uint8_t (*data)[50]);
void read_score(Score *score);
void clear_score(Score *score);
void fill_score_example(Score *score);
