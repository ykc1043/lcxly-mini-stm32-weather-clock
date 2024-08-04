# sweather.py
import subprocess
import requests
import serial
import time
from config import GAODE_API_KEY, SERIAL_PORT, BAUDRATE, AUTO_LOCATION, ADCODE
from weather_codes import get_weather_code

def get_current_ip():
    curl_command = 'curl ip.sb -4'
    try:
        output = subprocess.check_output(curl_command, shell=True)
        ip = output.decode('utf-8').strip()
        return ip
    except subprocess.CalledProcessError as e:
        print(f"Error getting current IP: {e}")
        return None

def get_location_info(ip, key):
    url = "https://restapi.amap.com/v3/ip"
    params = {
        'ip': ip,
        'key': key
    }
    try:
        response = requests.get(url, params=params, proxies={"http": None, "https": None})
        response.raise_for_status()
        data = response.json()
        if data['status'] == '1':
            return data['city'], data['adcode']
        else:
            print(f"Error in API response: {data['info']}")
            return None
    except requests.RequestException as e:
        print(f"Error getting location info: {e}")
        return None

def get_weather_info(adcode, key):
    url = "https://restapi.amap.com/v3/weather/weatherInfo"
    params = {
        'city': adcode,
        'key': key,
        'extensions': 'base',
        'output': 'JSON'
    }
    try:
        response = requests.get(url, params=params, proxies={"http": None, "https": None})
        response.raise_for_status()
        data = response.json()
        if data['status'] == '1' and data['count'] == '1':
            weather_info = data['lives'][0]
            weather = weather_info['weather']
            temperature = weather_info['temperature']
            return weather, temperature
        else:
            print(f"Error in weather API response: {data['info']}")
            return None
    except requests.RequestException as e:
        print(f"Error getting weather info: {e}")
        return None

def send_to_serial(port, baudrate, adcode, weather_code, temperature):
    try:
        with serial.Serial(port, baudrate, timeout=1) as ser:
            ser.write(f"set adcd {adcode}\n".encode('utf-8'))
            time.sleep(0.1)
            ser.write(f"set cw {weather_code}\n".encode('utf-8'))
            time.sleep(0.1)
            ser.write(f"set ctmp {temperature}\n".encode('utf-8'))
            time.sleep(0.1)
    except serial.SerialException as e:
        print(f"Error communicating with serial port: {e}")

def main():
    if AUTO_LOCATION:
        ip = get_current_ip()
        if ip:
            print(f"Current IP: {ip}")
            location_info = get_location_info(ip, GAODE_API_KEY)
            if location_info:
                city, adcode = location_info
                print(f"City: {city}, Adcode: {adcode}")
            else:
                print("Failed to get location info.")
                return
        else:
            print("Failed to get current IP.")
            return
    else:
        adcode = ADCODE
        print(f"Using predefined adcode: {adcode}")
    
    weather_info = get_weather_info(adcode, GAODE_API_KEY)
    if weather_info:
        weather, temperature = weather_info
        weather_code = get_weather_code(weather)
        print(f"Weather: {weather} (Code: {weather_code}), Temperature: {temperature}°C")
        send_to_serial(SERIAL_PORT, BAUDRATE, adcode, weather_code, temperature)
    else:
        print("Failed to get weather info.")

if __name__ == "__main__":
    main()
