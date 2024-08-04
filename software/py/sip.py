# sip.py
import subprocess
import serial
import time
from config import SERIAL_PORT,BAUDRATE
def get_current_ip():
    curl_command = 'curl ip.sb -4'
    try:
        output = subprocess.check_output(curl_command, shell=True)
        ip = output.decode('utf-8').strip()
        return ip
    except subprocess.CalledProcessError as e:
        print(f"Error getting current IP: {e}")
        return None

def send_ip_to_serial_port(ip):
    try:
        # 打开串口 COM5，波特率 115200
        ser = serial.Serial(SERIAL_PORT, BAUDRATE, timeout=1)
        #time.sleep(2)  # 给串口一些时间来建立连接
        command = f'set ip {ip}\n'
        ser.write(command.encode('utf-8'))
        print(f"Sent command: {command}")
        ser.close()
    except serial.SerialException as e:
        print(f"Error opening or writing to serial port: {e}")

def main():
    ip = get_current_ip()
    if ip:
        send_ip_to_serial_port(ip)
    else:
        print("Failed to retrieve current IP address.")

if __name__ == "__main__":
    main()
