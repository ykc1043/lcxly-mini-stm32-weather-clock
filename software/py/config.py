# config.py
GAODE_API_KEY = "35b23add63ce24bcca064d99d802ccad"
SERIAL_PORT = 'COM6'
BAUDRATE = 115200
AUTO_LOCATION = True  # Set to False to use a predefined adcode
ADCODE = '110000'  # Default adcode for manual location beijing

# Schedule in crontab format
# Each tuple consists of (minute, hour, day_of_month, month, day_of_week, script)
SCHEDULES = [
    ("0 * * * *", "sip.py"),           # Every hour
    ("0 */2 * * *", "stimestamp.py"),  # Every two hours
    ("0 */6 * * *", "sweather.py"),    # Every six hours
]
