#include "consoled.h"

// 这里声明你想让它控制的变量，.h文件也要改
extern struct Sysdata sysdata;
//

unsigned char cmd_prpt_str[] = "STM32: > ";
extern unsigned char cmd_prpt_str[];
//#define consoled_delay_ms(x) __delay_cycles((long)(1000000 * (double)x / 1000.0))

typedef void (*cmd_cb)(char *);
typedef struct
{
  char *cmd;
  cmd_cb cb;
} ST_CMD;

// 系统命令
void help_proc(char *str);
// void reboot_proc(char *str);

volatile int g_interrupt = 1; // 是否接收到了Ctrl+C中断信号。0为否，1为是
extern volatile int g_interrupt;

// led命令处理（例子）
void led_proc(char *str)
{

    // 如果命令参数不包含"-always"
    if (strstr(str, "-h"))
    {
      printf("you can use [on/off/blink time/-always] ctrl led\r\n");
      return;
    }

    if (strstr(str, "on"))
    {
      LED=0;
      
      return;
    }
    if (strstr(str, "off"))
    {
      LED=1;
      
      return;
    }


}
// led命令处理（例子）
void showall_proc(char *str)
{

    // 如果命令参数不包含"-always"
    if (strstr(str, "-h"))
    {
      printf("None\r\n");
      return;
    }

    printf("timestamp: %u\r\n", sysdata.timestamp);
    printf("ip_address: %s\r\n", sysdata.ip_address);
    printf("ShtTemp: %.2f\r\n", sysdata.ShtTemp);
    printf("ShtHumi: %.2f\r\n", sysdata.ShtHumi);
    printf("CurWeather: %u\r\n", sysdata.CurWeather);
    printf("CurTemp: %.2f\r\n", sysdata.CurTemp);
    printf("Adcode: %s\r\n", sysdata.adcode);

}

/**
 * 该函数根据传入的字符串命令设置系统数据。
 * 支持以下命令：
 * - "-always"：进入无限循环，每0.1秒执行一次任务。
 * - "-h"：输出帮助信息。
 * - "tstp <timestamp>"：设置时间戳。
 * - "ip <ip_address>"：设置IP地址。
 * - "stmp <ShtTemp>"：设置温度。
 * - "shum <ShtHumi>"：设置湿度。
 * - "cw <CurWeather>"：设置当前天气代码。
 * - "ctmp <CurTemp>"：设置当前温度。
 * - "adcd <adcode>"：设置行政区划代码。
 * 未知命令将输出 "Unknown command"。
 */
void set_proc(char* str) {
	sysdata.ScreenReflesh=true;
    if (strstr(str, "-always")) {
        while (1) {
            // 执行需要循环执行的任务
            //delay_ms(100); // 延时0.1秒
        }
    } else {
        if (strstr(str, "-h")) {
            printf("use [set <var> <val>] to set values\r\n");
            return;
        }

        if (strstr(str, "tstp")) {
            uint32_t timestamp;
            if (sscanf(str, "tstp %u", &timestamp) == 1) {
                sysdata.timestamp = timestamp;
                printf("timestamp set to %d\r\n", sysdata.timestamp);
            }
        } else if (strstr(str, "ip")) {
            char ip_address[16];
            if (sscanf(str, "ip %15s", ip_address) == 1) {
                strncpy((char *)sysdata.ip_address, ip_address, sizeof(sysdata.ip_address));
                printf("ip_address set to %s\r\n", sysdata.ip_address);
            }
        } else if (strstr(str, "stmp")) {
            float ShtTemp;
            if (sscanf(str, "stmp %f", &ShtTemp) == 1) {
                sysdata.ShtTemp = ShtTemp;
                printf("ShtTemp set to %.2f\r\n", sysdata.ShtTemp);
            }
        } else if (strstr(str, "shum")) {
            float ShtHumi;
            if (sscanf(str, "shum %f", &ShtHumi) == 1) {
                sysdata.ShtHumi = ShtHumi;
                printf("ShtHumi set to %.2f\r\n", sysdata.ShtHumi);
            }
        } else if (strstr(str, "cw")) {
            uint8_t CurWeather;
            if (sscanf(str, "cw %hhu", &CurWeather) == 1) {
                sysdata.CurWeather = CurWeather;
                printf("CurWeather set to %d\r\n", sysdata.CurWeather);
            }
        } else if (strstr(str, "ctmp")) {
            float CurTemp;
            if (sscanf(str, "ctmp %f", &CurTemp) == 1) {
                sysdata.CurTemp = CurTemp;
                printf("CurTemp set to %.2f\r\n", sysdata.CurTemp);
            }
        } else if (strstr(str, "adcd")) {
            char adcode[12];
            if (sscanf(str, "adcd %11s", adcode) == 1) {
                strncpy((char *)sysdata.adcode, adcode, sizeof(sysdata.adcode));
                printf("adcode set to %s\r\n", sysdata.adcode);
            }
        } else {
            printf("Unknown command\r\n");
        }
    }
}



