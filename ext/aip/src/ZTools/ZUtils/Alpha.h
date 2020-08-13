#ifndef __alpha_h
#define __alpha_h
//---------------------------------------------------------------------//
#define RUSALP_UPPERFIRST 192
#define RUSALP_UPPERLAST  223
#define RUSALP_LOWERFIRST 224
#define RUSALP_LOWERLAST  255
#define RUSALP_UPPERJO    168
#define RUSALP_LOWERJO    184
#define RUSALP_UPPERJE    197
#define RUSALP_LOWERJE    229
//---------------------------------------------------------------------//
bool  IsUpperAlpChar(unsigned char c);
bool  IsRusAlpChar(unsigned char c);

unsigned char upperchar(unsigned char c);

char* setupper(char* text);
char* setlower(char* text);
void  checkjo(char* text);
//---------------------------------------------------------------------//
#endif
