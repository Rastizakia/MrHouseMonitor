🏠 MrHouseMonitor

Mr House on your 2.4" CYD display — a sleek, interactive smart display interface for ESP32.

⚡ Features

Real-time display on 2.4" TFT

Fully customizable GUI using LVGL

Optimized graphics via TFT_eSPI

Easy integration with images and Python scripts

📂 Important Files
File	Location	Purpose
lv_conf.h	~/Arduino/libraries	LVGL configuration (GUI features, memory, behavior)
User_Setup.h	~/Arduino/libraries/TFT_eSPI	Display & SPI settings for TFT_eSPI
Image files	Same folder as .ino	Assets displayed on the screen

⚠️ Make sure to keep files in these exact locations to avoid errors.

🛠 Setup Instructions

Install Required Libraries

Via Arduino IDE Library Manager:

LVGL

TFT_eSPI

Configure Your Board

Select ESP32 Dev Module in Arduino IDE.

Place Files Correctly

lv_conf.h → ~/Arduino/libraries

User_Setup.h → ~/Arduino/libraries/TFT_eSPI

Images → same folder as your .ino file

Python Port Setup

Ensure the serial port in the Python script matches your ESP32 connection.

💡 Notes

LVGL (lv_conf.h) → Controls GUI features, memory allocation, and display behavior.

TFT_eSPI (User_Setup.h) → Configures SPI pins and display type for fast rendering.

Images → Store in .ino folder for easy loading.

Keeping proper file structure avoids compilation/runtime issues.
