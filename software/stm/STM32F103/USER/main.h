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
    uint32_t timestamp; // ϵͳʱ��
    char ip_address[16]; // IP ��ַ
	char adcode[12]; // ������������
    float ShtTemp; // SHT �¶�
    float ShtHumi; // SHT ʪ��
    uint8_t CurWeather; // ��ǰ����
    float CurTemp; // ��ǰ�¶�
};



#endif /* MAIN_H */
