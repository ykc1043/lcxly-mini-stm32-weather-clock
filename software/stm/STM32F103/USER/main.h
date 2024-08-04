#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
struct Sysdata
{
	bool ScreenReflesh;
    uint32_t timestamp; // 系统时间
    char ip_address[16]; // IP 地址
	char adcode[12]; // 行政区划代码
    float ShtTemp; // SHT 温度
    float ShtHumi; // SHT 湿度
    uint8_t CurWeather; // 当前天气
    float CurTemp; // 当前温度
};



#endif /* MAIN_H */
