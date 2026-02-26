import serial
import time
import psutil
import sys
import random
import subprocess
import os
from datetime import timedelta

PORT = "/dev/ttyUSB0"
BAUD = 115200

def format_bytes(n):
    if not n:
        return "0 B/s"
    for u in ["B/s", "KB/s", "MB/s", "GB/s", "TB/s"]:
        if n < 1024:
            return f"{n:.1f} {u}"
        n /= 1024
    return f"{n:.1f} PB/s"

def uptime():
    boot = psutil.boot_time()
    s = int(time.time() - boot)
    return str(timedelta(seconds=s)).replace(" day, ", "d ").replace(" days, ", "d ")

def lock():
    print("locking")
    subprocess.run("loginctl lock-session || xdg-screensaver lock || gnome-screensaver-command -l", shell=True)

def mute():
    print("mute toggle")
    subprocess.run("pactl set-sink-mute @DEFAULT_SINK@ toggle", shell=True)

def killhog():
    top = 0
    target = None
    for p in psutil.process_iter(["pid", "name", "cpu_percent"]):
        try:
            pid = p.info["pid"]
            if pid < 1000 or pid == os.getpid():
                continue
            c = p.info["cpu_percent"]
            if c > top:
                top = c
                target = p
        except:
            pass
    if target and top > 10:
        print("killing", target.info["name"], target.info["pid"])
        try:
            target.terminate()
        except:
            pass
    else:
        print("nothing to kill")

def main():
    try:
        print("connecting", PORT)
        s = serial.Serial(PORT, BAUD, timeout=1, dsrdtr=True)
        s.setDTR(False)
        time.sleep(0.1)
        s.setDTR(True)

        start = time.time()
        ok = False
        while time.time() - start < 8:
            if s.in_waiting:
                line = s.readline().decode(errors="ignore").strip()
                if line == "READY":
                    ok = True
                    break

        if not ok:
            print("no ready")
            s.close()
            sys.exit(1)

    except Exception as e:
        print("serial error", e)
        sys.exit(1)

    last = psutil.net_io_counters()

    try:
        while True:
            while s.in_waiting:
                line = s.readline().decode(errors="ignore").strip()
                if line == "BTN:LOCK":
                    lock()
                elif line == "BTN:MUTE":
                    mute()
                elif line == "BTN:KILL":
                    killhog()

            up = uptime()
            cpu = int(psutil.cpu_percent())
            mem = int(psutil.virtual_memory().percent)

            cur = psutil.net_io_counters()
            up_speed = cur.bytes_sent - last.bytes_sent
            down_speed = cur.bytes_recv - last.bytes_recv
            last = cur

            ds = format_bytes(down_speed)
            us = format_bytes(up_speed)

            glitch = 1 if cpu > 90 or random.random() < 0.005 else 0

            s.write(f"UPTIME:{up}\n".encode())
            s.write(f"NET:{ds}|{us}\n".encode())
            s.write(f"STATS:{cpu}|{mem}\n".encode())
            if glitch:
                s.write(b"GLITCH:1\n")

            print(cpu, mem)
            time.sleep(1)

    except KeyboardInterrupt:
        pass
    finally:
        s.close()

if __name__ == "__main__":
    main()
