import schedule
import subprocess
import asyncio
from config import SCHEDULES

def job(script):
    subprocess.run(["python", script])

async def run_all_scripts_once():
    for cron_expr, script in SCHEDULES:
        job(script)
        await asyncio.sleep(10)  # Wait for 10 seconds before running the next script

def schedule_jobs():
    for cron_expr, script in SCHEDULES:
        minute, hour, day_of_month, month, day_of_week = cron_expr.split()
        
        if minute == "*" and hour == "*" and day_of_month == "*" and month == "*" and day_of_week == "*":
            schedule.every().minute.do(job, script).tag(script)
        if minute != "*" and hour == "*" and day_of_month == "*" and month == "*" and day_of_week == "*":
            schedule.every(int(minute)).minutes.do(job, script).tag(script)
        if minute != "*" and hour != "*" and day_of_month == "*" and month == "*" and day_of_week == "*":
            schedule.every(int(hour)).hours.at(f":{minute.zfill(2)}").do(job, script).tag(script)
        if day_of_month != "*":
            schedule.every(int(day_of_month)).days.do(job, script).tag(script)
        if month != "*":
            schedule.every(int(month)).months.do(job, script).tag(script)
        if day_of_week != "*":
            schedule.every().day.at(f"{hour.zfill(2)}:{minute.zfill(2)}").do(job, script).tag(script)

async def main():
    await run_all_scripts_once()
    schedule_jobs()
    while True:
        schedule.run_pending()
        time_until_next_run = schedule.idle_seconds()
        if time_until_next_run is None:
            # No jobs scheduled, sleep for a while
            await asyncio.sleep(60)
        elif time_until_next_run > 0:
            # Sleep until the next scheduled job
            await asyncio.sleep(time_until_next_run)

if __name__ == "__main__":
    asyncio.run(main())
