#include "project.h"
#define TONE 1
#define NOTEPERTONE 7
#define LAYERS TONE *NOTEPERTONE + 1


extern const uint8 CYCODE DACData_C4[48][50];
extern const uint8 CYCODE DACData_D4[48][50];
extern const uint8 CYCODE DACData_E4[48][50];
extern const uint8 CYCODE DACData_F4[48][50];
extern const uint8 CYCODE DACData_G4[48][50];
extern const uint8 CYCODE DACData_A4[48][50];
extern const uint8 CYCODE DACData_B4[48][50];
extern const uint8 CYCODE DACData_C5[48][50];
extern uint8 CYCODE allN1ote[LAYERS][48][50];
extern const uint8 CYCODE allNoteNew[LAYERS][48][50];