# stimestamp.py
import serial
import time
import config

def get_current_timestamp():
    # 获取当前时间戳（秒）
    return int(time.time())

def send_timestamp(serial_port):
    timestamp = get_current_timestamp()
    message2 = f"set tstp {timestamp}\n"
    print(message2.encode('utf-8'))    
    serial_port.write(message2.encode('utf-8'))

def main():
    # 从配置文件中读取串口参数
    port = config.SERIAL_PORT
    baudrate = config.BAUDRATE

    # 打开串口
    ser = serial.Serial(port, baudrate, timeout=1)
    if not ser.is_open:
        print(f"无法打开串口 {port}")
        return

    try:
        # 发送时间戳
        send_timestamp(ser)
        print(f"已发送时间戳: {get_current_timestamp()}")
    finally:
        # 关闭串口
        ser.close()

if __name__ == "__main__":
    main()
