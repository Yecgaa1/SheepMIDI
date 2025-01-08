#include "DACWork.h"
#include "piano.h"
// 初始化乐谱，清空所有音符
void init_score(Score *score)
{
    for (int i = 0; i < TOTAL_NOTES; i++)
    {
        score->notes[i].data = NULL;
        score->notes[i].active = false;
    }
    score->current_time_step = 0;
}

// 填充乐谱中的音符
// 参数解释：score - 乐谱指针
//            bar - 小节号（0到3）
//            note - 音符号（0到15）
//            data - 指向波形数据的指针（可以为NULL）
void fill_score_note(Score *score, int bar, int note, const uint8_t (*data)[50])
{
    if (bar < 0 || bar >= BARS || note < 0 || note >= NOTES_PER_BAR)
    {
        printf("无效的小节号或音符号。\n");
        return;
    }
    int index = bar * NOTES_PER_BAR + note;
    score->notes[index].data = data;
}

// 读取并处理乐谱中的音符
// 这里的处理示例是调用synthesize函数
void read_score(Score *score)
{
    if (score->current_time_step >= TOTAL_NOTES)
    {
        score->current_time_step = 0; // 循环播放
    }

    SynthNote current_note = score->notes[score->current_time_step];
    
    if (current_note.data != NULL)
    {
        addNote(0, current_note.data);
    }
    // 递增时间步
    score->current_time_step = (score->current_time_step + 1) % TOTAL_NOTES;
}

// 清空乐谱中的所有音符
void clear_score(Score *score)
{
    for (int i = 0; i < TOTAL_NOTES; i++)
    {
        score->notes[i].data = NULL;
        score->notes[i].active = false;
    }
    score->current_time_step = 0;
}

// 示例填充函数
void fill_score_example(Score *score)
{
    fill_score_note(score, 0, 0, DACData_C4);
    fill_score_note(score, 0, 1, DACData_D4);
    fill_score_note(score, 0, 2, DACData_G4);
    fill_score_note(score, 0, 3, DACData_C4);
    fill_score_note(score, 0, 4, DACData_D4);
    fill_score_note(score, 0, 5, DACData_G4);
    fill_score_note(score, 0, 6, DACData_C4);
    fill_score_note(score, 0, 7, DACData_D4);
    fill_score_note(score, 0, 8, DACData_G4);
    fill_score_note(score, 0, 9, DACData_C4);
    fill_score_note(score, 0, 10, DACData_D4);
    fill_score_note(score, 0, 11, DACData_G4);

    fill_score_note(score, 1, 0, DACData_C4);
    fill_score_note(score, 1, 1, DACData_D4);
    fill_score_note(score, 1, 2, DACData_G4);
    fill_score_note(score, 1, 3, DACData_C4);
    fill_score_note(score, 1, 4, DACData_D4);
    fill_score_note(score, 1, 5, DACData_G4);
    fill_score_note(score, 1, 6, DACData_C4);
    fill_score_note(score, 1, 7, DACData_D4);
    fill_score_note(score, 1, 8, DACData_G4);
    fill_score_note(score, 1, 9, DACData_C4);
    fill_score_note(score, 1, 10, DACData_D4);
    fill_score_note(score, 1, 11, DACData_G4);

    fill_score_note(score, 2, 0, DACData_C4);
    fill_score_note(score, 2, 1, DACData_D4);
    fill_score_note(score, 2, 2, DACData_G4);
    fill_score_note(score, 2, 3, DACData_C4);
    fill_score_note(score, 2, 4, DACData_D4);
    fill_score_note(score, 2, 5, DACData_G4);
    fill_score_note(score, 2, 6, DACData_C4);
    fill_score_note(score, 2, 7, DACData_D4);
    fill_score_note(score, 2, 8, DACData_G4);
    fill_score_note(score, 2, 9, DACData_C4);
    fill_score_note(score, 2, 10, DACData_D4);
    fill_score_note(score, 2, 11, DACData_G4);

    fill_score_note(score, 3, 0, DACData_C4);
    fill_score_note(score, 3, 1, DACData_D4);
    fill_score_note(score, 3, 2, DACData_G4);
    fill_score_note(score, 3, 3, DACData_C4);
    fill_score_note(score, 3, 4, DACData_D4);
    fill_score_note(score, 3, 5, DACData_G4);
    fill_score_note(score, 3, 6, DACData_C4);
    fill_score_note(score, 3, 7, DACData_D4);
    fill_score_note(score, 3, 8, DACData_G4);
    fill_score_note(score, 3, 9, DACData_C4);
    fill_score_note(score, 3, 10, DACData_D4);
    fill_score_note(score, 3, 11, DACData_G4);
}
