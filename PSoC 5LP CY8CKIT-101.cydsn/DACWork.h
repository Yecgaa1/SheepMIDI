#include "project.h"
#include "stdio.h"
#include <string.h> 
#include <stdint.h>
#include <stdbool.h>
void InitDACWork();
void DACWork_key(uint8 key);
void synthesize(uint8_t output[50]);

typedef struct {
    const uint8_t (*data)[50]; // 指向48次合成的50个数据点的波形数据
    int current_step;          // 当前合成到的位置（0到47）
    bool active;               // 是否处于活动状态
} SynthNote;

typedef struct {
    SynthNote notes[3]; // 每个通道最多三个同时合成的音符
} Channel;

// 定义三个通道，每个通道最多三个音符
#define NUM_CHANNELS 3
#define MAX_NOTES_PER_CHANNEL 4

Channel synthChannels[NUM_CHANNELS];