ST_CMD cmd_list[] =
    {
        {"?", help_proc},

        {"led", led_proc},

        {"showall", showall_proc},
		
		{"set",set_proc}
};

// 命令?处理函数
void help_proc(char *str)
{
  int i, size = sizeof(cmd_list) / sizeof(ST_CMD);

  printf("this is helper~\r\n");
  for (i = 0; i < size; i++)
  {
    printf("%s", (unsigned char *)cmd_list[i].cmd);
    printf("\r\n");
  }
}

unsigned char debug_rev_buf[64];

unsigned char rev_cnt = 0;
extern unsigned char rev_cnt;
char *skip(char *buf)
{
  while (*buf == ' ')
  {
    buf++;
  }
  return buf;
}

void debug_cmd_proc(char *buf)
{
  int i, size = sizeof(cmd_list) / sizeof(ST_CMD);
  int found = 0; // 添加一个标志变量，表示是否找到了匹配的命令

  for (i = 0; i < size; i++)
  {
    int len = strlen(cmd_list[i].cmd);

    if (strncmp(buf, cmd_list[i].cmd, len) == 0)
    {
      char *s = skip(buf + len);
      cmd_list[i].cb(s);
      found = 1; // 找到了匹配的命令
      break;
    }
  }
  if (!found)
  {
    // 如果没有找到匹配的命令，则发送"No command found\r\n"
    printf("%s",(unsigned char *)buf);
    printf(": command not found\r\n");
  }
}

void at_proc(unsigned char ch)
{
  if (ch == 0x0C) // Ctrl+L，ASCII码为0x0C
  {
    // 清屏操作，发送换行符并清空接收缓冲区
    printf("\r\n");
    memset(debug_rev_buf, 0, 64);

    // 发送清屏命令，例如："\033[2J\033[H"
    printf("\033[2J\033[H");

    // 发送命令提示符
    printf("%s",cmd_prpt_str);

    return;
  }
  if (ch == 0x15) // Ctrl+U，ASCII码为0x15
  {
    // 清除当前行的输入内容，将接收缓冲区重置为命令提示符后的位置
    rev_cnt = 0;
    memset(debug_rev_buf, 0, 64);

    // 发送换行符并重复发送命令提示符
    printf("\r\n");
    printf("%s",cmd_prpt_str);

    return;
  }
  if (ch > 0x7e) // 不合法字符
  {

    return;
  }
  if (ch < 0x20) // 发送完成
  {
    //            USART1_send1_buf_string("end");
    printf("\r\n");

    if (rev_cnt)
    {
      rev_cnt = 0;
      debug_cmd_proc((char *)debug_rev_buf);
      memset(debug_rev_buf, 0, 64);
    }
    // 在用户输入的那一行加上命令提示符
    printf("%s",cmd_prpt_str);
  }
  else
  {

    debug_rev_buf[rev_cnt] = ch;
    rev_cnt++;
  }
  if (rev_cnt >= 64)
  {
    rev_cnt = 0;
    memset(debug_rev_buf, 0, 64);
  }
}
