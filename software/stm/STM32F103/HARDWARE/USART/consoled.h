#pragma once
#ifndef _CONSOLED_H_
#define _CONSOLED_H_


#include "string.h"
#include "stdio.h"
#include "sys.h"
#include "main.h"
#include "led.h"

extern volatile int g_interrupt;
extern unsigned char cmd_prpt_str[];
//这里声明你想让它控制的变量，.c文件也要改


void at_proc(unsigned char ch);



#endif

